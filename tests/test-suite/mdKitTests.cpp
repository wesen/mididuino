#include <CppUnitLite2.h>
#include <TestResultStdErr.h>
#include <Test.h>

#include <MD.h>

struct MDKitFixture {
  MDKitFixture() {
  }

  MDKit kit;
  MDKit kit2;


};

void initMDLFO(MDLFO &lfo) {
}

void initMDKit(MDKit &kit) {
  m_memcpy(kit.name, "FOOBAR", sizeof(kit.name));
  kit.origPosition = 0;

  for (uint8_t track = 0; track < 16; track++) {
    for (uint8_t param = 0; param < 24; param++) {
      kit.params[track][param] = param + track;
    }
    
    kit.models[track] = track;
    kit.levels[track] = track;
    kit.trigGroups[track] = track;
    kit.muteGroups[track] = track;
    
  }

  for (uint8_t param = 0; param < 8; param++) {
    kit.reverb[param] = param;
    kit.delay[param] = param * 2;
    kit.eq[param] = param * 3;
    kit.dynamics[param] = param * 4;
  }
}

#define M_CHECK_EQUAL(val1, val2) \
  if ((val1) != (val2)) { return false; } 

bool cmp_kits(MDKit &kit, MDKit &kit2) { 
  for (uint8_t track = 0; track < 16; track++) { 
    for (uint8_t param = 0; param < 24; param++) {
      M_CHECK_EQUAL(kit.params[track][param], kit2.params[track][param]); 
    } 
    M_CHECK_EQUAL(kit.models[track], kit2.models[track]);
    M_CHECK_EQUAL(kit.levels[track], kit2.levels[track]);
    M_CHECK_EQUAL(kit.trigGroups[track], kit2.trigGroups[track]);
    M_CHECK_EQUAL(kit.muteGroups[track], kit2.muteGroups[track]);
  } 
  for (uint8_t param = 0; param < 8; param++) {
    M_CHECK_EQUAL(kit.reverb[param], kit2.reverb[param]);
    M_CHECK_EQUAL(kit.delay[param], kit2.delay[param]);
    M_CHECK_EQUAL(kit.eq[param], kit2.eq[param]);
    M_CHECK_EQUAL(kit.dynamics[param], kit2.dynamics[param]);
  }

  return true;
}

TEST_F (MDKitFixture, MDKitSysex) {
  uint8_t data[4092];
  initMDKit(kit);
  uint16_t len = kit.toSysex(data, sizeof(data));
  bool ret = kit2.fromSysex(data + 6, len - 7);
  CHECK(ret);
  ret = cmp_kits(kit, kit2);
  CHECK(ret);
}

TEST_F (MDKitFixture, MDKitSwap) {
  initMDKit(kit);
  initMDKit(kit2);
  // check that both kits have been initialized successfully
  CHECK(cmp_kits(kit, kit2));

  // swap two tracks in the one kit
  kit2.swapTracks(0, 1);

  // check that the tracks have been swapped correctly
  for (uint8_t param = 0; param < 24; param++) {
    CHECK(kit.params[0][param] == kit2.params[1][param]);
    CHECK(kit.params[1][param] == kit2.params[0][param]);
  }
  CHECK(kit.models[0] == kit2.models[1]);
  CHECK(kit.models[1] == kit2.models[0]);
  CHECK(kit.levels[0] == kit2.levels[1]);
  CHECK(kit.levels[1] == kit2.levels[0]);
  CHECK(kit.trigGroups[0] == kit2.trigGroups[1]);
  CHECK(kit.trigGroups[1] == kit2.trigGroups[0]);
  CHECK(kit.muteGroups[0] == kit2.muteGroups[1]);
  CHECK(kit.muteGroups[1] == kit2.muteGroups[0]);

  // load the swapped kit from sysex and check that it has been serialized successfully
  MDKit kit3;
  initMDKit(kit3);
  uint8_t data[4092];
  uint16_t len = kit2.toSysex(data, sizeof(data));
  bool ret = kit3.fromSysex(data + 6, len - 7);
  CHECK(ret);
  ret = cmp_kits(kit2, kit3);
  CHECK(ret);

  // check that swapping tracks works in both directions
  kit2.swapTracks(0, 1);
  ret = cmp_kits(kit, kit2);
  CHECK(ret);

  // check that swapping the track on the copied to sysex pattern
  // brings back the original kit
  kit3.swapTracks(0, 1);
  ret = cmp_kits(kit, kit3);
  CHECK(ret);
}
