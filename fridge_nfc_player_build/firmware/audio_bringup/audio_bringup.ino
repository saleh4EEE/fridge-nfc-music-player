#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#include "AudioFileSourceSD.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"

// Classic ESP32 WROOM pin map from ../wiring.md.
static constexpr int PIN_SD_SCK = 18;
static constexpr int PIN_SD_MISO = 19;
static constexpr int PIN_SD_MOSI = 23;
static constexpr int PIN_SD_CS = 13;
static constexpr int PIN_I2S_BCLK = 26;
static constexpr int PIN_I2S_LRCLK = 25;
static constexpr int PIN_I2S_DOUT = 27;
static constexpr uint32_t SD_SPI_HZ = 10000000;
static constexpr float TEST_GAIN = 0.15f;
static constexpr const char *TEST_TRACKS[] = {
    "/music/001.wav",
    "/music/002.wav",
};
static constexpr size_t TEST_TRACK_COUNT = sizeof(TEST_TRACKS) / sizeof(TEST_TRACKS[0]);

static AudioOutputI2S *audioOut = nullptr;
static AudioGeneratorWAV *wav = nullptr;
static AudioFileSourceSD *audioFile = nullptr;
static size_t trackIndex = 0;
static bool sdReady = false;

static void stopTrack() {
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
  if (!sdReady || index >= TEST_TRACK_COUNT) return false;
  stopTrack();

  Serial.printf("Opening test track: %s\n", TEST_TRACKS[index]);
  audioFile = new AudioFileSourceSD(TEST_TRACKS[index]);
  if (audioFile == nullptr || !audioFile->isOpen()) {
    Serial.printf("ERROR: could not open %s\n", TEST_TRACKS[index]);
    stopTrack();
    return false;
  }

  wav = new AudioGeneratorWAV();
  if (wav == nullptr || !wav->begin(audioFile, audioOut)) {
    Serial.printf("ERROR: could not start WAV decoder for %s\n", TEST_TRACKS[index]);
    stopTrack();
    return false;
  }
  Serial.println("Playback started. Keep volume low for this first test.");
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(250);
  Serial.println("Audio bring-up: SD -> ESP32 WAV -> I2S -> MAX98357A");

  SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
  sdReady = SD.begin(PIN_SD_CS, SPI, SD_SPI_HZ);
  if (!sdReady) {
    Serial.println("ERROR: SD mount failed. Check 3.3 V, SPI wiring, CS, and FAT32.");
    return;
  }
  Serial.println("microSD mounted");

  audioOut = new AudioOutputI2S();
  audioOut->SetPinout(PIN_I2S_BCLK, PIN_I2S_LRCLK, PIN_I2S_DOUT);
  audioOut->SetGain(TEST_GAIN);

  if (!startTrack(trackIndex)) {
    Serial.println("Check that /music/001.wav and /music/002.wav exist and are supported PCM WAV files.");
  }
}

void loop() {
  if (!sdReady || wav == nullptr || !wav->isRunning()) return;

  if (!wav->loop()) {
    Serial.printf("Track %s finished.\n", TEST_TRACKS[trackIndex]);
    stopTrack();
    trackIndex = (trackIndex + 1) % TEST_TRACK_COUNT;
    if (!startTrack(trackIndex)) {
      Serial.println("Audio test stopped after a playback error; inspect the card and wiring.");
      sdReady = false;
    }
  }
}
