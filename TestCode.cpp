#include "unity.h"
#include <string.h> /* for strcmp() */
#include <pthread.h> /* for pthread_* calls */

/* Include things to test */
#include "error_macros.h"

/**
 * Provide constant for a non-zero length, which should be valid, exact value
 * is unimportant, only that it ISN'T zero (0)
 */
static const int NONZERO_QUEUE_LENGTH = 5;
/**
 * Constant is to indicate that this is for a valid queue length, but exact
 * value doesn't matter so don't use literal.
 */
static const int VALID_QUEUE_LENGTH = 1;


//sometimes you may want to get at local data in a module.
//for example: If you plan to pass by reference, this could be useful
//however, it should often be avoided


extern "C" void setUp(void)
{
}

extern "C" void tearDown(void)
{
}

extern "C" void test_BasicEquality(void)
{
    //All of these should pass
    TEST_ASSERT_EQUAL(0, 0);
}
