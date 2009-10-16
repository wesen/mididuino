#ifndef MIDIUART_HOST_H__
#define MIDIUART_HOST_H__

#include <MidiUartParent.hh>
#include "RingBuffer.h"

#ifdef apple
#include "MidiUartOSX.h"
#endif

#include "MidiUartWin.h"

class MidiUartHostClass;
extern MidiUartHostClass MidiUart;

#endif /* MIDIUART_HOST_H__ */
