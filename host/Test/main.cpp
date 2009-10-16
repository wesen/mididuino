#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <Midi.h>
#include <MidiUartHost.h>

MidiUartHostClass MidiUart;
MidiClass Midi;
MidiClass Midi2;

void handleIncomingMidi() {
}

int main() {
  TestResultStdErr result;
  TestRegistry::Instance().Run(result);
  return (result.FailureCount());
}
