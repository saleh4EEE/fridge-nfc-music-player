# Firmware setup

## Toolchain and libraries

1. Install Arduino IDE 2.x and the Espressif `esp32` board package.
2. Select **ESP32 Dev Module** (classic ESP32, FQBN `esp32:esp32:esp32`) for the received WROOM board; set the serial upload port and a conservative upload speed if the clone is unreliable.
3. Install **ESP8266Audio 2.4.2** from the upstream Earle Philhower project.
4. Install the **Elechouse PN532** library from its official `PN532` repository. Copy the repository's `PN532` and `PN532_I2C` library folders into the Arduino `libraries` directory; the code uses their `PN532_I2C` and `PN532` classes.
5. Open `fridge_nfc_player/fridge_nfc_player.ino`. Keep `tag_map.h` in that same sketch folder; Arduino IDE expects the sketch folder and main `.ino` file to share the name `fridge_nfc_player`.

Upstream references: [ESP8266Audio releases](https://github.com/earlephilhower/ESP8266Audio/releases), [ESP8266Audio SD source API](https://github.com/earlephilhower/ESP8266Audio/blob/master/src/AudioFileSourceSD.h), [Elechouse PN532 library](https://github.com/elechouse/PN532).

The starter uses `AudioFileSourceSD`, `AudioGeneratorWAV`, and `AudioOutputI2S`. These are the library interfaces for SD-backed WAV playback, I²S pin selection, and the PN532 I²C driver. The SD card is accessed using Arduino-ESP32 `SD` and `SPI`. See `compile_info.md` for the successful compile configuration and its physical-test limits.

## Configure and upload

- Wire exactly as in `../wiring.md`.
- Set the PN532 V3 board to I²C, not SPI or HSU.
- Set `MAX_GAIN_LINEAR` in the sketch conservatively. It is software gain, not a calibrated SPL control. Start at `0.20f` and increase only if the signal remains clean.
- Replace the UID placeholders in `fridge_nfc_player/tag_map.h` with the actual UID strings printed to Serial Monitor. Use uppercase hexadecimal and no spaces.
- Confirm every SD path exists. Keep 16-bit PCM dual-mono WAV files at 44.1 kHz in `/music/`.
- Compile before flashing, then use the bring-up checklist. Disconnect the power bank before connecting the DevKit to the PC for programming.

## Firmware behavior

- PN532 scanning runs in a low-priority FreeRTOS task on the other ESP32 core. It sends newly detected UIDs to the main loop through a queue, so the blocking reader call does not run inside the audio servicing loop.
- A tag starts its mapped song. Holding a tag does not intentionally restart the same song. Removing it and presenting it again selects it again.
- Play/stop toggles between playing and stopping. Stop restarts from the beginning the next time play is pressed; this starter does not implement seek-and-resume pause.
- Next/previous cycles through the entries in `fridge_nfc_player/tag_map.h`.
- Unknown tag UIDs are printed to Serial Monitor; copy that value into `tag_map.h`, set its path to an existing WAV, then upload again.
- The starter has no screen or status LED. Add UI only after the audio and tag path works.

## Known limits

This source is a starter, not measured firmware for your particular clones. It compiled with the exact configuration in `compile_info.md`, but has not been run on the delivered modules. If you change the library or ESP32 core versions and get compile diagnostics, fix them before powering the amp. The I²S audio chain and pin mapping are compatible on paper; actual SD signal quality, PN532 clone behavior, MAX98357A mode straps, power-bank auto-off, and NFC range on steel require physical checks.

