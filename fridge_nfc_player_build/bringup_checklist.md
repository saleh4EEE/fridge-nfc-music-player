# Bring-up checklist

Complete these in order. Disconnect the power bank before changing wiring. Never connect a speaker lead to GND on the MAX98357A.

## 1. Inspect modules, unpowered

- [ ] Read the silkscreen on the ESP32 board and confirm it is a classic ESP32 WROOM DevKit with Micro-USB.
- [ ] Identify the PN532 V3 I²C switch position from the board’s own markings/manual.
- [ ] Confirm the microSD module is 3.3 V logic and identify SCK, MOSI/DI, MISO/DO, CS, VCC, GND.
- [ ] Confirm the audio module is MAX98357A and locate VIN, GND, BCLK, LRC/WS, DIN, SD/MODE, GAIN and OUT+/OUT−.
- [ ] Confirm speaker impedance is 4 Ω or greater.
- [ ] Confirm the 3.3 V regulator input/output polarity and rated output current.
- [ ] Leave GPIO16/17 unused. Leave PN532 IRQ and reset disconnected for the initial build.

## 2. Wire and inspect power only

- [ ] Use one 5 V USB power bank rated at least 2 A.
- [ ] Branch the same 5 V source to DevKit Micro-USB and the peripheral rail using a rated splitter/distribution adapter. Do not connect separate power banks.
- [ ] Route peripheral 5 V to MAX98357A VIN and the 3.3 V regulator input.
- [ ] Route regulator 3.3 V to PN532 VCC and the 3.3 V SPI SD breakout VCC.
- [ ] Join all grounds: ESP32, PN532, SD, regulator, and audio board.
- [ ] Check that local supply capacitors are fitted: amp-side bulk cap across MAX VIN/GND and 3.3 V decoupling near SD/peripheral rail; observe electrolytic polarity.
- [ ] Check with a meter that the rail is 5 V and the regulated peripheral rail is 3.3 V before inserting modules.
- [ ] Do not also connect the DevKit to a computer while the bank powers it. For programming, disconnect the bank, then connect the PC USB cable.

## 3. Test the audio path by itself

- [ ] Format a genuine 16 GB card as FAT32.
- [ ] Create `/music/` and copy one standard 16-bit PCM WAV file into it.
- [ ] Wire SPI: GPIO18 SCK, GPIO23 MOSI→DI, GPIO19 MISO←DO, GPIO13 CS.
- [ ] If the SD board has no CS pull-up, add 10 kΩ from CS to 3.3 V.
- [ ] Leave the PN532 and buttons disconnected for this test.
- [ ] Put `/music/001.wav` and `/music/002.wav` on the card.
- [ ] Run `firmware/audio_bringup/audio_bringup.ino` at low gain; verify both tracks play in order before adding NFC.

## 4. Test PN532 by itself

- [ ] Wire SDA GPIO21 and SCL GPIO22; set PN532 V3 to I²C.
- [ ] Keep the reader antenna behind plastic and away from the speaker magnet and amp wires.
- [ ] Read every token UID at the Serial Monitor. NTAG21x UID is commonly 7 bytes; firmware uses the reported length.
- [ ] Test each actual token against the actual fridge, first without then with ferrite. Do not glue until repeated reads are reliable.

## 5. Combine audio, NFC, buttons, and battery

- [ ] Wire play/stop GPIO32, next GPIO33, previous GPIO14; other side of each button to GND.
- [ ] Replace UID placeholders in `firmware/fridge_nfc_player/tag_map.h`; match each file path to files on the SD card.
- [ ] Upload `firmware/fridge_nfc_player/fridge_nfc_player.ino` only after the isolated SD/audio and NFC checks pass.
- [ ] Verify one token selects one track; holding the token should not continuously restart it.
- [ ] Verify next/previous cycles only the tracks configured in the map.
- [ ] Increase volume gradually; stop if audible clipping, brownout, hot wiring, SD errors or amp overheating occurs.
- [ ] Test playback from the power bank through a full track, including paused/stopped idle, and check whether the bank auto-switches off.
- [ ] Only after these checks, close the case and verify the fridge mounting is secure.

## Stop conditions

Stop and correct the wiring if any GPIO sees 5 V, the 3.3 V rail is out of tolerance, the amplifier speaker output is tied to ground, the board heats unexpectedly, or the power bank resets. Do not try the loose pouch cell as a workaround.

