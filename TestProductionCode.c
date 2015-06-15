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

/*
 ***********************************************************************
 *                              Work Queue Tests
 ***********************************************************************
 */

/**
 *******************************************************************************
 * @brief test_newWorkQueue - Test constructing a Work_Queue
 *******************************************************************************
 */
void test_newWorkQueue(void)
{
    Work_Queue *myQueue = new Work_Queue();

    TEST_ASSERT_NOT_NULL(myQueue);
}
/**
 *******************************************************************************
 * @brief test_lockWorkQueue - Test manually locking a Work_Queue
 *******************************************************************************
 */
void test_lockWorkQueue(void)
{
    Work_Queue *myQueue = new Work_Queue();

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->lock();
    TEST_ASSERT_EQUAL(myQueue->isLocked(), TRUE);
}
/**
 *******************************************************************************
 * @brief test_unlockWorkQueue - Test manually unlocking a Work_Queue (after
 * locking).
 *******************************************************************************
 */
void test_unlockWorkQueue(void)
{
    Work_Queue *myQueue = new Work_Queue();

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->lock();
    TEST_ASSERT_EQUAL(myQueue->isLocked(), TRUE);
    myQueue->unlock();
    TEST_ASSERT_EQUAL(myQueue->isLocked(), FALSE);
}
/**
 *******************************************************************************
 * @brief test_pushWorkQueue - Test adding file path to Work_Queue
 *******************************************************************************
 */
void test_pushWorkQueue(void)
{
    Work_Queue *myQueue = new Work_Queue();
    string filePath = "TESTTESTTEST";

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->push(filePath);
}
/**
 *******************************************************************************
 * @brief test_frontWorkQueue - Test getting the file path from front
 *******************************************************************************
 */
void test_frontWorkQueue(void)
{
    Work_Queue *myQueue = new Work_Queue();
    string filePath = "TESTTESTTEST";
    const char *expectedStr = filePath.c_str();
    const char *actualStr;

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->push(filePath);
    actualStr = myQueue->front().c_str();
    TEST_ASSERT_EQUAL_STRING(expectedStr, actualStr);
}
/**
 *******************************************************************************
 * @brief test_popWorkQueue - Test adding file path, and then popping it off.
 *******************************************************************************
 */
void test_popWorkQueue(void)
{
    Work_Queue *myQueue = new Work_Queue();
    string filePath = "TESTTESTTEST";

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->push(filePath);
    myQueue->pop();
}
/**
 *******************************************************************************
 * @brief test_sizeWorkQueueOneEntry - Test adding entry and getting queue size
 *******************************************************************************
 */
void test_sizeWorkQueueOneEntry(void)
{
    Work_Queue *myQueue = new Work_Queue();
    string filePath = "TESTTESTTEST";
    unsigned int size = 0;

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->push(filePath);
    size = myQueue->size();
    TEST_ASSERT_EQUAL(size, 1);
}
/**
 *******************************************************************************
 * @brief test_sizeWorkQueueMoreThanOneEntry - Test adding multiple entries and
 * getting queue size
 *******************************************************************************
 */
void test_sizeWorkQueueMoreThanOneEntry(void)
{
    Work_Queue *myQueue = new Work_Queue();
    string filePath = "TESTTESTTEST";
    unsigned int size = 0;

    TEST_ASSERT_NOT_NULL(myQueue);

    myQueue->push(filePath);
    myQueue->push(filePath);
    myQueue->push(filePath);

    size = myQueue->size();
    TEST_ASSERT_EQUAL(size, 3);
}

/**
 *******************************************************************************
 * @brief test_multiThreadAdd - Test adding file path to Work_Queue from
 * different threads.
 *******************************************************************************
 */
void test_multiThreadAdd(void)
{
    Work_Queue *myQueue = new Work_Queue();
    pthread_t thread1;
    pthread_t thread2;
    int stat = 0;

    stat = pthread_create(&thread1, NULL, ChildThread1, (void*) myQueue);
    TEST_ASSERT_EQUAL(stat, 0);
    stat = pthread_create(&thread2, NULL, ChildThread2, (void*) myQueue);
    TEST_ASSERT_EQUAL(stat, 0);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    while (!myQueue->empty())
    {
        printf("Queue item: %s\n", myQueue->front().c_str());
        myQueue->pop();
    }
}

/**
 *******************************************************************************
 * @brief ChildThread1 - Child thread #1 for test_multiThreadAdd
 *
 * <!-- Parameters -->
 *      @param[in]      arg            Pointer to structure containing
 *                                     arguments.
 *
 * <!-- Returns -->
 *      None (if return type is void)
 *
 * @par Description:
 *      Push two "file paths" onto the queue, sleeping for a second in between.
 *
 *******************************************************************************
 */
void *ChildThread1(void *arg)
{
    Work_Queue *q = (Work_Queue *) arg;
    printf ("In ChildThread1\n"); fflush(stdout);
    q->push("AAA");
    sleep(1);
    q->push("CCC");
    sleep(1);
}
/**
 *******************************************************************************
 * @brief ChildThread2 - Child thread #2 for test_multiThreadAdd
 *
 * <!-- Parameters -->
 *      @param[in]      arg            Pointer to structure containing
 *                                     arguments.
 *
 * <!-- Returns -->
 *      None (if return type is void)
 *
 * @par Description:
 *      Push two "file paths" onto the queue, sleeping for a second in between.
 *
 *******************************************************************************
 */
void *ChildThread2(void *arg)
{
    Work_Queue *q = (Work_Queue *) arg;
    printf ("In ChildThread2\n"); fflush(stdout);
    sleep(1);
    q->push("BBB");
    sleep(1);
    q->push("DDD");
}

/**
 *******************************************************************************
 * @brief test_queueReaderWriter - Test 2 readers and 1 writer threads pushing
 * file paths onto the queue and reading them off when they arrive.
 *******************************************************************************
 */
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
    TEST_ASSERT_EQUAL(stat, 0);
    stat = pthread_create(&thread1, NULL, ReaderThread, (void*) &args2);
    TEST_ASSERT_EQUAL(stat, 0);
    stat = pthread_create(&thread3, NULL, ReaderThread, (void*) &args3);
    TEST_ASSERT_EQUAL(stat, 0);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    while (!myQueue->empty())
    {
        printf("Queue item: %s\n", myQueue->front().c_str());
        myQueue->pop();
    }
}
/**
 *******************************************************************************
 * @brief ReaderThread - Child thread, reading thread, for
 * test_queueReaderWriter
 *
 * <!-- Parameters -->
 *      @param[in]      arg            Pointer to structure containing
 *                                     arguments.
 *
 * <!-- Returns -->
 *      None (if return type is void)
 *
 * @par Description:
 *      Keep trying to read file paths off of the queue until the EXIT command
 *      is received.  Print out the file paths read from the queue.
 *******************************************************************************
 */
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

    return(NULL);
}
/**
 *******************************************************************************
 * @brief WriterThread - Child thread, writing thread, for
 * test_queueReaderWriter
 *
 * <!-- Parameters -->
 *      @param[in]      arg            Pointer to structure containing
 *                                     arguments.
 *
 * <!-- Returns -->
 *      None (if return type is void)
 *
 * @par Description:
 *      Push 6 "file paths" onto the queue, and then send 2 EXIT commands (test
 *      has 2 reader threads spawned).
 *******************************************************************************
 */
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

    return(NULL);
}

/*
 ***********************************************************************
 *                             Word Parsing Tests
 ***********************************************************************
 */
/**
 *******************************************************************************
 * @brief test_goodIsLowerWordChars - test checking for all of the lower case
 * "word" characters.
 *******************************************************************************
 */
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
/**
 *******************************************************************************
 * @brief test_goodIsUpperWordChars - test checking for all of the upper case
 * "word" characters.
 *******************************************************************************
 */
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
/**
 *******************************************************************************
 * @brief test_goodIsNumWordChars - test checking for all of the number "word"
 * characters.
 *******************************************************************************
 */
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
/**
 *******************************************************************************
 * @brief test_badIsWordChars - test checking for some of the non-word
 * characters.
 *******************************************************************************
 */
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

/*
 ***********************************************************************
 *                                Buffer Tests
 ***********************************************************************
 */
/**
 *******************************************************************************
 * @brief test_bufferWordOneOnly - test processing a character buffer containing
 * only a single word.
 *******************************************************************************
 */
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
/**
 *******************************************************************************
 * @brief test_bufferWordOneStartsLate - test processing a character buffer
 * containing only a single word, but ends in that word.
 *******************************************************************************
 */
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
/**
 *******************************************************************************
 * @brief test_bufferWordOneGarbageAtEnd - test processing a character buffer
 * containing only a single word at the beginning with non-word characters
 * following.
 *******************************************************************************
 */
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
/**
 *******************************************************************************
 * @brief test_bufferWordOneGarbageAtEnd - test processing a character buffer
 * containing only a single word in the middle, or surrounded by non-word
 * characters.
 *******************************************************************************
 */
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
/**
 *******************************************************************************
 * @brief test_bufferWordTwoWordsOnly - test processing a character buffer
 * containing two words with only a single non-word char separating.
 *******************************************************************************
 */
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
/**
 *******************************************************************************
 * @brief test_bufferWordTwoWordsWithGarbageBothEnds - test processing a
 * character buffer containing two words, with non-word charaters at both ends
 * and separating.
 *******************************************************************************
 */
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
/**
 *******************************************************************************
 * @brief test_Buffer4WordsAtStart - Test processing a buffer with 4 words, with
 * the first starting at the beginning of the buffer.
 *******************************************************************************
 */
void test_Buffer4WordsAtStart(void)
{
    bufferProcFourWordsAtBegin();
}
/**
 *******************************************************************************
 * @brief test_Buffer1WordsAtStart - Test processing a buffer with 1 word,
 * starting at the beginning of the buffer.
 *******************************************************************************
 */
void test_Buffer1WordsAtStart(void)
{
    bufferProcOneWordAtBegin();
}
/**
 *******************************************************************************
 * @brief test_lockWorkQueue - Test processing a buffer with part of a word at
 * the beginning and off then end of the processing buffer.
 *******************************************************************************
 */
void test_BufferFullOffEnds(void)
{
    bufferProcFullBuffer();
}
/**
 *******************************************************************************
 * @brief test_fileProcess - Test processing a mocked file, which has enough
 * data to go past a single read's worth, and will have a word at the end of the
 * buffer.
 *******************************************************************************
 */
void test_fileProcess(void)
{
    fileProcess();
}

/*
 ***********************************************************************
 *                                Dictionary Tests
 ***********************************************************************
 */
/**
 *******************************************************************************
 * @brief test_WordDictConstruct - Construct a Word_Dict
 *******************************************************************************
 */
void test_WordDictConstruct(void)
{
    wordDictConstruct();
}
/**
 *******************************************************************************
 * @brief test_WordDictConstruct - Add multiple pairs of word and word count to
 * the dictionary.  Verify that they were inserted.
 *******************************************************************************
 */
void test_WordDictInsert(void)
{
    wordDictAddItems();
}
/**
 *******************************************************************************
 * @brief test_WordDictLock - Test manually locking the Word_Dict
 *******************************************************************************
 */
void test_WordDictLock(void)
{
    wordDictLock();
}
/**
 *******************************************************************************
 * @brief test_WordDictUnlock - Test manually unlocking the Word_Dict (after
 * locking)
 *******************************************************************************
 */
void test_WordDictUnlock(void)
{
    wordDictUnlock();
}
/**
 *******************************************************************************
 * @brief test_WordDictIterItems - Test adding in multiple dictionary pairs, and
 * then using the class methods to iterate through all the dictionary items.
 *******************************************************************************
 */
void test_WordDictIterItems(void)
{
    wordDictIterItems();
}
/**
 *******************************************************************************
 * @brief test_WordSearchGoodFind - Test search on word known to be in the
 * dictionary.
 *******************************************************************************
 */
void test_WordSearchGoodFind(void)
{
    wordDictGoodFind();
}
/**
 *******************************************************************************
 * @brief test_WordSearchGoodNoFind - Test search on word known to NOT be in the
 * dictionary.
 *******************************************************************************
 */
void test_WordSearchGoodNoFind(void)
{
    wordDictGoodMiss();
}
/**
 *******************************************************************************
 * @brief test_WordSearchGoodNoFind - Test search on word known to be in the
 * dictionary, and then incrementing its word count.
 *******************************************************************************
 */
void test_wordDictGoodIncrement(void)
{
    wordDictGoodIncrement();
}
