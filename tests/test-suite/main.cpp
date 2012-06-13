#include "TestSuite.h"

int main() {
  TestResultStdErr result;
  TestRegistry::Instance().Run(result);
  return (result.FailureCount());
}
