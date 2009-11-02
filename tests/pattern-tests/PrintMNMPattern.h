#ifndef PRINT_MNM_PATTERN_H__
#define PRINT_MNM_PATTERN_H__

#include "MNM.h"

class PrintMNMPattern : public MNMPattern {
 public:
	void printTrigs(uint64_t trigs) {
		for (int i = 0; i < length; i++) {
			if (IS_BIT_SET64(trigs, i)) {
				printf("X  ");
			} else {
				printf(".  ");
			}
		}
		printf("\n");
	}

	void printLocks(int track) {
		printf("lockpattern: %llx\n", lockPatterns[track]);

		for (int param = 0; param < 72; param++) {
			if (isParamLocked(track, param)) {
				printf("P%.2d (idx: %d): ", param, paramLocks[track][param]);

				for (int i = 0; i < length; i++) {
					uint8_t lock = getLock(track, i, param);
					if (lock != 255) {
						printf("%.3d ", lock);
					} else {
						printf("    ");
					}
				}
				printf("\n");
			}
		}
	}

	void print() {
		for (int i = 0; i < 6; i++) {
			printf("\nTrack %.2d:  ", i);
			printf("amp:\n");
			printTrigs(ampTrigs[i]);
			printf("filter:\n");
			printTrigs(filterTrigs[i]);
			printf("lfo:\n");
			printTrigs(lfoTrigs[i]);
			printf("off:\n");
			printTrigs(offTrigs[i]);
			printf("trigless:\n");
			printTrigs(triglessTrigs[i]);
			printf("chord:\n");
			printTrigs(chordTrigs[i]);

			printLocks(i);
		}
	}
};

#endif /* PRINT_MNM_PATTERN_H__ */
