# Fridge NFC Music Player

A low-cost, offline, fridge-mounted music player prototype. NFC tokens select songs stored on a microSD card. The ESP32 reads WAV files and sends digital audio to a MAX98357A I²S DAC/amplifier.

## Start here

1. Read `fridge_nfc_player_build/README.md` for the project overview.
2. Follow `fridge_nfc_player_build/firmware/STAGED_BRINGUP.md` to test audio and NFC separately before combining them.
3. Use `fridge_nfc_player_build/wiring.md` and `fridge_nfc_player_build/bringup_checklist.md` for the exact wiring and safety checks.
4. Use `fridge_nfc_player_build/shopping_list.md` for components and sourcing references.

The build package includes the combined player firmware plus temporary audio-only and NFC-only diagnostic sketches. The design is intended for a classic ESP32-WROOM DevKit, PN532 V3 NFC reader, SPI microSD breakout, MAX98357A amplifier, and a 4 Ω or higher speaker.

## Current verification status

The combined firmware has a recorded successful compile in `fridge_nfc_player_build/firmware/compile_info.md`. The diagnostic sketches are provided for hardware bring-up. This design has not been verified on the exact physical modules, power bank, enclosure, or refrigerator; complete the staged tests before mounting it permanently.

## Design notes

- Offline playback: song files live on the microSD card; NFC tags store only identifiers.
- The MAX98357A combines the DAC and speaker amplifier.
- The 3.3 V-only microSD breakout and PN532 use the regulated 3.3 V rail. The ESP32 DevKit and amplifier use 5 V. All grounds are common.
- Speaker output connects only across the amplifier's OUT+ and OUT− terminals.
