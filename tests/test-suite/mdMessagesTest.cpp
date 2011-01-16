#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <MD.h>

struct MDGlobalFixture {
  MDGlobalFixture() {
  }

  MDGlobal global;
  MDGlobal global2;
};

TEST_F (MDGlobalFixture, MDGlobalSysex) {
  uint8_t data[4092];
  uint16_t len = global.toSysex(data, sizeof(data));
  bool ret = global2.fromSysex(data + 6, len - 7);
  CHECK(ret);

}

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
