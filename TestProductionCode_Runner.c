#include "unity.h"
#include <setjmp.h>
#include <stdio.h>

/* char MessageBuffer[50]; */

extern void setUp(void);
extern void tearDown(void);

extern void test_BasicEquality(void);
extern void test_createNonZeroLengthQueue(void);
extern void test_queueEmptyOnCreate(void);

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
  RUN_TEST(test_BasicEquality, 30);
  RUN_TEST(test_createNonZeroLengthQueue, 38);
  RUN_TEST(test_queueEmptyOnCreate, 48);

  UnityEnd();
  return 0;
}
