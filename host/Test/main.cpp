#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <Midi.h>
#include <MidiUartHost.h>

int main() {
  TestResultStdErr result;
  TestRegistry::Instance().Run(result);
  return (result.FailureCount());
}
