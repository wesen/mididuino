#include <MD.h>

bool supaTriggaActive = false;
bool restorePlayback = false;
bool breakdownActive = false;
bool storedBreakdownActive = false;

/* get ram r1 track automatically */
/* get ram p1 track automatically */
#define RAM_R1_TRACK 14
#define RAM_P1_TRACK 15

RangeEncoder flfEncoder(0, 127, "FLF", 0);
RangeEncoder flwEncoder(0, 127, "FLW", 127);
RangeEncoder fbEncoder(0, 127, "FB", 32);
RangeEncoder levEncoder(0, 127, "LEV", 100);
EncoderPage page(&flfEncoder, &flwEncoder, &fbEncoder, &levEncoder);

RangeEncoder timEncoder(0, 127, "TIM", 24);
RangeEncoder frqEncoder(0, 127, "FRQ", 0);
RangeEncoder modEncoder(0, 127, "MOD", 0);
EncoderPage page2(&timEncoder, &frqEncoder, &modEncoder, &fbEncoder);

MDEncoder pFlfEncoder(RAM_P1_TRACK, MODEL_FLTF, "FLF", 0);
MDEncoder pFlwEncoder(RAM_P1_TRACK, MODEL_FLTW, "FLW", 127);
MDEncoder pSrrEncoder(RAM_P1_TRACK, MODEL_SRR, "SRR", 0);
MDEncoder pVolEncoder(RAM_P1_TRACK, MODEL_VOL, "VOL", 100);
EncoderPage page4(&pFlfEncoder, &pFlwEncoder, &pSrrEncoder, &pVolEncoder);

