# Firmware compile record

**Result:** successful compile.

| Setting | Value |
|---|---|
| Target | `esp32:esp32:esp32` (classic ESP32 Dev Module / WROOM family) |
| Arduino-ESP32 core | 3.3.12 |
| ESP8266Audio | 2.4.2 |
| PN532 library | Elechouse `PN532_HSU` branch, commit `8b05896a6616396392f0eac5af88a97c9733ebce` |
| Source | `fridge_nfc_player/fridge_nfc_player.ino` |
| Flash use | 379,690 bytes (28% of 1,310,720 bytes) |
| Static RAM | 24,536 bytes (7% of 327,680 bytes) |

The compile emitted upstream dependency warnings about unused variables/narrowing conversions in the Elechouse PN532 repository and an unused PDM output path in ESP8266Audio. The sketch itself had no compile errors. These warnings are not from the WAV/I²S code paths used here.

This checks compilation only. There is no hardware-in-loop confirmation yet: no power-up, tag scan, SD read, audio output, bank-runtime, thermal, or on-fridge RF test has been performed. Module clone revisions and speaker/enclosure behavior still need the checklist in `../bringup_checklist.md`.

