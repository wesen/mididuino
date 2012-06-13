#include "TestSuite.h"

#include <MD.h>

struct MDSongFixture {
  MDSongFixture() {
  }

  MDSong song;
  MDSong song2;
};

TEST_F (MDSongFixture, MDSongSysex) {
  uint8_t data[4092];
  uint16_t len = song.toSysex(data, sizeof(data));
  bool ret = song2.fromSysex(data + 6, len - 7);
  CHECK(ret);
}
