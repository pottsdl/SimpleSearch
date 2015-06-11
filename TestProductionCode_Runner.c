#include "unity.h"
#include <setjmp.h>
#include <stdio.h>

/* char MessageBuffer[50]; */

extern void setUp(void);
extern void tearDown(void);

extern void test_BasicEquality(void);

static void runTest(UnityTestFunction test)
{
  if (TEST_PROTECT())
  {
      setUp();
      test();
  }
  if (TEST_PROTECT() && !TEST_IS_IGNORED)
  {
    tearDown();
  }
}
void resetTest()
{
  tearDown();
  setUp();
}


int main(void)
{
  Unity.TestFile = "TestProductionCode.c";
  UnityBegin();

  // RUN_TEST calls runTest
  RUN_TEST(test_BasicEquality, 16);

  UnityEnd();
  return 0;
}
