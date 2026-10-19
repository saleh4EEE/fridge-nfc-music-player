# Fridge NFC Music Player — Project Audit

**Review date:** 18 October 2026  
**Status:** Technically feasible as a module prototype, but not physically verified. Use the corrected wiring in [fridge_nfc_player_module_wiring.md](fridge_nfc_player_module_wiring.md). The earlier PCB draft is superseded.

## Executive verdict

The core idea is sound: a passive NFC disc selects a locally stored track, an ESP32 reads the track from microSD, decodes it, and sends digital audio over I²S to a speaker amplifier. The pictured ESP32-WROOM-32E board is capable of this. No custom PCB and no second ESP32 board are needed.

The current design is **not yet “no-mistakes guaranteed.”** The exact shopping listings are low-cost clones, and photos do not verify their electrical details. Three items need special care:

1. **Power:** the photographed pouch cell only claims over-charge protection. Do not use it for this build. Use a protected 5 V USB power bank for the first version. Add a separate 3.3 V regulator for the PN532 and microSD so their combined load does not depend on an unknown DevKit clone regulator. An AP2112K-3.3-based 600 mA LDO breakout is an appropriate module-level target if its input range and thermal area are documented.
2. **Sound:** the pictured 35 × 25 mm driver is the main quality limit. It will make sound, but it cannot produce convincing bass. The enclosure and speaker selection matter more than changing WROOM to an S3.
3. **NFC on a refrigerator:** the uploaded tag listing does not show an anti-metal/ferrite backing. Bare NFC stickers directly on steel may read poorly or not at all. The reader must also be tested with ferrite behind it on the actual fridge.

The recommendation is to keep the WROOM-32E, keep the I²S audio chain for the sound-focused version, and build it in stages. Do not buy the loose battery, BQ24074, TPS61088, or a separate DAC for this module prototype.

**Correction to the earlier shorthand about PSRAM:** not every WROOM-32E variant is PSRAM-free. Espressif lists common N4/N8/N16 versions with no PSRAM and N4R2/N8R2/N16R2 variants with 2 MB. The AliExpress screenshot does not identify the exact suffix. This WAV design does not need PSRAM either way, so the board choice remains valid.

## Define the product clearly

This plan is an **offline, mono, local-file music player**:

```text
NFC disc UID → PN532 → ESP32 track lookup → microSD file → PCM decode
            → I²S → MAX98357A DAC + amplifier → one moving-coil speaker
```

The NFC disc stores an ID, not a song. Songs live on the SD card. It is not Spotify, phone streaming, or audio stored in the NFC tag. The MAX98357A contains the DAC, so a PCM5102A or other separate DAC is unnecessary. The DFPlayer Mini in the uploaded images is an alternative, simpler architecture, not an extra module in this one.

## Parts audit

| Part from the project | Decision | Sharp assessment |
|---|---|---|
| ESP32-WROOM-32E DevKitC-style board, Micro-USB | **Keep** | Sufficient for NFC, SPI SD, WAV decode, buttons, and I²S. The WROOM board has no microSD socket; the socket is on the separate SD breakout. No PSRAM is required for the chosen PCM-WAV path. WROOM-32E suffixes vary: N4/N8/N16 have no PSRAM; N4R2/N8R2/N16R2 include 2 MB. The screenshot does not show the suffix. The exact low-cost board is a clone, not an authenticated Espressif DevKitC; check its pin labels and regulator before relying on its electrical ratings. |
| PN532 V3 red reader | **Keep, verify switch and pinout** | The uploaded image matches the V3 switch layout. I²C is Channel 1 ON / Channel 2 OFF in the Elechouse V3 manual. Use 3.3 V for this build so all exposed logic is 3.3 V-safe; do not assume every clone has the same level shifting. |
| NTAG213/215/216 stickers | **Keep only if NFC test passes** | These are suitable NFC tag families for track IDs. The uploaded listing does not prove that they are on-metal tags. Prefer ferrite-backed/on-metal tags, or put the tag on a removable plastic token with ferrite between tag and fridge. |
| microSD card | **Keep; choose a genuine card** | 16 GB is enough for about 23–25 hours of 16-bit/44.1 kHz stereo PCM WAV. A fast card does not improve sound quality; reliability matters. Format FAT32. A no-brand card with an inflated capacity is a risk. |
| microSD interface | **Add** | Buy a real 3.3 V SPI breakout with a socket and 3.3 V logic. The ESP32 board has no socket. Avoid generic “5 V SD modules” unless their level shifting and regulator are documented; some cheap designs are poor at ESP32 logic levels. |
| MAX98357A I²S mono amp | **Keep; verify exact breakout** | It combines DAC and speaker amp. It accepts 3.3 V I²S logic and runs from 5 V. Its SD/MODE pin controls shutdown and left/right/average selection; a generic breakout may not have the same resistor straps as Adafruit’s. Buy a breakout with documented SD/GAIN mode or test its mode before enclosing it. |
| 35 × 25 mm speaker listing | **Electrically usable; upgrade for sound** | Select the 4 Ω / 3 W option, not the 8 Ω option if maximum loudness is needed. This small oval driver is the weakest part acoustically. Prefer a 40–50 mm full-range 4 Ω driver with a sealed rear chamber if it fits. |
| DFPlayer Mini | **Do not add to the I²S build** | It is a valid lower-cost fallback and already has a microSD socket, decoder, and small amp. If chosen, it replaces the microSD breakout, WAV decoding, and MAX98357A path; do not wire it into the MAX98357A’s I²S input. Its cheap clones have more variable audio/noise behavior. |
| ESP32-S3-N16R8 | **Do not switch just for sound** | Extra RAM/features do not improve the DAC, amplifier, or speaker sound. The WROOM is enough. Consider S3 only if later firmware specifically needs PSRAM or more memory. |
| 3.7 V 2000 mAh 504070 pouch cell from photo | **Reject for now** | The listing text shown advertises over-charge protection only. That does not establish over-discharge, over-current, or short-circuit protection. A LiPo also needs a proper charger and 5 V boost rail. Do not connect it directly to 3V3, 5V, or the speaker amp. |
| BQ24074 + TPS61088 PCB power scheme in old draft | **Do not build now** | It was a PCB-level concept and is unnecessarily complex for this module build. A charger breakout and boost board also need verified current, thermal, and power-path behavior. |
| Peripheral 3.3 V rail | **Add AP2112K-3.3 breakout or equivalent** | Feed it from the 5 V bank output and use it for PN532 + 3.3 V SD. AP2112K is a 600 mA LDO with 2.5–6 V input; check the actual module layout/thermal rating rather than assuming the chip's headline current is available on every tiny breakout. |
| Supply decoupling | **Add if not already fitted on the module** | Put 470 µF / 10 V plus 100 nF at MAX98357A VIN/GND, and 100 µF / 6.3 V plus 100 nF near the 3.3 V SD/peripheral rail. Keep leads short and retain regulator-required capacitors. |

## Correct module wiring

Use these GPIO numbers, but follow the **silkscreen labels on the received board**, not a pin’s physical position in a product photo.

Do not use GPIO16/17 in this build. The exact WROOM-32E suffix in the photo is unreadable; PSRAM variants reserve those lines. The firmware therefore leaves PN532 reset unconnected and has no buttons on GPIO16/17.

| Function | ESP32-WROOM GPIO | Peripheral pin |
|---|---:|---|
| PN532 I²C SDA | 21 | SDA |
| PN532 I²C SCL | 22 | SCL |
| PN532 IRQ (optional) | 34 | IRQ; input only, add pull-up only if the module does not provide one |
| microSD SPI clock | 18 | CLK / SCK |
| microSD SPI controller-to-card data | 23 | DI / SI / MOSI |
| microSD SPI card-to-controller data | 19 | DO / SO / MISO |
| microSD select | 13 | CS / SS; add 10 kΩ pull-up to 3.3 V if the breakout has none |
| MAX98357A bit clock | 26 | BCLK |
| MAX98357A word select | 25 | LRC / LRCLK / WS |
| MAX98357A audio data | 27 | DIN |
| Play/stop, next, previous | 32, 33, 14 | Each button between GPIO and GND; configure `INPUT_PULLUP` |

Power tree for the no-PCB prototype:

```text
One 5 V USB power bank, rated ≥2 A total
  ├─ USB output 1 → DevKit Micro-USB (board power/programming path)
  └─ USB output 2 → 5 V-to-terminal adapter/distribution
       ├─ +5 V/GND → MAX98357A VIN/GND
       └─ +5 V/GND → documented 3.3 V regulator, ≥500 mA output
                         ├─ 3.3 V/GND → PN532 VCC/GND
                         └─ 3.3 V/GND → 3.3 V SPI microSD breakout

ESP32 GND, amp GND, regulator GND, PN532 GND, SD GND = common ground
```

Use one power bank; do not use two independent supplies. Connect the amplifier speaker **only between OUT+ and OUT−**. Neither speaker wire goes to ground. Do not put 5 V on an ESP32 GPIO or 3V3 pin. Power the DevKit through Micro-USB only in this setup, and disconnect the power bank before connecting its Micro-USB to a computer. Espressif says the DevKitC’s Micro-USB, 5 V-header, and 3V3-header power methods are mutually exclusive.

### microSD connection explained

The WROOM module itself has no card socket. The **SPI microSD breakout** supplies the physical socket and routes the card’s SPI signals to GPIO18/19/23/13. `MOSI/DI/SI` carries bytes from ESP32 to card; `MISO/DO/SO` carries bytes from card to ESP32. Power the breakout at 3.3 V. Keep these wires short (under about 10 cm for a hand-wired prototype) and use a known-good card.

## Audio processing and realistic quality

- Track selection is not audio processing: firmware reads the tag UID, looks up a filename, and opens that file on the SD card.
- For the simplest high-quality path, use **16-bit integer PCM WAV at 44.1 kHz**, converted from a good source. A WAV made by re-encoding a low-quality MP3 cannot restore lost quality.
- The ESP32 software reads the WAV from SD and streams PCM on I²S. The MAX98357A performs digital-to-analog conversion and Class-D amplification. Start with ESP8266Audio **version 2.4.2**, whose upstream release line includes ESP32 Arduino core 3.x support and recent I²S/WAV fixes. Pin the library version and compile a simple SD-to-audio sketch before combining NFC/buttons; if you later upgrade the library or board core, compile and retest.
- The MAX98357A breakout must be configured deliberately. For stereo files it must average L+R, or the firmware should provide the same mono content in both left and right slots and the amp should be strapped to select one channel. Do not assume an unmarked SD/MODE pin is safe: ground can shut the amp down. The documented Adafruit breakout mixes channels at 5 V by default; no such assumption is safe for an undocumented clone.
- MAX98357A at 5 V can reach around 2.5 W into 4 Ω at 1% THD; the often-quoted 3 W figure is around 10% THD. Keep digital volume below clipping. The photographed 4 Ω / 3 W driver is compatible, but its small diaphragm and a shallow/leaky case will limit bass and clarity.
- Best value for audible improvement: larger quality driver + sealed chamber + vibration-resistant mounting, before paying more for a different ESP32 or a separate DAC.

### Firmware flow

1. Mount the FAT32 card and confirm `/music/` is readable at startup.
2. Poll the PN532 over I²C. Read the complete UID length and bytes; NTAG213/215/216 commonly report a 7-byte UID, so do not hard-code the lookup to four bytes.
3. Match UID → filename (for example `/music/001.wav`). The UID is the selection key; the tag does not need to contain track data.
4. Stop and close the current decoder/file cleanly, then open the new file and restart I²S. Debounce repeated tag reads so one held disc does not restart the same song continuously.
5. Poll buttons while servicing the audio decoder continuously. The supplied starter runs NFC polling on a separate low-priority core task, uses a conservative fixed software gain, and implements play/stop plus next/previous (stop restarts from the beginning; it does not resume). Avoid long `delay()` calls in the audio loop.

## Power and battery assessment

**Prototype choice:** a commercial 5 V, 2 A power bank, 5,000 mAh or larger, with two simultaneous outputs or a proper 5 V splitter/distribution adapter. This avoids bare-cell charging and protection design. Pick one with low-current/always-on mode if available and verify it does not shut off while paused. Do not assume that “5,000 mAh” means 5,000 mAh at 5 V; capacity is usually quoted at the internal cell voltage and converted output energy is lower.

A 5,000 mAh bank is likely adequate for a portable prototype. A rough expectation is **about 7–15 hours** at ordinary listening, but volume, amp efficiency, NFC polling, bank conversion losses, and auto-off behavior dominate. Measure the real current with the actual speaker and bank before treating that runtime as a requirement.

The 2000 mAh pouch battery photo is not an acceptable shortcut. To make a later slimmer integrated battery, select a documented protected 1S pack with over-charge, over-discharge, and over-current/short protection, and use a charger/power-path plus a 5 V boost supply sized for the measured peak current. That is a separate design revision; do not mix it into this first assembly.

## Enclosure and interface

Use a shallow two-piece case with a replaceable printed album-art insert behind a clear front. Put the NFC target mark at the PN532 coil center, a real open grille over the speaker, and the speaker in its own sealed rear pocket. The small oval speaker may fit a thin case, but a larger round driver and a deeper sealed chamber will sound better. Hide the three main buttons along the lower edge; add volume buttons only if the face still looks clean. Leave the SD card, USB charging lead, and reset/programming access reachable.

Keep the PN532 at the front behind plastic with ferrite behind it. Put the ESP32 antenna end near an outer edge and away from the fridge steel as much as the case allows. Put the speaker magnet, MAX98357A, and any buck converter away from the NFC coil. A decorative LED ring can be added later, after NFC/audio tests; it adds power and wiring without improving sound.

The commercial power bank is safer and simpler than the pouch cell but adds bulk and weight. Give it a real compartment/retainer and use mechanically retained fridge-rated magnets or a mounting method rated for the full assembly weight. Keep the magnets away from the PN532 antenna. Do not rely on a thin adhesive sticker alone to support the device on a vertical fridge door.

## Cost review (Malaysia, approximate)

The screenshot prices are promotional storefront snapshots, not a confirmed delivered bill of materials. The ESP32 screenshot shows RM4.60 plus RM7.19 shipping; the PN532 shows about RM2.60; the tag listing shows about RM0.95 each; and the 35 × 25 mm speaker shows RM2.16. The separate MAX98357A module is currently listed around RM7.76–RM10.90 by local online sellers; a branded Adafruit part is more. A 5,000 mAh bank is roughly RM35–RM59 in the listings reviewed. Prices, shipping, and the exact variant change.

| Build portion | Planning allowance |
|---|---:|
| Controller, PN532, 9 basic tags, small speaker (visible screenshot options) | RM20–35 before combined shipping |
| 3.3 V SPI microSD breakout + reliable 16 GB card | RM16–40 |
| MAX98357A documented breakout | RM8–25 |
| 3.3 V peripheral regulator, wires/headers/buttons/USB adapter | RM15–35 |
| 5,000 mAh 5 V power bank | RM35–59 |
| Ferrite and simple acrylic/plastic housing, artwork/grille/mounting | RM20–55 |
| **Expected complete prototype** | **about RM115–250**, excluding tools and variable shipping/tax |

You can reduce the amount paid now by using components already purchased and starting with one tag. Do not save RM10 by buying an unverified SD card or unsafe battery; those are high-impact failure points. A better speaker/enclosure could add RM10–30 and is the upgrade most likely to improve perceived sound.

## Pros and cons

**Pros**

- Offline playback: no phone, Wi-Fi, account, or streaming dependency.
- WROOM has enough compute and exposed GPIO; using an SPI card module is standard and avoids changing boards.
- I²S keeps the audio digital up to the DAC/amp, and one MAX98357A module avoids extra analog wiring and a separate DAC.
- NFC mapping is simple and tags are passive; adding/remapping a track is a firmware/data-table task.
- Modular wiring lets you replace the SD, NFC, amp, or speaker without redesigning a PCB.

**Cons / tradeoffs**

- This is mono, not stereo. One small speaker cannot reproduce deep bass; the thin enclosure will worsen it.
- More modules/wires mean more assembly and more points for loose connections than a single integrated audio board.
- WROOM software playback needs a carefully serviced audio loop; blocking delays or SD errors can cause gaps.
- NFC range on fridge steel is an enclosure/antenna problem, not guaranteed by the nominal free-air range.
- The power bank is safe and easy but bulkier than a bare cell and may be unattractive unless the case includes a battery compartment.
- Extremely cheap clone listings may mislabel the board, omit the MAX98357A mode resistors, include level shifters unsuitable for 3.3 V, or ship a fake-capacity SD card.

## Bring-up gates — do these in order

1. Inspect each received board and photograph the pin labels. Confirm the SD breakout logic voltage, the PN532 I²C switch setting, and the amp’s SD/GAIN resistor configuration before powering it.
2. With only the ESP32 powered by USB, upload a blink/serial sketch and identify the actual board/USB-UART chip. Confirm it can be programmed repeatedly.
3. Add the SD breakout and card. Run a read-only file-list test and play one known-good WAV through I²S into the amp at low volume.
4. Add PN532. Read each tag UID away from and then on the fridge; test the actual ferrite/tag construction before gluing the enclosure.
5. Add buttons, track mapping, volume limiting, and resume/next behavior. Keep the audio loop running frequently; no long blocking delays while playing.
6. Run at expected maximum volume from the power bank. Check for brownouts, SD dropouts, distortion, hot wiring/regulator, and power-bank auto-off.
7. Only after those checks, make the attractive case and mount it. Leave the SD card, USB charge port, and reset/programming access reachable.

No physical compatibility test has been performed because the actual delivered modules are not present for measurement. The plan is electrically compatible on paper, but final confirmation depends on the exact breakout revisions and this bring-up sequence.

## References

- [Espressif ESP32-DevKitC guide and pin/power rules](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html)
- [Espressif ESP32-WROOM-32E datasheet](https://documentation.espressif.com/esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.html)
- [Diodes Inc. AP2112 LDO datasheet](https://www.diodes.com/datasheet/download/AP2112.pdf)
- [Elechouse PN532 V3 manual](https://www.elechouse.com/elechouse/images/product/PN532_module_V3/PN532_%20Manual_V3.pdf)
- [Adafruit microSD electrical and SPI notes](https://learn.adafruit.com/adafruit-micro-sd-breakout-board-card-tutorial/look-out)
- [Adafruit MAX98357A output, power, gain, and channel mode](https://learn.adafruit.com/adafruit-max98357-i2s-class-d-mono-amp/pinouts)
- [ESP8266Audio current releases](https://github.com/earlephilhower/ESP8266Audio/releases)
- [Local MAX98357A listing, Synacorp](https://synacorp.my/v3/en/sounds/3932-max98357a-i2s-audio-amplifier-module-3w-class-d-power-amplifier-board-soldered.html)
- [5,000 mAh power-bank listing, Harvey Norman Malaysia](https://www.harveynorman.com.my/smart-tech-and-phones/phone-accessories/mobile-power-banks/yoobao-5000mah-5v-2.1a-mini-size-power-bank-blue-yb-pb-q11-bl.html)
