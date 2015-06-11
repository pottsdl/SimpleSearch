#include "unity.h"

/* Include things to test */
#include "listdir.h"
#include "work_queue.h"

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

void setUp(void)
{
}

void tearDown(void)
{
}

void test_BasicEquality(void)
{
    char *first_dir = ".";
    listdir(first_dir);
    //All of these should pass
    TEST_ASSERT_EQUAL(0, 0);
}

void test_createNonZeroLengthQueue(void)
{
    Work_Queue_t *testQueue = NULL;
    static const int NONZERO_QUEUE_LENGTH = 5;

    testQueue = createWorkQueue(NONZERO_QUEUE_LENGTH);
    TEST_ASSERT_NOT_NULL(testQueue);
    TEST_ASSERT_EQUAL(testQueue->length, NONZERO_QUEUE_LENGTH);
}

void test_queueEmptyOnCreate(void)
{
    Work_Queue_t *testQueue = NULL;

    testQueue = createWorkQueue(VALID_QUEUE_LENGTH);
    TEST_ASSERT_NOT_NULL(testQueue);
    TEST_ASSERT_EQUAL(testQueue->queue_empty, TRUE);
}
