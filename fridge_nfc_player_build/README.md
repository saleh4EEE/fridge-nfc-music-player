# NFC fridge music player — build package

This folder contains the complete module-prototype design package for the first no-custom-PCB version. The player is offline: an NFC token selects a song stored on a microSD card. It does not stream from a phone or store music on the tag.

## What is in this folder

- `shopping_list.md` — complete order checklist and what to confirm on each module.
- `wiring.md` and `wiring.svg` — module wiring, pin map, and power tree.
- `project_audit.md` — compatibility, costs, tradeoffs, and unresolved physical checks.
- `firmware/fridge_nfc_player/fridge_nfc_player.ino` — Arduino starter firmware for WAV playback, NFC lookup, and three buttons.
- `firmware/fridge_nfc_player/tag_map.h` and `firmware/tag_map.csv` — editable UID-to-file mapping template.
- `firmware/README.md` and `firmware/compile_info.md` — library setup, firmware configuration, compile result, and operating behavior.
- `firmware/STAGED_BRINGUP.md` — exact order for testing audio and NFC separately, then combining them.
- `firmware/audio_bringup/audio_bringup.ino` and `firmware/nfc_bringup/nfc_bringup.ino` — temporary diagnostic sketches for isolated subsystem checks.
- `sd_card/README.md` — SD formatting, file naming, and WAV export requirements.
- `bringup_checklist.md` — electrical inspection and final functional checks before mounting.

## Recommended architecture

```text
NTAG213 token → PN532 V3 (I²C) → ESP32-WROOM-32E
microSD breakout (SPI) → ESP32 WAV decoder → I²S → MAX98357A DAC/amp → mono speaker
5 V power bank → ESP32 DevKit + MAX98357A + external 3.3 V regulator
```

The MAX98357A is both the DAC and speaker amplifier. Do not add a second DAC or the DFPlayer Mini to this architecture. The microSD socket is on its own SPI breakout; the ESP32 board has no socket.

## Build status and honesty

The pin map and software interfaces are designed around documented module APIs. The included firmware compiled successfully for a classic ESP32 using Arduino-ESP32 3.3.12, ESP8266Audio 2.4.2, and the Elechouse PN532 library revision listed in `firmware/compile_info.md`. The exact low-cost clone revisions in the photos have not been bench-tested. Compilation confirms source/API compatibility; it does not verify the received SD, NFC, amplifier, speaker, battery bank, or NFC range on fridge steel. Confirm module labels and complete the bring-up checklist before closing the enclosure.

The illustrated pouch cell is not selected. Use a commercial 5 V power bank for the first build. The small 35 × 25 mm speaker will play music, but a larger full-range driver and a sealed rear chamber are the highest-value sound-quality improvements.

## Marketplace cart

No shopping cart has been changed. An earlier marketplace safety gate blocked marketplace cart actions, and that restriction remains in effect. The order-ready list is in `shopping_list.md`; it gives exact electrical requirements so you can add the items yourself.

