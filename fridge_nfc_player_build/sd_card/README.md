# microSD card setup

Format a genuine 16 GB card as FAT32. Create the `music` folder at the card root and put the WAV files in it:

```text
SD card root
├── music/
│   ├── 001.wav
│   ├── 002.wav
│   └── 003.wav
└── (optional copy of tag_map.csv for your records)
```

Use ordinary **16-bit integer PCM WAV**, **44.1 kHz**, stereo, with the same mono program in left and right channels (dual-mono). Avoid 24-bit/32-bit float WAV, compressed WAV, unusual metadata, and MP3 renamed as `.wav`. Export from a lossless source; converting a low-quality MP3 to WAV does not restore lost sound.

The firmware uses `/music/001.wav` style paths. Each path in `firmware/tag_map.h` must exactly match the file name (FAT file paths are usually case-insensitive, but keep the spelling consistent). A 16 GB card holds roughly 23–25 hours of 16-bit 44.1 kHz stereo PCM audio before file-system overhead.

To add a song: copy a WAV file to `/music/`, add its UID and path to `firmware/fridge_nfc_player/tag_map.h`, then upload the firmware again. `firmware/tag_map.csv` is a human-readable inventory; the starter firmware does not parse the CSV automatically.

