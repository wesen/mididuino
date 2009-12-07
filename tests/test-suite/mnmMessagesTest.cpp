#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <MNM.h>

struct MNMGlobalFixture {
	MNMGlobalFixture() {
	}

	MNMGlobal global;
};

TEST_F (MNMGlobalFixture, MNMGlobalSysex) {
}
	
