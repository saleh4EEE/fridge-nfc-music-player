# Buy list — fridge NFC music player

Prices and stock checked on 19 October 2026. Links open the product page; shipping, tax, and final stock are set at checkout. The pictured ESP32 and PN532 are reused if you already bought them. This is a list only; no vendor carts were changed.

## Keep from your photos

| Part | Action | Notes |
|---|---|---|
| ESP32-WROOM-32 DevKit board | Reuse; do not upgrade | The pictured classic ESP32 board is sufficient for this offline WAV player. The firmware uses ESP8266Audio and 16-bit, 44.1 kHz PCM WAV; PSRAM is not needed. |
| PN532 V3 NFC reader | Reuse if it is the pictured red V3 board | Set it to I²C. Power it from 3.3 V and keep its antenna at the front of the case. |
| 16 GB microSD card | Reuse if it passes a capacity/read check | Format FAT32. A faster card will not improve sound. A replacement link is below if the pictured card is fake or faulty. |
| Small 4 Ω / 3 W speaker | Keep as a bench-test spare | It should work electrically, but its small cone will limit music quality and bass. The recommended 40 mm speaker below is the sound upgrade. |
| Plain NFC stickers / loose Li-ion pouch cell | Do not rely on these for the final build | Plain stickers may fail on the steel fridge. Use anti-metal tags. Use the power bank below instead of the unverified loose cell. |

## Buy for the working electronics

| Qty | Exact item | Direct link / current listing | Why this one |
|---:|---|---|---|
| 1 | SPI microSD socket breakout, Adafruit #4682 | [Adafruit product 4682](https://www.adafruit.com/product/4682) — US$3.50 | This is the missing card socket for the ESP32. It is 3.3 V power and logic only; do not connect 5 V to it. The board does not include a card. |
| 1 | Fixed 3.3 V buck converter, Adafruit TPS62827 #4920 | [Adafruit product 4920](https://www.adafruit.com/product/4920) — US$6.95 | Takes the bank’s 5 V and supplies 3.3 V to the PN532 and SD breakout. Use this instead of trusting the unknown regulator on a clone DevKit. |
| 1 | I²S mono DAC + class-D amp, Adafruit MAX98357A breakout #3006 | [Mouser Malaysia product 3006](https://my.mouser.com/en/ProductDetail/Adafruit/3006?qs=SV%252B0z0o3NTtodS8r%2Ftqo5A%3D%3D) — RM24.57 listed | One board converts ESP32 I²S audio to speaker power; no separate PCM5102 DAC or DFPlayer is required. Buy the MAX98357A breakout, not an analog-input PAM8403 board. |
| 1 | Full-range speaker, PUI Audio AS04004PR-R, 4 Ω / 3 W / 40 mm | [RS Malaysia speaker 276-821](https://my.rs-online.com/web/p/miniature-speakers/0276821) — RM33.97 listed | A known 40 mm driver that matches the amp. Put it in a sealed rear chamber. It will sound clearer and fuller than the pictured 35 × 25 mm driver, though a small enclosure still cannot produce deep bass. |
| 1 | 5 V USB power bank, Xiaomi 10000 mAh 22.5 W Lite | [DirectD product page](https://www.directd.com.my/mi-powerbank-10000mah-22w-lite) — RM59 listed | It has two USB-A outputs plus USB-C and supports simultaneous outputs. Use USB-A port 1 for the ESP32 and USB-A port 2 for the 5 V amp/regulator branch. The rated capacity at 5 V is lower than the headline 10,000 mAh. |
| 1 | Short USB-A male to 2-wire bare-end 5 V power lead, 2 A minimum | [Shopee Malaysia listing](https://shopee.com.my/0.3-1-2M-DC-5V-USB-2.0-Type-A-Male-2-Pin-Cable-Power-Supply-Adapter-Charge-for-Smart-Devices-DIY-Connector-Wire-i.471745680.8870560506) — select 0.3 m | This feeds the MAX98357A and 3.3 V buck in parallel from USB-A port 2. Use a short lead with copper conductors and a seller-confirmed ≥2 A rating; the listing does not publish its wire gauge. Red is +5 V and black is GND. |
| 1 | USB-A to Micro-USB **data** cable, if you do not already have one | [HOCO X26 cable](https://www.hocomalaysia.com/estore/index.php?product_id=2107&route=product%2Fproduct) — RM15 listed | For programming the pictured Micro-USB DevKit and for its bank power lead. Charge-only cables will not upload firmware. |
| 1 | 40-piece 10 cm female-to-female Dupont jumper set | [Synacorp jumper set](https://synacorp.my/v3/ms/jumper-wires/1700-10cm-female-to-female-breadboard-jumper-wire-40pcs.html) — RM3.50 listed | Short signal connections for I²C, SPI, and I²S. Solder the included headers to the breakout boards first; replace loose jumpers with soldered/secured wiring in the finished case. |
| 3 | Momentary push buttons, 12 × 12 mm with cap | [Synacorp 12×12 button](https://synacorp.my/v3/en/push-buttons/1636-365-12x12x73-push-button-with-cap.html) — RM0.50 each listed | Choose black/white caps for play-stop, next, and previous. These are board-mount buttons, so support them behind the front-panel holes. |

### If a pictured part is missing or defective

| Qty | Part | Direct link / current listing |
|---:|---|---|
| 1 | Classic ESP32 38-pin CP2102 Micro-USB DevKit, ESP-WROOM-32 | [GI Electronics board](https://www.gie.com.my/shop.php?action=wireless%2Fwifi%2Fesp32_38pin_micro-cp2102) — RM28 listed. This is a replacement for the pictured board, not an upgrade. |
| 1 | PN532 V3 NFC reader with I²C/SPI/HSU selection | [Synacorp PN532 V3](https://synacorp.my/v3/en/modules/3187-pn532-nfc-rfid-v3-card-tag-reader-module-i2c-spi-hsu.html) — RM16.90 listed. It includes a 4-pin cable and sample cards/tags. |
| 1 | Genuine 16 GB microSDHC card | [DirectD SanDisk card](https://directd.com.my/sandisk) — RM39 listed; choose the 16 GB microSD option. |

## NFC tokens and the fridge mount

| Qty | Exact item | Direct link / current listing | Notes |
|---:|---|---|---|
| 9 | 25 mm NTAG213 anti-metal NFC stickers | [Synacorp NTAG213 options](https://synacorp.my/v3/en/nfc-1356mhz/2476-1721-ntag213-nxp-nfc-mifare-1356mhz-ntag-213-rfid-universal-label-tag-sticker.html) — RM1 each listed | Select the exact option **“NTAG213 (ANTI-METAL) OEM”**. Stick one behind the center label of each removable disc; the tag stores the track ID, while audio stays on the SD card. |
| 1 sheet/roll | Thin ferrite sheet intended for NFC/RFID at 13.56 MHz | [Shopee ferrite-sheet listing](https://shopee.com.my/CHIHIRO1HXY3-1Roll-Antimagnetic-Sticker-3-Sizes-RFID-NFC-Microwave-Shielding-Absorbing-Material-Single-sided-Adhesive-Low-High-Frequency-Anti-Met-Ferrite-Sheet-i.1708400077.54614868517?extraParams=%7B%22display_model_id%22%3A311278929563%2C%22model_selection_logic%22%3A3%7D) | Cut a piece slightly larger than the PN532 antenna and place it behind the reader coil, between the coil and fridge steel. Confirm the selected listing variation is ferrite, not ordinary metal foil. Test tag reads before closing the case. |
| 4 | N35 neodymium magnets, 20 × 10 × 3 mm | [Synacorp N35 magnet](https://synacorp.my/v3/en/rectangular-magnets/3137-2281-n35-rectangular-solid-disc-neodymium-magnet.html) — RM2.50 each listed; select 20×10×3 mm | Mechanically capture them at the case corners, well away from the PN532 antenna. The seller does not publish pull-force data; test the finished case on the fridge and check it stays put when the door closes. |
| 9 optional visual tokens | 60 mm round, 3 mm acrylic discs, cut to size | [UMAKE cut-to-size acrylic](https://umake.my/materials) — clear acrylic starts at RM3.90 | This makes the “mini CD” look. Choose circles, 60 mm diameter, 3 mm thick; add your printed record artwork on the front and the anti-metal tag on the back. The tag can also go behind a paper/card disc if you want to save money. |

## Order after the electronics arrive

Do not order the main enclosure panels until the actual speaker, DevKit, and reader have been laid out on paper. Then order a 3 mm clear front panel and a black/white rear panel from [UMAKE cut-to-size acrylic](https://umake.my/materials). Leave room for the speaker’s 18.3 mm depth, SD-card access, USB access, and a sealed rear speaker chamber. Keep all metal/magnets away from the PN532 coil. The maker’s cut-to-size page accepts custom dimensions and circle/rectangle shapes.

## Planned power and signal split

```text
Power bank USB-A 1 ── Micro-USB data/power cable ──► ESP32 DevKit
Power bank USB-A 2 ── short 5 V pigtail ──┬────────► MAX98357A VIN
                                         └────────► TPS62827 IN (5 V)
TPS62827 OUT (3.3 V) ─────────────────────┬────────► PN532 VCC
                                         └────────► microSD 3V
ESP32 GPIO 21/22 ──────────────────────────────────► PN532 SDA/SCL (I²C)
ESP32 GPIO 18/23/19/13 ────────────────────────────► SD SCK/MOSI/MISO/CS (SPI)
ESP32 GPIO 26/25/27 ───────────────────────────────► amp BCLK/LRC/DIN (I²S)
MAX98357A OUT+/OUT− ───────────────────────────────► 4 Ω speaker only
All module grounds ─────────────────────────────────► common GND
```

Use the project’s [wiring.md](wiring.md) for every pin and control connection. The speaker connects only between `OUT+` and `OUT−`; neither speaker wire goes to GND. Disconnect the power bank before connecting the DevKit to a computer. Never power its USB connector and 5 V header at the same time.

## Cost picture

With the pictured ESP32, PN532, and microSD card reused, the locally priced electronics above are about RM151 before ferrite, magnets, tokens/case, shipping, and the two Adafruit imports (US$10.45 total before shipping). Add about RM10 for the four mounting magnets. The case and tokens add cost based on dimensions. If the pictured speaker is acceptable for a first test, skip the PUI speaker now and upgrade after listening.

The power bank replaces the loose pouch cell and avoids a separate charger, boost board, and battery-protection design. The selected WROOM board remains adequate for the chosen WAV/I²S playback path; buying an ESP32-S3 will not improve speaker sound by itself.

