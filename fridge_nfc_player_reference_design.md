# Superseded PCB Draft

This draft assumes a custom PCB. The current plan is a ready-made module build with no custom PCB: see [fridge_nfc_player_module_wiring.md](fridge_nfc_player_module_wiring.md). Do not use the battery charger/boost section below for the current build.

# Fridge NFC Music Player — Rev A Reference Design

This is a buildable logical design using the ESP32-WROOM-32E DevKitC shown in the uploaded AliExpress screenshot. It prioritizes clean audio and low-risk assembly. Use the DevKitC as a plug-in board on a carrier PCB for Rev A; a production PCB can later replace it with the ESP32-WROOM-32E module.

## 1. Architecture

```text
NTAG213 disc token
        │ 13.56 MHz NFC
        ▼
PN532 V3 (I²C) ──► ESP32-WROOM-32E ──► microSD (SPI, 16 GB FAT32)
                         │                         │ MP3/WAV bytes
                         │                         ▼
                         └──── I²S PCM ─────► MAX98357A (I²S DAC + mono Class-D amp)
                                                      │ OUTP / OUTN
                                                      ▼
                                                4 Ω speaker

Protected 1S LiPo → BQ24074 power-path charger → 5 V boost rail
                                                   ├─► MAX98357A
                                                   └─► ESP32 DevKitC 5V/VIN
```

**Audio decision:** omit the DFPlayer Mini in this sound-focused version. The ESP32 reads the card and decodes the file to PCM; the MAX98357A accepts digital I²S and contains both the DAC and speaker amplifier. This avoids the ESP32's low-resolution internal DAC and avoids relying on the analog output/amp quality of an inexpensive DFPlayer clone. A separate line-level DAC is not needed for this mono 3 W speaker design.

## 2. Parts and target specifications

| Ref | Part | Selection / target |
|---|---|---|
| U1 | MCU board | ESP32-WROOM-32E DevKitC clone from the screenshot; 30-pin/38-pin header version with PCB antenna. Micro-USB is fine for programming. Use the module, not S3, for this first build. |
| U2 | NFC reader | PN532 V3 breakout, set to I²C mode. Use 3.3 V logic; check the exact breakout's VCC marking before powering it. |
| U3 | Audio DAC + amplifier | MAX98357A I²S mono amplifier breakout, powered from 5 V, configured for left+right mix if the breakout exposes channel selection. |
| JSD1 | Card | 16 GB microSD, FAT32. For a custom PCB use a 3.3 V socket/interface, not a 5 V level-shifting module. |
| SPK1 | Speaker | Prefer a larger 40–50 mm full-range 4 Ω, 3 W driver if the case allows. The pictured 35 × 25 mm, 4 Ω/3 W driver is electrically suitable but likely to sound thinner. Give it a sealed rear cavity. |
| BT1 | Battery | 1-cell 3.7 V LiPo, 2000 mAh, with protection for over-charge, over-discharge, and over-current. Verify ≥3 A continuous discharge and ≥4 A protection trip/peak capability from the cell/pack data; otherwise choose a better specified pack. |
| U4 | Charger / power path | BQ24074-based 1-cell Li-ion/LiPo charger breakout, USB 5 V input, power-path output. Set charge current to 500 mA or lower if the cell requires it; use a module with the cell-temperature input configured appropriately. |
| U5 | Boost | TPS61088-based boost module adjusted to **5.0 V**, rated for at least **2 A continuous at 5 V**. Use an assembled module for Rev A; the IC's headline switch-current rating is not the same as guaranteed module output current. |
| F1 | Input protection | 2 A resettable fuse on the USB 5 V input to the charger, selected to suit the charger's input-current limit. |
| SW1 | Main power | Slide switch rated ≥3 A DC, in series with the battery/system path before the boost converter. |
| J1/J2/J3 | Connectors | J1 USB-C 5 V charge input; J2 keyed JST-PH 2-pin battery; J3 2-pin speaker connector rated ≥2 A. |
| SW2–SW4 | Controls | Play/pause, next, previous; optional volume buttons. Momentary normally-open switches. |
| D1 | Status | One LED with 1 kΩ series resistor, or a low-current RGB status LED if desired. |
| NFC tags | Tokens | NTAG213, preferably anti-metal/ferrite-backed, or tags mounted on plastic discs with ferrite behind the tag. |

The BQ24074 is a single-cell charger with power-path management; the TPS61088 accepts a single-cell battery range and is intended for boost conversion. The finished boost module still needs its own thermal/current rating check. [TI BQ24074](https://www.ti.com/product/BQ24074) · [TI TPS61088](https://www.ti.com/product/TPS61088)

## 3. Logical power schematic

```text
J1 USB-C VBUS (5 V, 2 A source) ── F1 2 A ──► U4 BQ24074 IN
J1 GND ─────────────────────────────────────► U4 GND
J1 CC1 ── 5.1 kΩ ── GND      J1 CC2 ── 5.1 kΩ ── GND   (USB-C sink Rd resistors)

BT1 protected LiPo + ────────────────────────► U4 BAT
BT1 protected LiPo − ────────────────────────► U4 GND
U4 OUT / SYS (about 3.0–4.4 V) ── SW1 ──────► U5 VIN
U4 GND ──────────────────────────────────────► U5 GND
U5 VOUT set to 5.0 V ────────────────────────► +5V_SYS

+5V_SYS ──┬──► U3 MAX98357A VIN
          └──► U1 DevKitC 5V/VIN pin (not 3V3)
U1 3V3 pin ──► U2 PN532 VCC, microSD VCC, and logic pull-ups (only if the board supports 3.3 V VCC)
All grounds join to the carrier-board ground plane.
```

**Power rules:** never connect the LiPo directly to the DevKitC 3V3 pin. ESP32-WROOM-32E requires a regulated 3.0–3.6 V rail and its supply should be capable of at least 500 mA; the DevKitC's 5V/VIN path uses its onboard regulator. [Espressif WROOM-32E data sheet](https://documentation.espressif.com/esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.html)

Place 470 µF low-ESR + 100 nF ceramic at U3's 5 V input; 10 µF + 100 nF near the DevKitC 5 V feed; 10 µF + 100 nF beside the SD socket; and 100 nF beside the PN532 header. Follow the selected breakout's own capacitors and do not remove them. Keep the boost converter switch node and inductor away from the NFC antenna and I²S lines. Return the amplifier's high-current ground directly to the power-entry/boost ground area; keep its speaker current out of the PN532/SD return path.

At maximum output, a 3.2 W speaker channel plus MCU/card load can create large battery peaks. Size the cell, JST connector, switch, traces, and protection for **at least 3 A continuous from the cell**, with transient margin; the specific 2000 mAh listing is a hold until its discharge and protection ratings are confirmed. A 2 A 5 V boost rating is headroom, not the expected constant load. Do not infer output current from an online module title alone. Do not play at full volume while charging during early testing; monitor charger and boost temperatures.

If using a charger breakout that already has its own USB-C input, do not duplicate J1 or its CC resistors on the carrier. When the DevKitC is connected to a computer by USB for programming, turn SW1 off so the battery boost and computer USB 5 V are not tied together through the DevKitC.

## 4. Data and control connections

Use the DevKitC header pin labels; all listed GPIO signals are 3.3 V logic.

| Signal | ESP32-WROOM-32E GPIO | Connect to | Notes |
|---|---:|---|---|
| I²C SDA | GPIO21 | PN532 SDA | Add 4.7 kΩ pull-up to 3V3 if the PN532 breakout does not already include one. |
| I²C SCL | GPIO22 | PN532 SCL | Same bus voltage/pull-up as SDA. |
| NFC IRQ | GPIO34 | PN532 IRQ | Optional; input-only pin is suitable. If unused, firmware may poll. |
| NFC reset | GPIO16 | PN532 RSTPDN | Optional if breakout exposes reset. |
| SD SCK | GPIO18 | microSD SCK | SPI bus. |
| SD MISO | GPIO19 | microSD MISO | SPI bus. |
| SD MOSI | GPIO23 | microSD MOSI | SPI bus. |
| SD CS | GPIO13 | microSD CS | Add 10 kΩ pull-up to 3V3 so the card stays deselected at boot. |
| I²S BCLK | GPIO26 | MAX98357A BCLK | Digital audio bit clock. |
| I²S LRCLK/WS | GPIO25 | MAX98357A LRC/WS | Sample/channel clock. |
| I²S DATA | GPIO27 | MAX98357A DIN | PCM data from ESP32. |
| Play/pause | GPIO32 | SW2 to GND | Configure INPUT_PULLUP; debounce in firmware. |
| Next | GPIO33 | SW3 to GND | Configure INPUT_PULLUP. |
| Previous | GPIO14 | SW4 to GND | Configure INPUT_PULLUP. |
| Status LED | GPIO4 | 1 kΩ → LED → GND | Optional; active-high. |

Avoid boot-strapping pins GPIO0, 2, 5, 12, and 15 for switches or chip-selects. Do not put 5 V on any ESP32 GPIO. PN532 uses I²C; it supports I²C/SPI/HSU host interfaces. [NXP PN532 data sheet](https://www.nxp.com/docs/en/nxp/data-sheets/PN532_C1.pdf)

## 5. Audio path and speaker wiring

```text
microSD SPI → ESP32 file reader + MP3/WAV decoder → 16-bit PCM I²S
GPIO26 BCLK ─────────────────────────────────────► MAX98357A BCLK
GPIO25 LRCLK ────────────────────────────────────► MAX98357A LRC/WS
GPIO27 DATA ─────────────────────────────────────► MAX98357A DIN
MAX98357A OUTP ──────────────────────────────────► Speaker +
MAX98357A OUTN ──────────────────────────────────► Speaker −
```

The speaker connects **between OUTP and OUTN**. Neither speaker terminal goes to ground: this is a bridge-tied Class-D output. Keep the two speaker traces together, short, and away from the NFC coil and I²S lines. Add no speaker output capacitor. The MAX98357A accepts 2.5–5.5 V, I²S PCM and can deliver up to 3.2 W into 4 Ω at 5 V; this is a peak capability, not a promise that the small pictured driver will sound full-range. [Analog Devices MAX98357A](https://www.analog.com/en/products/max98357a.html)

For the best sound within this size/cost target:

1. Use 44.1 kHz, 16-bit stereo MP3 at 192–320 kb/s or 16-bit WAV files. The amp breakout must mix L/R to mono; verify the channel-selection strap on its documentation.
2. Use a 4 Ω driver rated at least 3 W. Prefer 40–50 mm if the enclosure allows. The screenshot's slim 35 × 25 mm speaker is a compactness choice, not the sound-quality choice.
3. Build a sealed rear chamber around the driver and a front grille with open area. Prevent front and rear sound waves from leaking around the speaker frame.
4. Set the amp gain conservatively and use software volume control. If it sounds harsh, lower the PCM peak level rather than raising the amp gain.
5. Keep Wi-Fi/Bluetooth off during playback unless needed; this reduces current spikes and potential RF coupling, though it does not change the MP3 decode fidelity.

The MAX98357A is an I²S DAC plus Class-D amplifier in one device, so adding a PCM5102A DAC would add cost and analog wiring without helping this single-speaker topology. A separate line DAC becomes useful only if the design changes to an analog line-out, headphone output, or a different amplifier stage.

## 6. NFC/mechanical placement

- Put the PN532 antenna at the front face, immediately behind a thin non-metallic window. Keep the antenna module's ferrite sheet between its coil and the fridge steel.
- Use NTAG213 tags on plastic CD-shaped tokens with ferrite backing; ordinary NFC stickers directly on steel may have poor range.
- Keep the reader coil at least 20–30 mm from the speaker magnet and the boost converter/inductor. Confirm tag reads after the case is mounted to the fridge.
- Keep the ESP32 DevKitC antenna end at a board edge with the antenna clear of copper, battery foil, speaker magnet, and steel. The uploaded board is the PCB-antenna version.

## 7. Firmware behavior

1. Mount the FAT32 SD card and scan `/tracks/001.mp3` … `/tracks/099.mp3` at startup.
2. Read a tag UID through PN532. Use a table `UID → track number`; do not store audio on the NFC tag.
3. Stop the current decoder, open the mapped file, decode to PCM and stream to I²S at 44.1 kHz.
4. Play/pause, next, previous and volume buttons operate independently of NFC. A repeat scan of the active tag can pause/resume or restart the track—choose one behavior in firmware.
5. Save last volume and track preference in ESP32 NVS. Fade volume down briefly before changing tracks to avoid clicks.

## 8. PCB layout and bring-up

- Rev A carrier: 2-layer, 1.6 mm FR-4, 1 oz copper, solid bottom ground plane. Use ≥1.0 mm 5 V/battery traces for short 2 A paths; widen battery/amp returns and use short connections.
- Put U5 boost components together at the power-entry edge. Keep its SW node small and far from the PN532 coil, SD, and ESP32 antenna.
- Put MAX98357A and speaker connector near the speaker; keep OUTP/OUTN short and paired. Put PN532 at the opposite edge, with a keepout under/around its antenna per module instructions.
- Use separate branches from +5V_SYS for the DevKitC and amplifier. Use a continuous ground plane, but place the amplifier/boost return paths so they do not cross the NFC/SD return paths.
- First power-up without the battery: bench supply at 5.0 V, current limit 0.5 A for logic, then raise the limit for audio. Verify 3V3, I²C detection, SD read, and quiet I²S output before connecting the speaker.
- Next test audio at low volume, then increase gradually while checking 5 V sag, boost temperature, battery connector temperature, and ESP32 brownout logs.
- Measure NFC read range with the final enclosure attached to the fridge. Adjust ferrite/antenna spacing before gluing the reader in place.

## 9. Buy/hold from the uploaded listings

- **Buy/keep:** the pictured ESP32-WROOM-32E DevKitC board is suitable for prototyping. Use the Micro-USB variant shown.
- **Buy/keep:** 16 GB microSD option; format FAT32.
- **Choose:** the speaker listing's 4 Ω / 3 W variant only if the thin case is essential; otherwise find a larger 4 Ω / 3 W full-range speaker.
- **Add:** MAX98357A I²S mono amplifier breakout, BQ24074 power-path charger module, 5 V ≥2 A TPS61088-based boost module, protection-rated LiPo, ferrite, buttons, and connectors.
- **Do not populate for this version:** DFPlayer Mini. It is a valid simpler fallback, but is not in this audio chain.

## 10. Design sources

- [ESP32-WROOM-32E data sheet](https://documentation.espressif.com/esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.html)
- [ESP32-DevKitC board guide](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html)
- [PN532 data sheet](https://www.nxp.com/docs/en/nxp/data-sheets/PN532_C1.pdf)
- [MAX98357A product page and data sheet](https://www.analog.com/en/products/max98357a.html)
- [BQ24074 product page](https://www.ti.com/product/BQ24074)
- [TPS61088 product page](https://www.ti.com/product/TPS61088)
