# Fridge NFC Music Player — Ready-Made Module Wiring

This is the no-custom-PCB version. It replaces the earlier PCB-oriented draft. It uses the ESP32-WROOM-32E DevKitC-style board in your screenshot, plus ready-made reader, SD, and audio modules connected with short wires.

## Recommended build

```text
NTAG213 disc → PN532 V3 NFC reader → ESP32-WROOM-32E
microSD SPI module ────────────────→ ESP32 → software audio decode → I²S
MAX98357A I²S DAC/amplifier ←───────────────────────────────┘
4 Ω speaker ←── MAX98357A speaker outputs

5 V USB power bank ──USB 1──► ESP32 DevKitC Micro-USB
                  └─USB 2──► 5 V branch ─┬─► MAX98357A power
                                         └─► 3.3 V regulator ─► PN532 + microSD
All module grounds connected together
```

The ESP32 board itself has **no microSD socket**. Add a **3.3 V SPI microSD reader breakout**; it holds the card and connects to four ESP32 GPIO pins. Use a 16 GB card formatted FAT32. The card's SPI pins are called CLK/SCK, DI/SI, DO/SO, and CS on most breakouts.

## Parts to use

1. **ESP32-WROOM-32E DevKitC-style board** — the board in your AliExpress screenshot. The Micro-USB version is fine for programming. It is a development board with the ESP32 module and USB-to-serial circuit; most GPIO pins are exposed on its two header rows. WROOM-32E suffixes differ in flash/PSRAM; the common N4 has no PSRAM, while some N4R2/N8R2/N16R2 variants include 2 MB. This PCM-WAV design needs no PSRAM, so check the module marking but do not change boards for this project. The low-cost listing does not show enough detail to confirm its exact module suffix. [Espressif DevKitC guide](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html) · [WROOM-32E variants](https://documentation.espressif.com/esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.html)
2. **PN532 V3 NFC module** — the red board from your picture, with the antenna printed on the PCB. Use I²C mode.
3. **3.3 V-only SPI microSD breakout**, such as the Adafruit 3V-only SPI/SDIO breakout or an equivalent explicitly made for 3.3 V logic. Do not buy a module that requires 5 V logic. The SD card itself is a 3.3 V device. [Adafruit SD breakout guide](https://learn.adafruit.com/adafruit-microsd-spi-sdio)
4. **MAX98357A mono I²S amplifier breakout**, preferably a breakout with documented channel selection and screw terminals. It contains the DAC and amplifier; no separate DAC is needed. [Adafruit MAX98357A guide](https://learn.adafruit.com/adafruit-max98357-i2s-class-d-mono-amp)
5. **Speaker:** the 4 Ω / 3 W option in your photo will work electrically. For fuller sound, buy a larger 40–50 mm 4 Ω full-range speaker and give it a sealed rear cavity. The 35 × 25 mm speaker is very slim and will have limited bass.
6. **5 V USB power bank**, 5,000 mAh or larger, with a regulated 5 V output rated at least 2 A. Prefer two simultaneous outputs and an always-on/low-current mode. Runtime and idle auto-off behavior depend on the exact bank.
7. **3.3 V regulator module** fed from 5 V for the PN532 and microSD breakout. Target an AP2112K-3.3-based breakout (600 mA rated) or an equivalent documented 3.3 V regulator module with at least 500 mA output. This avoids relying on the unknown 3.3 V regulator on a low-cost DevKit clone to supply the ESP32 plus both peripherals. Check that the module input accepts 5 V and that its actual board has enough thermal area; keep it away from the NFC antenna. A linear LDO is electrically quiet; a buck module is more efficient but should be kept physically away from the PN532. [AP2112K datasheet](https://www.diodes.com/datasheet/download/AP2112.pdf)
8. **USB-A 5 V-to-terminal adapter/distribution point** for the amp VIN/GND and 3.3 V regulator input; short jumper wires; soldered headers or screw terminals; three to five momentary normally-open buttons; NTAG213 anti-metal tags (or ferrite-backed plastic tokens); and a thin ferrite sheet behind the PN532 antenna.

Use the power bank instead of the loose 3.7 V pouch cell for this first module build. It already contains the cell charger and 5 V regulator, so this setup needs no TP4056 charger, boost converter, or bare-cell wiring.

## Exact signal wiring

Power everything off while wiring. The ESP32 GPIOs are 3.3 V logic; never put 5 V on them.

| From | Pin | To | Pin |
|---|---|---|---|
| ESP32 DevKitC | GPIO 21 | PN532 V3 | SDA |
| ESP32 DevKitC | GPIO 22 | PN532 V3 | SCL |
| ESP32 DevKitC | GPIO 34 | PN532 V3 | IRQ (optional; leave unconnected for polling firmware) |
| 3.3 V regulator | 3V3 OUT | PN532 V3 | VCC |
| 3.3 V regulator | GND | PN532 V3 | GND |
| ESP32 DevKitC | GPIO 18 | microSD module | CLK / SCK |
| ESP32 DevKitC | GPIO 23 | microSD module | DI / SI / MOSI |
| ESP32 DevKitC | GPIO 19 | microSD module | DO / SO / MISO |
| ESP32 DevKitC | GPIO 13 | microSD module | CS / SS |
| 3.3 V regulator | 3V3 OUT | microSD module | 3V / VCC |
| 3.3 V regulator | GND | microSD module | GND |
| ESP32 DevKitC | GPIO 26 | MAX98357A | BCLK |
| ESP32 DevKitC | GPIO 25 | MAX98357A | LRC / LRCLK / WS |
| ESP32 DevKitC | GPIO 27 | MAX98357A | DIN |
| ESP32 DevKitC | GND | MAX98357A | GND (shared signal ground) |
| USB power bank output 2 | +5 V | MAX98357A | VIN |
| USB power bank output 2 | GND | MAX98357A | GND / common ground |
| USB power bank output 2 | +5 V | 3.3 V regulator | VIN |
| USB power bank output 2 | GND | 3.3 V regulator | GND / common ground |
| MAX98357A | OUT+ | Speaker | + |
| MAX98357A | OUT− | Speaker | − |

`DI/SI/MOSI` means data **from the ESP32 to the card**. `DO/SO/MISO` means data **from the card to the ESP32**. Do not swap these. Use short SD and I²S wires (ideally under 10 cm). The 3.3 V-only breakout's `3V` pin gets the dedicated 3.3 V regulator output, never 5 V. The ESP32 GPIO signals remain 3.3 V. SD cards are 3.3 V devices and can have current peaks above 100 mA, so keep the supply and wires short. [SD pinout and power notes](https://learn.adafruit.com/adafruit-micro-sd-breakout-board-card-tutorial/look-out)

### PN532 switch setting

On the V3 board shown, set the interface switches to **I²C: Channel 1 ON, Channel 2 OFF** (the labels may be printed as IIC/SPI/HSU). Use the board's silk-screen labels to identify SDA and SCL. Feed VCC with **3.3 V**. The V3 manual confirms the I²C switch setting and that its VCC accepts 3.3–5 V. [Elechouse PN532 V3 manual](https://www.elechouse.com/elechouse/images/product/PN532_module_V3/PN532_%20Manual_V3.pdf)

### Audio board settings and speaker

- Power the MAX98357A from the power bank's **separate 5 V output**. Its supply should be able to provide at least 800 mA; the guide recommends 5 V for the full output. [Adafruit MAX98357A power/output limits](https://learn.adafruit.com/adafruit-max98357-i2s-class-d-mono-amp/pinouts)
- Add 470 µF / 10 V electrolytic in parallel with 100 nF ceramic directly across MAX98357A VIN/GND if the breakout does not already provide equivalent local bulk decoupling. Keep leads short and observe electrolytic polarity.
- Pick a **mono** MAX98357A breakout. At 5 V, the Adafruit breakout's default SD setting mixes left and right into mono; leave its GAIN pin open for its default 9 dB gain. If using a different breakout, follow that board's own GAIN/SD instructions.
- Connect the speaker only between `OUT+` and `OUT−`. This is a bridge-tied output: **neither speaker wire goes to GND**. Use a moving-coil speaker of **4 Ω or higher**. [Adafruit speaker-output notes](https://learn.adafruit.com/adafruit-max98357-i2s-class-d-mono-amp/pinouts)
- Keep amp power/speaker wires away from the PN532 antenna. If the sound distorts, lower the software volume; do not increase gain.

## Power wiring

Use one power bank with two simultaneous 5 V outputs (or one 5 V output and a rated splitter/distribution point):

```text
Power bank USB output 1 ──Micro-USB cable──► ESP32 DevKitC USB port
Power bank USB output 2 ──5 V distribution──┬─► MAX98357A VIN
                                            └─► 3.3 V regulator IN
3.3 V regulator OUT ──────────────────────────► PN532 VCC and microSD 3V
All GNDs ─────────────────────────────────────► common ground
```

This keeps amplifier and peripheral supply current off the DevKit clone's 3.3 V regulator. The shared ground wire is required for I²S. Power the DevKitC only through Micro-USB and leave its 5V/3V3 power pins unused. The official DevKitC guide says the USB, 5V-header, and 3V3-header supplies are mutually exclusive. Disconnect the power bank before plugging the DevKit into a computer for programming. [DevKitC power rules](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html)

Place a 100 µF / 6.3 V electrolytic and 100 nF ceramic across the external 3.3 V peripheral rail near the SD breakout if equivalent local decoupling is not already fitted. Retain the regulator module's specified input/output capacitors.

If the bank has one output, use a **rated 5 V / 2 A splitter/distribution point**: one branch to the DevKit Micro-USB, the other to MAX98357A VIN and the 3.3 V regulator input. Do not feed 5 V into the DevKit's 5V pin while its USB port is connected. Do not connect two different power banks; use one shared source.

## Buttons and NFC tokens

Connect each momentary normally-open button between a GPIO and GND:

| Button | ESP32 pin | Other side |
|---|---:|---|
| Play/stop | GPIO 32 | GND |
| Next | GPIO 33 | GND |
| Previous | GPIO 14 | GND |

Configure the pins as `INPUT_PULLUP` in firmware and debounce them in code. GPIO 14, 32, and 33 are usable on the pictured WROOM DevKitC. Avoid GPIO16/17 because the unreadable WROOM suffix could indicate a PSRAM variant that reserves those pins. The firmware maps each PN532 tag UID to a track filename; the tag stores only an ID, not the music.

The fridge steel affects NFC. The uploaded sticker listing does not establish that the tags are anti-metal. Use on-metal NTAG213 tags, or make removable plastic tokens with ferrite between the tag and steel. Put a ferrite sheet behind the PN532 antenna, then verify reads on the actual fridge before closing the case. Expect shorter range than the free-air module claim. Keep the reader near the front cover and the speaker magnet/amp power parts at the opposite side of the case. For the first firmware test, poll over I²C and leave IRQ unconnected; GPIO34 is input-only and has no internal pull-up, so use IRQ only after checking the module's IRQ output/pull-up arrangement.

## Card contents and audio firmware

Format the card FAT32 and start with 16-bit stereo WAV files at 44.1 kHz in `/music/`, for example `/music/001.wav`. This avoids MP3 decoding overhead while keeping full CD-rate PCM audio. A 16 GB card holds roughly 25 hours of that format.

Use the Arduino **ESP8266Audio 2.4.2** library (`AudioFileSourceSD` + `AudioGeneratorWAV` + `AudioOutputI2S`). Despite its name, the library supports ESP32; the current release notes include ESP32 Arduino core 3.x support and recent I²S/WAV fixes. For this WROOM board, start with **PCM WAV, 16-bit, 44.1 kHz**; PSRAM is not needed for this simple local-file path. Configure the MAX98357A breakout's SD/GAIN mode from its own documentation. The Adafruit breakout mixes L+R at 5 V by default, but a generic clone may not; safest is dual-mono content (same mono track in both WAV channels) and a documented left-channel or average strap. Do not assume an unmarked SD pin will play. A 16 GB card stores about 23–25 hours of 16-bit 44.1 kHz stereo PCM. [ESP8266Audio releases and support](https://github.com/earlephilhower/ESP8266Audio/releases) · [MAX98357A channel-mode guide](https://learn.adafruit.com/adafruit-max98357-i2s-class-d-mono-amp/pinouts)

The audio library's loop must run frequently; avoid long `delay()` calls while music plays. The compiled starter sketch is in [fridge_nfc_player_build/firmware](fridge_nfc_player_build/firmware/README.md); confirm received module labels and finish the bring-up checks before connecting the amp or closing the case.

## Assemble and check in this order

1. Solder header pins onto the DevKit, PN532, SD breakout, regulator, and amp breakout. Do not use a loose breadboard for speaker power; solder or screw-terminal connections are more reliable in the mounted case.
2. With power disconnected, check all pin labels. Confirm the SD breakout is 3.3 V logic and confirm the selected 3.3 V regulator input/output pins before connecting the peripherals.
3. Power the ESP32 through Micro-USB from the bank, power the 3.3 V regulator from the 5 V branch, and connect the SD breakout to that regulator. Run an SD test and confirm it lists `/music/001.wav`.
4. Add the PN532 to the same regulated 3.3 V peripheral rail. Confirm I²C mode, 3.3 V pull-ups, and tag UID reads away from the fridge.
5. Add the MAX98357A to the 5 V distribution branch and join its GND to ESP32 GND. Test audio at low volume, then raise volume gradually.
6. Run the combined firmware: tag → filename → sound. Finally attach ferrite and test reads with the enclosure mounted on the fridge.

## Important sourcing checks

- The exact cheap module clone can differ from the photo. Before connecting power, check its labels and pinout against its own listing/manual. The photo alone does not verify regulator, SD-mode resistors, speaker rating, or battery protection.
- For highest sound quality, the pictured 35 × 25 mm driver is the limiting part. Prefer a larger 4 Ω driver and a sealed speaker cavity if the enclosure can fit it.
- The pictured loose 3.7 V battery is not used in this no-PCB build. Its listing only advertises over-charge protection, so do not use it unless the exact cell/pack documentation confirms over-discharge, over-current/short-circuit protection, and a safe charge method. Do not connect it directly to the ESP32 or amplifier.
