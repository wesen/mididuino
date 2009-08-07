#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <Midi.h>
#include <MidiUartOsx.h>

MidiUartOSXClass MidiUart;
MidiClass Midi;

int main() {
  TestResultStdErr result;
  TestRegistry::Instance().Run(result);
  return (result.FailureCount());
}
