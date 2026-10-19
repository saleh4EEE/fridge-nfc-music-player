#include <Arduino.h>
#include <Wire.h>

#include <PN532_I2C.h>
#include <PN532.h>

static constexpr int PIN_NFC_SDA = 21;
static constexpr int PIN_NFC_SCL = 22;
static constexpr uint16_t NFC_TIMEOUT_MS = 25;
static constexpr uint32_t REMOVAL_DEBOUNCE_MS = 700;

static PN532_I2C pn532I2c(Wire);
static PN532 nfc(pn532I2c);

static void printUid(const uint8_t *uid, uint8_t uidLength) {
  for (uint8_t i = 0; i < uidLength; ++i) Serial.printf("%02X", uid[i]);
}

void setup() {
  Serial.begin(115200);
  delay(250);
  Serial.println("NFC bring-up: PN532 I2C UID reader");

  Wire.begin(PIN_NFC_SDA, PIN_NFC_SCL);
  nfc.begin();
  const uint32_t version = nfc.getFirmwareVersion();
  if (!version) {
    Serial.println("ERROR: PN532 not found. Check I2C mode, 3.3 V, SDA/SCL, and common GND.");
    while (true) delay(1000);
  }

  Serial.printf("PN532 found, firmware 0x%08lX\n", static_cast<unsigned long>(version));
  nfc.SAMConfig();
  nfc.setPassiveActivationRetries(0x01);
  Serial.println("Present one tag at a time. Copy each UID exactly into tag_map.h.");
}

void loop() {
  static uint8_t previousUid[7] = {0};
  static uint8_t previousLength = 0;
  static bool previousPresent = false;
  static uint32_t lastSeenMs = 0;

  uint8_t uid[7] = {0};
  uint8_t uidLength = 0;
  const bool found = nfc.readPassiveTargetID(
      PN532_MIFARE_ISO14443A, uid, &uidLength, NFC_TIMEOUT_MS);
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

    if (!sameUid || !previousPresent || (now - lastSeenMs) > REMOVAL_DEBOUNCE_MS) {
      Serial.print("Tag UID: ");
      printUid(uid, uidLength);
      Serial.println();
    }
    memcpy(previousUid, uid, uidLength);
    previousLength = uidLength;
    previousPresent = true;
    lastSeenMs = now;
  } else if (previousPresent && (now - lastSeenMs) > REMOVAL_DEBOUNCE_MS) {
    previousPresent = false;
    previousLength = 0;
    Serial.println("Tag removed; ready for next tag.");
  }
}
