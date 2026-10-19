# Staged firmware bring-up

Use one ESP32 and add one subsystem at a time. These are temporary diagnostic sketches; the final player remains `fridge_nfc_player/fridge_nfc_player.ino`.

## Before flashing either sketch

- Install the libraries and select **ESP32 Dev Module** as described in `README.md`.
- Disconnect the power bank before connecting the ESP32 DevKit to the computer for programming.
- Upload one sketch at a time. Arduino IDE requires the `.ino` file to live in a same-named folder; these folders already follow that rule.
- Use Serial Monitor at **115200 baud**.

## Stage A: SD and audio only

1. Format the card as FAT32 and copy two supported 16-bit PCM, 44.1 kHz WAV files to `/music/001.wav` and `/music/002.wav`.
2. Connect the ESP32, SD breakout, MAX98357A, and speaker using `../wiring.md`. Leave the PN532 and buttons disconnected.
3. Check the 3.3 V and 5 V rails with a meter before connecting the modules. Keep the amp gain low and the speaker connected only across OUT+ and OUT−.
4. Open `audio_bringup/audio_bringup.ino`, compile, and upload it.
5. Pass when Serial Monitor reports the card mounted and both tracks play in order without resets or audible glitches. If a track fails, stop and resolve the SD/audio path before adding NFC.

The sketch auto-plays `/music/001.wav`, then `/music/002.wav`, and repeats. It uses the same SPI/I²S pins as the final firmware and starts at conservative software gain.

## Stage B: PN532 and tags only

1. Power off and disconnect the MAX98357A and speaker. Connect only the ESP32 and PN532 using the 3.3 V rail and common ground.
2. Set the PN532 V3 switches to I²C according to the module's silk-screen/manual. Connect SDA to GPIO21 and SCL to GPIO22.
3. Open `nfc_bringup/nfc_bringup.ino`, compile, and upload it.
4. Pass when startup reports the PN532 firmware and each tag UID appears once when presented. Remove a tag and present it again to confirm it is detected again.
5. Record the UIDs in `../tag_map.csv`. Check NFC range on the intended fridge surface before fixing the reader in place.

## Stage C: Combined player and controls

1. Power off. Reconnect the SD breakout, PN532, MAX98357A, and speaker according to `../wiring.md`.
2. Replace UID placeholders in `fridge_nfc_player/tag_map.h` with the recorded UIDs. Confirm every mapped file exists on the card.
3. Upload `fridge_nfc_player/fridge_nfc_player.ino`.
4. Connect the three normally-open buttons: GPIO32 play/stop, GPIO33 next, GPIO14 previous; the other terminal of each button goes to GND.
5. Pass when a known tag selects its mapped song, a held tag does not repeatedly restart playback, an unknown tag is reported without starting another track, and all three buttons work while playing.

## Stage D: battery and mounting acceptance

- Run the combined player from the power bank for a full track and through stopped/idle time. Confirm the bank does not switch off and the ESP32 does not reset.
- Increase volume gradually. Stop if there is clipping, brownout, unexpected heat, SD errors, or unstable NFC reads.
- Test NFC repeatedly with the enclosure in its final position and on the fridge. Adjust antenna spacing/ferrite only after recording baseline range; do not glue the reader until it passes.
- Mount only after all prior stages pass. This computer-side package cannot verify the physical modules, power-bank behavior, sound quality, or on-fridge NFC range.
