#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include <PN532_I2C.h>
#include <PN532.h>

#include "AudioFileSourceSD.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include "tag_map.h"

// Classic ESP32-WROOM-32E DevKit GPIO map. GPIO16/17 are intentionally unused:
// some WROOM-32E suffix variants include PSRAM that reserves them.
static constexpr int PIN_NFC_SDA = 21;
static constexpr int PIN_NFC_SCL = 22;
static constexpr int PIN_SD_SCK = 18;
static constexpr int PIN_SD_MISO = 19;
static constexpr int PIN_SD_MOSI = 23;
static constexpr int PIN_SD_CS = 13;
static constexpr int PIN_I2S_BCLK = 26;
static constexpr int PIN_I2S_LRCLK = 25;
static constexpr int PIN_I2S_DOUT = 27;
static constexpr int PIN_BUTTON_PLAY_STOP = 32;
static constexpr int PIN_BUTTON_NEXT = 33;
static constexpr int PIN_BUTTON_PREVIOUS = 14;

static constexpr uint32_t SD_SPI_HZ = 10000000;
static constexpr float MAX_GAIN_LINEAR = 0.20f;
static constexpr uint16_t NFC_RESPONSE_TIMEOUT_MS = 25;
static constexpr uint32_t NFC_REMOVAL_DEBOUNCE_MS = 700;
static constexpr uint32_t BUTTON_DEBOUNCE_MS = 30;

struct TagEvent {
  uint8_t uidLength;
  uint8_t uid[7];
};

struct DebouncedButton {
  uint8_t pin;
  bool stableHigh;
  bool lastRawHigh;
  uint32_t lastChangeMs;
};

static PN532_I2C pn532I2c(Wire);
static PN532 nfc(pn532I2c);
static QueueHandle_t tagQueue = nullptr;
static TaskHandle_t nfcTaskHandle = nullptr;

static AudioOutputI2S *audioOut = nullptr;
static AudioGeneratorWAV *wav = nullptr;
static AudioFileSourceSD *audioFile = nullptr;

static bool sdReady = false;
static int currentTrack = -1;

static DebouncedButton buttonPlayStop{PIN_BUTTON_PLAY_STOP, true, true, 0};
static DebouncedButton buttonNext{PIN_BUTTON_NEXT, true, true, 0};
static DebouncedButton buttonPrevious{PIN_BUTTON_PREVIOUS, true, true, 0};

static bool wasPressed(DebouncedButton &button, uint32_t now) {
  const bool rawHigh = digitalRead(button.pin) == HIGH;
  if (rawHigh != button.lastRawHigh) {
    button.lastRawHigh = rawHigh;
    button.lastChangeMs = now;
  }

  if ((now - button.lastChangeMs) >= BUTTON_DEBOUNCE_MS &&
      rawHigh != button.stableHigh) {
    button.stableHigh = rawHigh;
    return !button.stableHigh;  // active-low press edge
  }
  return false;
}

static void printUid(const uint8_t *uid, uint8_t uidLength, char *out, size_t outSize) {
  if (outSize == 0) return;
  size_t pos = 0;
  for (uint8_t i = 0; i < uidLength && (pos + 2) < outSize; ++i) {
    const int wrote = snprintf(out + pos, outSize - pos, "%02X", uid[i]);
    if (wrote != 2) break;
    pos += 2;
  }
  out[pos] = '\0';
}

static int findTrackByUid(const char *uidHex) {
  for (size_t i = 0; i < TAG_TRACK_COUNT; ++i) {
    if (strcmp(TAG_TRACKS[i].uidHex, uidHex) == 0) return static_cast<int>(i);
  }
  return -1;
}

static void stopPlayback() {
  if (wav != nullptr) {
    if (wav->isRunning()) wav->stop();
    delete wav;
    wav = nullptr;
  }
  if (audioFile != nullptr) {
    audioFile->close();
    delete audioFile;
    audioFile = nullptr;
  }
}

static bool startTrack(size_t index) {
  if (!sdReady || index >= TAG_TRACK_COUNT) return false;

  stopPlayback();
  currentTrack = static_cast<int>(index);

  audioFile = new AudioFileSourceSD(TAG_TRACKS[index].path);
  if (audioFile == nullptr || !audioFile->isOpen()) {
    Serial.printf("ERROR: cannot open %s\n", TAG_TRACKS[index].path);
    if (audioFile != nullptr) {
      delete audioFile;
      audioFile = nullptr;
    }
    return false;
  }

  wav = new AudioGeneratorWAV();
  if (wav == nullptr || !wav->begin(audioFile, audioOut)) {
    Serial.printf("ERROR: WAV decoder could not start %s\n", TAG_TRACKS[index].path);
    stopPlayback();
    return false;
  }

  Serial.printf("Playing track %u: %s\n", static_cast<unsigned>(index + 1),
                TAG_TRACKS[index].path);
  return true;
}

static void togglePlayStop() {
  if (wav != nullptr && wav->isRunning()) {
    stopPlayback();
    Serial.println("Stopped. Play/stop starts the current track from the beginning.");
    return;
  }

  const size_t index = currentTrack >= 0 ? static_cast<size_t>(currentTrack) : 0;
  if (!startTrack(index)) Serial.println("No track started; check SD and tag_map.h.");
}

static void playRelativeTrack(int direction) {
  if (TAG_TRACK_COUNT == 0) return;
  int base = currentTrack;
  if (base < 0) base = (direction > 0) ? -1 : 0;
  int next = base + direction;
  if (next < 0) next = static_cast<int>(TAG_TRACK_COUNT) - 1;
  if (next >= static_cast<int>(TAG_TRACK_COUNT)) next = 0;
  startTrack(static_cast<size_t>(next));
}

static void handleTag(const TagEvent &event) {
  char uidHex[15] = {0};  // seven UID bytes -> fourteen hexadecimal characters + NUL
  printUid(event.uid, event.uidLength, uidHex, sizeof(uidHex));
  const int index = findTrackByUid(uidHex);
  if (index < 0) {
    Serial.printf("Unknown NFC UID: %s (add it to tag_map.h)\n", uidHex);
    return;
  }
  startTrack(static_cast<size_t>(index));
}

static void nfcTask(void *parameter) {
  (void)parameter;
  uint8_t previousUid[7] = {0};
  uint8_t previousLength = 0;
  bool previousPresent = false;
  uint32_t lastSeenMs = 0;

  for (;;) {
    uint8_t uid[7] = {0};
    uint8_t uidLength = 0;
    const bool found = nfc.readPassiveTargetID(
        PN532_MIFARE_ISO14443A, uid, &uidLength, NFC_RESPONSE_TIMEOUT_MS);
    const uint32_t now = millis();

    if (found && uidLength > 0 && uidLength <= sizeof(uid)) {
      bool sameUid = previousPresent && uidLength == previousLength;
      if (sameUid) {
        for (uint8_t i = 0; i < uidLength; ++i) {
          if (uid[i] != previousUid[i]) {
            sameUid = false;
            break;
          }
        }
      }

      if (!sameUid || !previousPresent || (now - lastSeenMs) > NFC_REMOVAL_DEBOUNCE_MS) {
        TagEvent event{};
        event.uidLength = uidLength;
        memcpy(event.uid, uid, uidLength);
        if (tagQueue != nullptr) xQueueSend(tagQueue, &event, 0);
      }
      memcpy(previousUid, uid, uidLength);
      previousLength = uidLength;
      previousPresent = true;
      lastSeenMs = now;
    } else if (previousPresent && (now - lastSeenMs) > NFC_REMOVAL_DEBOUNCE_MS) {
      previousPresent = false;
      previousLength = 0;
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup() {
  Serial.begin(115200);
  delay(250);
  Serial.println("Fridge NFC music player starting");

  pinMode(buttonPlayStop.pin, INPUT_PULLUP);
  pinMode(buttonNext.pin, INPUT_PULLUP);
  pinMode(buttonPrevious.pin, INPUT_PULLUP);

  SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
  sdReady = SD.begin(PIN_SD_CS, SPI, SD_SPI_HZ);
  if (!sdReady) {
    Serial.println("ERROR: microSD mount failed. Check 3.3 V, SPI wiring, CS, and FAT32 card.");
  } else {
    Serial.println("microSD mounted");
  }

  audioOut = new AudioOutputI2S();
  audioOut->SetPinout(PIN_I2S_BCLK, PIN_I2S_LRCLK, PIN_I2S_DOUT);
  audioOut->SetGain(MAX_GAIN_LINEAR);

  Wire.begin(PIN_NFC_SDA, PIN_NFC_SCL);
  nfc.begin();
  const uint32_t version = nfc.getFirmwareVersion();
  if (!version) {
    Serial.println("ERROR: PN532 not found. Check I2C switch, 3.3 V, SDA/SCL, and GND.");
  } else {
    Serial.printf("PN532 found, firmware 0x%08lX\n", static_cast<unsigned long>(version));
    nfc.SAMConfig();
    nfc.setPassiveActivationRetries(0x01);
    tagQueue = xQueueCreate(3, sizeof(TagEvent));
    if (tagQueue == nullptr) {
      Serial.println("ERROR: could not create NFC queue");
    } else {
      const BaseType_t taskOk = xTaskCreatePinnedToCore(
          nfcTask, "NFC reader", 4096, nullptr, 1, &nfcTaskHandle, 0);
      if (taskOk != pdPASS) Serial.println("ERROR: could not start NFC task");
    }
  }

  Serial.println("Ready. Scan a mapped tag. See firmware/tag_map.h and sd_card/README.md.");
}

void loop() {
  // Keep the audio decoder serviced on every main-loop pass.
  if (wav != nullptr && wav->isRunning() && !wav->loop()) {
    Serial.println("Track ended");
    stopPlayback();
  }

  TagEvent event{};
  if (tagQueue != nullptr && xQueueReceive(tagQueue, &event, 0) == pdTRUE) {
    handleTag(event);
  }

  const uint32_t now = millis();
  if (wasPressed(buttonPlayStop, now)) togglePlayStop();
  if (wasPressed(buttonNext, now)) playRelativeTrack(+1);
  if (wasPressed(buttonPrevious, now)) playRelativeTrack(-1);

  // No delay here: WAV decode/I2S output needs frequent servicing.
}

