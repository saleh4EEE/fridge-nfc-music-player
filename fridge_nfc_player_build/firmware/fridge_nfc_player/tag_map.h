#pragma once

// Replace each UID placeholder with the UID printed in the Arduino Serial Monitor.
// Use uppercase hex, no spaces or separators. Example: "04A1B2C3D4E5F6".
// The UID is the lookup key; the music itself stays on the SD card.
struct TagTrack {
  const char *uidHex;
  const char *path;
};

static const TagTrack TAG_TRACKS[] = {
  {"REPLACE_WITH_UID_HEX_1", "/music/001.wav"},
  {"REPLACE_WITH_UID_HEX_2", "/music/002.wav"},
  {"REPLACE_WITH_UID_HEX_3", "/music/003.wav"},
};

static constexpr size_t TAG_TRACK_COUNT = sizeof(TAG_TRACKS) / sizeof(TAG_TRACKS[0]);

