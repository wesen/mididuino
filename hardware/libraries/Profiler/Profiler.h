/*
 * MidiCtrl - Simple Profiler to benchmark embedded midi ctrl code
 *
 * (c) 2009 - 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#ifndef PROFILER_H__
#define PROFILER_H__

#include "PlatformConfig.h"
#include "Task.hh"

#ifndef HOST_MIDIDUINO
void enableProfiling();
void disableProfiling();
void sendProfilingData();
extern Task profilingTask;
#endif

#endif /* PROFILER_H__ */
