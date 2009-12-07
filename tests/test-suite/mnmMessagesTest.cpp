#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <MNM.h>

struct MNMGlobalFixture {
	MNMGlobalFixture() {
	}

	MNMGlobal global;
	MNMGlobal global2;
};

TEST_F (MNMGlobalFixture, MNMGlobalSysex) {
	global.autotrackChannel = 1;
	global.baseChannel = 2;
	global.channelSpan = 3;
	global.multitrigChannel = 4;
	global.multimapChannel = 5;
	global.clockIn = true;
	global.clockOut = false;
	global.ctrlIn = true;
	global.ctrlOut = false;

	global.transportIn = true;
	global.sequencerOut = false;
	global.arpOut = true;
	global.transportOut = false;
	global.midiClockOut = true;
	global.pgmChangeOut = false;

	global.note = 0;
	global.gate = 1;
	global.sense = 2;
	global.minVelocity = 3;
	global.maxVelocity = 4;

	for (uint8_t i = 0; i < 6; i++) {
		global.midiMachineChannels[i] = i;
		for (uint8_t j = 0; j < 4; j++) {
			global.ccDestinations[i][j] = i * j;
		}
		global.midiSeqLegato[i] = 1;
		global.legato[i] = 0;
	}
	for (uint8_t i = 0; i < 32; i++) {
		global.mapRange[i] = i;
		global.mapPattern[i] = i + 1;
		global.mapOffset[i] = i + 2;
		global.mapLength[i] = i + 3;
		global.mapTranspose[i] = i + 4;
		global.mapTiming[i] = i + 5;
	}
	global.globalRouting = 0;
	global.pgmChangeIn = true;
	global.baseFreq = 1000;

	uint8_t data[4092];
	uint16_t len = global.toSysex(data, sizeof(data));
	bool ret = global2.fromSysex(data + 6, len - 7);
	CHECK(ret);

	CHECK_EQUAL(global.autotrackChannel, global2.autotrackChannel);
	CHECK_EQUAL(global.baseChannel, global2.baseChannel);
	CHECK_EQUAL(global.channelSpan, global2.channelSpan);
	CHECK_EQUAL(global.multitrigChannel, global2.multitrigChannel);
	CHECK_EQUAL(global.clockIn, global2.clockIn);
	CHECK_EQUAL(global.clockOut, global2.clockOut);
	CHECK_EQUAL(global.ctrlIn, global2.ctrlIn);
	CHECK_EQUAL(global.ctrlOut, global2.ctrlOut);
	CHECK_EQUAL(global.transportIn, global2.transportIn);
	CHECK_EQUAL(global.sequencerOut, global2.sequencerOut);
	CHECK_EQUAL(global.arpOut, global2.arpOut);
	CHECK_EQUAL(global.transportOut, global2.transportOut);
	CHECK_EQUAL(global.keyboardOut, global2.keyboardOut);
	CHECK_EQUAL(global.midiClockOut, global2.midiClockOut);
	CHECK_EQUAL(global.pgmChangeOut, global2.pgmChangeOut);
	CHECK_EQUAL(global.note, global2.note);
	CHECK_EQUAL(global.gate, global2.gate);
	CHECK_EQUAL(global.sense, global2.sense);
	CHECK_EQUAL(global.minVelocity, global2.minVelocity);
	CHECK_EQUAL(global.maxVelocity, global2.maxVelocity);

	for (uint8_t i = 0; i < 6; i++) {
		CHECK_EQUAL(global.midiMachineChannels[i], global2.midiMachineChannels[i]);
		for (uint8_t j = 0; j < 4; j++) {
			CHECK_EQUAL(global.ccDestinations[i][j], global2.ccDestinations[i][j]);
		}
		CHECK_EQUAL(global.midiSeqLegato[i], global2.midiSeqLegato[i]);
		CHECK_EQUAL(global.legato[i], global2.legato[i]);
	}

	for (uint8_t i = 0; i < 32; i++) {
		CHECK_EQUAL(global.mapRange[i], global2.mapRange[i]);
		CHECK_EQUAL(global.mapPattern[i], global2.mapPattern[i]);
		CHECK_EQUAL(global.mapOffset[i], global2.mapOffset[i]);
		CHECK_EQUAL(global.mapLength[i], global2.mapLength[i]);
		CHECK_EQUAL(global.mapTranspose[i], global2.mapTranspose[i]);
		CHECK_EQUAL(global.mapTiming[i], global2.mapTiming[i]);
	}

	CHECK_EQUAL(global.globalRouting, global2.globalRouting);
	CHECK_EQUAL(global.pgmChangeIn, global2.pgmChangeIn);
	CHECK_EQUAL(global.baseFreq, global2.baseFreq);
}
	
struct MNMKitFixture {
	MNMKitFixture() {
	}

	MNMKit kit;
	MNMKit kit2;
};

TEST_F (MNMKitFixture, MNMKitSysex) {
	kit.origPosition = 15;
	strncpy(kit.name, "HELLO", 16);

	for (uint8_t i = 0; i < 6; i++) {
		kit.levels[i] = i;
		for (uint8_t j = 0; j < 72; j++) {
			kit.parameters[i][j] = i + j;
		}
		kit.models[i] = i + 1;
		kit.types[i] = i + 2;

		for (uint8_t j = 0; j < 6; j++) {
			kit.destPages[i][j][0] = i + j;
			kit.destPages[i][j][1] = i + j + 1;
			kit.destParams[i][j][0] = i + j + 2;
			kit.destParams[i][j][1] = i + j + 3;
		}

		kit.trigTracks[i] = i + 3;
	}

	kit.patchBusIn = 1000;
	kit.mirrorLR = 5;
	kit.mirrorUD = 6;
	kit.lpKeyTrack = 7;
	kit.hpKeyTrack = 8;
	kit.trigPortamento = 9;
	kit.trigLegatoAmp = 10;
	kit.trigLegatoLFO = 11;

	kit.commonMultimode = 3;
	kit.commonTiming = 64;

	kit.splitKey = 60;
	kit.splitRange = 40;
	
	uint8_t data[4092];
	uint16_t len = kit.toSysex(data, sizeof(data));
	bool ret = kit2.fromSysex(data + 6, len - 7);
	CHECK(ret);

	CHECK_EQUAL(kit.origPosition, kit2.origPosition);
	CHECK(!strncmp(kit.name, kit2.name, 16));

	for (uint8_t i = 0; i < 6; i++) {
		CHECK_EQUAL(kit.levels[i], kit2.levels[i]);
		for (uint8_t j = 0; j < 72; j++) {
			CHECK_EQUAL(kit.parameters[i][j], kit2.parameters[i][j]);
		}
		CHECK_EQUAL(kit.models[i], kit2.models[i]);
		CHECK_EQUAL(kit.types[i], kit2.types[i]);

		for (uint8_t j = 0; j < 6; j++) {
			CHECK_EQUAL(kit.destPages[i][j][0], kit2.destPages[i][j][0]);
			CHECK_EQUAL(kit.destPages[i][j][1], kit2.destPages[i][j][1]);
			CHECK_EQUAL((int)kit.destParams[i][j][0], (int)kit2.destParams[i][j][0]);
			CHECK_EQUAL((int)kit.destParams[i][j][1], (int)kit2.destParams[i][j][1]);
		}

		CHECK_EQUAL(kit.trigTracks[i], kit2.trigTracks[i]);
	}

	CHECK_EQUAL(kit.patchBusIn, kit2.patchBusIn);
	CHECK_EQUAL(kit.mirrorLR, kit2.mirrorLR);
	CHECK_EQUAL(kit.mirrorUD, kit2.mirrorUD);
	CHECK_EQUAL(kit.lpKeyTrack, kit2.lpKeyTrack);
	CHECK_EQUAL(kit.hpKeyTrack, kit2.hpKeyTrack);
	CHECK_EQUAL(kit.trigPortamento, kit2.trigPortamento);
	CHECK_EQUAL(kit.trigLegatoAmp, kit2.trigLegatoAmp);
	CHECK_EQUAL(kit.trigLegatoLFO, kit2.trigLegatoLFO);
	CHECK_EQUAL(kit.commonMultimode, kit2.commonMultimode);
	CHECK_EQUAL((int)kit.commonTiming, (int)kit2.commonTiming);
	CHECK_EQUAL(kit.splitKey, kit2.splitKey);
	CHECK_EQUAL(kit.splitRange, kit2.splitRange);
}
