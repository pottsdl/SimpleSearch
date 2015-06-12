#include "unity.h"
#include <string.h> /* for strcmp() */
#include <pthread.h> /* for pthread_* calls */

/* Include things to test */
#include "error_macros.h"
#include "work_queue.hpp"
#include "buffer_processing.hpp"
#include "word_dict.hpp"

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

void *ChildThread1(void *arg);
void *ChildThread2(void *arg);
void *ReaderThread(void *arg);
void *WriterThread(void *arg);

typedef struct
{
    Work_Queue *myQueue;
    int thread_indx;
} ReaderWriterArgs_t;

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
    //All of these should pass
    TEST_ASSERT_EQUAL(0, 0);
}

void test_createNonZeroLengthQueue(void)
{
    Work_Queue_t *testQueue = NULL;

    testQueue = createWorkQueue(NONZERO_QUEUE_LENGTH);
    TEST_ASSERT_NOT_NULL(testQueue);
}

void test_queueEmptyOnCreate(void)
{
    Work_Queue_t *testQueue = NULL;

    testQueue = createWorkQueue(VALID_QUEUE_LENGTH);
    TEST_ASSERT_NOT_NULL(testQueue);
}

void test_queueLock(void)
{
    Work_Queue_t *testQueue = NULL;
    int stat = STATUS_SUCCESS;

    testQueue = createWorkQueue(VALID_QUEUE_LENGTH);
    TEST_ASSERT_NOT_NULL(testQueue);

    stat = pthread_mutex_lock (&testQueue->mut);
    TEST_ASSERT_EQUAL(stat, STATUS_SUCCESS);

    stat = pthread_mutex_unlock (&testQueue->mut);
    TEST_ASSERT_EQUAL(stat, STATUS_SUCCESS);
}



void test_newWorkQueue(void)
{
    Work_Queue *myQueue = new Work_Queue();

    TEST_ASSERT_NOT_NULL(myQueue);
}

void test_lockWorkQueue(void)
{
    Work_Queue *myQueue = new Work_Queue();

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->lock();
    TEST_ASSERT_EQUAL(myQueue->isLocked(), TRUE);
}

void test_unlockWorkQueue(void)
{
    Work_Queue *myQueue = new Work_Queue();

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->lock();
    TEST_ASSERT_EQUAL(myQueue->isLocked(), TRUE);
    myQueue->unlock();
    TEST_ASSERT_EQUAL(myQueue->isLocked(), FALSE);
}

void test_pushWorkQueue(void)
{
    Work_Queue *myQueue = new Work_Queue();
    string testString = "TESTTESTTEST";

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->push(testString);
}

void test_frontWorkQueue(void)
{
    Work_Queue *myQueue = new Work_Queue();
    string testString = "TESTTESTTEST";
    const char *expectedStr = testString.c_str();
    const char *actualStr;

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->push(testString);
    /* printf("Queue item: %s\n", myQueue->front().c_str()); */
    actualStr = myQueue->front().c_str();
    TEST_ASSERT_EQUAL(strcmp(expectedStr, actualStr), 0);
}

void test_popWorkQueue(void)
{
    Work_Queue *myQueue = new Work_Queue();
    string testString = "TESTTESTTEST";

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->push(testString);
    myQueue->pop();
}

void test_sizeWorkQueueOneEntry(void)
{
    Work_Queue *myQueue = new Work_Queue();
    string testString = "TESTTESTTEST";
    unsigned int size = 0;

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->push(testString);
    size = myQueue->size();
    TEST_ASSERT_EQUAL(size, 1);
}

void test_sizeWorkQueueMoreThanOneEntry(void)
{
    Work_Queue *myQueue = new Work_Queue();
    string testString = "TESTTESTTEST";
    unsigned int size = 0;

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->push(testString);
    myQueue->push(testString);
    myQueue->push(testString);

    size = myQueue->size();
    TEST_ASSERT_EQUAL(size, 3);
}

void test_multiThreadAdd(void)
{
    Work_Queue *myQueue = new Work_Queue();
    pthread_t thread1;
    pthread_t thread2;
    int stat = 0;

    stat = pthread_create(&thread1, NULL, ChildThread1, (void*) myQueue);
    stat = pthread_create(&thread2, NULL, ChildThread2, (void*) myQueue);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    while (!myQueue->empty())
    {
        printf("Queue item: %s\n", myQueue->front().c_str());
        myQueue->pop();
    }
}
void *ChildThread1(void *arg)
{
    Work_Queue *q = (Work_Queue *) arg;
    printf ("In ChildThread1\n"); fflush(stdout);
    q->push("AAA");
    sleep(1);
    q->push("CCC");
    sleep(1);
}
void *ChildThread2(void *arg)
{
    Work_Queue *q = (Work_Queue *) arg;
    printf ("In ChildThread2\n"); fflush(stdout);
    sleep(1);
    q->push("BBB");
    sleep(1);
    q->push("DDD");
}

void test_queueReaderWriter(void)
{
    Work_Queue *myQueue = new Work_Queue();
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    int stat = 0;
    ReaderWriterArgs_t args1 = { myQueue, 1 };
    ReaderWriterArgs_t args2 = { myQueue, 2 };
    ReaderWriterArgs_t args3 = { myQueue, 3 };

    stat = pthread_create(&thread2, NULL, WriterThread, (void*) &args1);
    stat = pthread_create(&thread1, NULL, ReaderThread, (void*) &args2);
    stat = pthread_create(&thread3, NULL, ReaderThread, (void*) &args3);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    while (!myQueue->empty())
    {
        printf("Queue item: %s\n", myQueue->front().c_str());
        myQueue->pop();
    }
}
void *ReaderThread(void *arg)
{
    ReaderWriterArgs_t *_arg = (ReaderWriterArgs_t *) arg;
    Work_Queue *q = _arg->myQueue;
    string queueString = "";
    int tid = _arg->thread_indx;

    while (queueString != "EXIT")
    {
        if (q->empty())
        {
            printf ("[%d] Waiting for not empty...\n", tid);
            q->waitForNotEmpty();
        }
        printf ("[%d] Queue not empty, popping front\n", tid);
        queueString = q->pop_front();
        printf ("[%d] Read:%s\n", tid, queueString.c_str());
    }
}

void *WriterThread(void *arg)
{
    ReaderWriterArgs_t *_arg = (ReaderWriterArgs_t *) arg;
    Work_Queue *q = _arg->myQueue;
    int tid = _arg->thread_indx;

    sleep(5);

    printf ("[%d] Pushing ABC\n", tid);
    q->push("ABC");
    printf ("[%d] Pushing DEF\n", tid);
    q->push("DEF");
    printf ("[%d] Pushing GHI\n", tid);
    q->push("GHI");
    printf ("[%d] Pushing JKL\n", tid);
    q->push("JKL");
    printf ("[%d] XXX  Pushing EXIT  XXXX\n", tid);
    q->push("EXIT");
    printf ("[%d] XXX  Pushing EXIT  XXXX\n", tid);
    q->push("EXIT");

}

void test_goodIsLowerWordChars(void)
{
    char goodList[] = "abcdefghijklmnopqrstuvwxyz";
    int charIdx = 0;

    for (charIdx = 0; charIdx < strlen(goodList); charIdx++)
    {
        Bool_t isGood = FALSE;

        isGood = isWordChar(goodList[charIdx]);
        if (isGood == FALSE)
        {
            fprintf (stderr, "Test FAILED '%c' should have been GOOD.\n",
                    goodList[charIdx]);
        }
        TEST_ASSERT_EQUAL(isWordChar(goodList[charIdx]), TRUE);
    }
}

void test_goodIsUpperWordChars(void)
{
    char goodList[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int charIdx = 0;

    for (charIdx = 0; charIdx < strlen(goodList); charIdx++)
    {
        Bool_t isGood = FALSE;

        isGood = isWordChar(goodList[charIdx]);
        if (isGood == FALSE)
        {
            fprintf (stderr, "Test FAILED '%c' should have been GOOD.\n",
                    goodList[charIdx]);
        }
        TEST_ASSERT_EQUAL(isWordChar(goodList[charIdx]), TRUE);
    }
}

void test_goodIsNumWordChars(void)
{
    char goodList[] = "0123456789";
    int charIdx = 0;

    for (charIdx = 0; charIdx < strlen(goodList); charIdx++)
    {
        Bool_t isGood = FALSE;

        isGood = isWordChar(goodList[charIdx]);
        if (isGood == FALSE)
        {
            fprintf (stderr, "Test FAILED '%c' should have been GOOD.\n",
                    goodList[charIdx]);
        }
        TEST_ASSERT_EQUAL(isWordChar(goodList[charIdx]), TRUE);
    }
}
void test_badIsWordChars(void)
{
    char goodList[] = "-!@#$%^&*()+_~`";
    int charIdx = 0;

    for (charIdx = 0; charIdx < strlen(goodList); charIdx++)
    {
        Bool_t isGood = FALSE;

        isGood = isWordChar(goodList[charIdx]);
        if (isGood == TRUE)
        {
            fprintf (stderr, "Test FAILED '%c' should have been BAD.\n",
                    goodList[charIdx]);
        }
        TEST_ASSERT_EQUAL(isWordChar(goodList[charIdx]), FALSE);
    }
}

void test_bufferWordMultiple(void)
{
}

void test_bufferWordOneOnly(void)
{
    char mybuf[] = "abc";
    int buflen = strlen(mybuf);
    int ret = -1;
    char *result_word = NULL;

    ret = processBufferForWords(mybuf, buflen, &result_word);

    if (result_word != NULL)
    {
        printf ("Found word: %s\n", result_word);
    }
    TEST_ASSERT_NOT_NULL(result_word);
    TEST_ASSERT_EQUAL(strcmp(result_word, "abc"), 0);
    /* Should've processed up to 'c', so all 3 */
    TEST_ASSERT_EQUAL(ret, 3);
    return;
}
void test_bufferWordOneStartsLate(void)
{
    char mybuf[] = "!-=abc";
    int buflen = strlen(mybuf);
    int ret = -1;
    char *result_word = NULL;

    ret = processBufferForWords(mybuf, buflen, &result_word);

    if (result_word != NULL)
    {
        printf ("Found word: %s\n", result_word);
    }
    TEST_ASSERT_NOT_NULL(result_word);
    TEST_ASSERT_EQUAL(strcmp(result_word, "abc"), 0);
    /* Should've processed up to 'c', so all 6 */
    TEST_ASSERT_EQUAL(ret, 6);
    return;
}
void test_bufferWordOneGarbageAtEnd(void)
{
    char mybuf[] = "zzz!-=";
    int buflen = strlen(mybuf);
    int ret = -1;
    char *result_word = NULL;

    ret = processBufferForWords(mybuf, buflen, &result_word);

    if (result_word != NULL)
    {
        printf ("Found word: %s\n", result_word);
    }
    TEST_ASSERT_NOT_NULL(result_word);
    TEST_ASSERT_EQUAL(strcmp(result_word, "zzz"), 0);
    /* Should've processed up to 3rd 'z', so 8 */
    TEST_ASSERT_EQUAL(ret, 3);
    return;
}
void test_bufferWordOneGarbageAtBothEnds(void)
{
    char mybuf[] = "===*&zzz!-=";
    int buflen = strlen(mybuf);
    int ret = -1;
    char *result_word = NULL;

    ret = processBufferForWords(mybuf, buflen, &result_word);

    if (result_word != NULL)
    {
        printf ("Found word: %s\n", result_word);
    }
    TEST_ASSERT_NOT_NULL(result_word);
    TEST_ASSERT_EQUAL(strcmp(result_word, "zzz"), 0);
    /* Should've processed up to 3rd 'z', so 8 */
    TEST_ASSERT_EQUAL(ret, 8);
    return;
}
void test_bufferWordTwoWordsOnly(void)
{
    char mybuf[] = "zzz=abc";
    int buflen = strlen(mybuf);
    int ret = -1;
    char *result_word = NULL;

    ret = processBufferForWords(mybuf, buflen, &result_word);

    if (result_word != NULL)
    {
        printf ("Found word: %s\n", result_word);
    }
    TEST_ASSERT_NOT_NULL(result_word);
    TEST_ASSERT_EQUAL(strcmp(result_word, "zzz"), 0);
    /* Should've processed up to 3rd 'z', so 3 */
    TEST_ASSERT_EQUAL(ret, 3);
    return;
}
void test_bufferWordTwoWordsWithGarbageBothEnds(void)
{
    char mybuf[] = "!!!zzz=abc!!!";
    int buflen = strlen(mybuf);
    int ret = -1;
    char *result_word = NULL;

    ret = processBufferForWords(mybuf, buflen, &result_word);

    if (result_word != NULL)
    {
        printf ("Found word: %s\n", result_word);
    }
    TEST_ASSERT_NOT_NULL(result_word);
    TEST_ASSERT_EQUAL(strcmp(result_word, "zzz"), 0);
    /* Should've processed up to 3rd 'z', so 6 */
    TEST_ASSERT_EQUAL(ret, 6);
    return;
}

void test_WordDictConstruct(void)
{
    wordDictConstruct();
}
void test_WordDictInsert(void)
{
    wordDictAddItems();
}
void test_WordDictLock(void)
{
    wordDictLock();
}
void test_WordDictUnlock(void)
{
    wordDictUnlock();
}
void test_WordDictIterItems(void)
{
    wordDictIterItems();
}
