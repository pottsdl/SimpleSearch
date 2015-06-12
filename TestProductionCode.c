#include "unity.h"
#include <string.h> /* for strcmp() */
#include <pthread.h> /* for pthread_* calls */

/* Include things to test */
#include "error_macros.h"
#include "work_queue.hpp"
#include "linked_list.h"

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

void createList(List_Node_t **list)
{
    int stat = STATUS_SUCCESS;
    Bool_t is_empty = FALSE;

    TEST_ASSERT_NOT_NULL(list);

    stat = create_list(list);
    TEST_ASSERT_EQUAL(stat, STATUS_SUCCESS);
    TEST_ASSERT_NOT_NULL(*list);
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

void test_createList(void)
{
    List_Node_t *list = NULL;
    int stat = STATUS_SUCCESS;

    stat = create_list(&list);

    /* For a new list:
     * - next should be NULL (no items)
     * - prev should be NULL (no items)
     * - list_item should be NULL (no items)
     */
    TEST_ASSERT_EQUAL(stat, STATUS_SUCCESS);
    TEST_ASSERT_NULL(list->next);
    TEST_ASSERT_NULL(list->prev);
    TEST_ASSERT_NULL(list->list_item);
}

void test_nullListCreate(void)
{
    int stat = create_list(NULL);
    TEST_ASSERT_NOT_EQUAL(stat, STATUS_SUCCESS);
}
void test_mallocFailedCreateList(void)
{
    List_Node_t *list = NULL;
    int stat = STATUS_SUCCESS;

    fakeMallocFailure = TRUE;
    stat = create_list(&list);
    fakeMallocFailure = FALSE;

    TEST_ASSERT_EQUAL(stat, STATUS_INVALID_PTR);
}

void test_emptyBlankList(void)
{
    List_Node_t *list = NULL;
    int stat = STATUS_SUCCESS;
    Bool_t is_empty = FALSE;

    stat = create_list(&list);
    TEST_ASSERT_EQUAL(stat, STATUS_SUCCESS);
    TEST_ASSERT_NOT_NULL(list);

    stat = is_list_empty(list, &is_empty);
    TEST_ASSERT_EQUAL(stat, STATUS_SUCCESS);
    TEST_ASSERT_EQUAL(is_empty, TRUE);
}

void test_addFirstListItem(void)
{
    List_Node_t *list = NULL;
    int stat = STATUS_SUCCESS;
    Bool_t is_empty = FALSE;

#if 0
    stat = create_list(&list);
    TEST_ASSERT_EQUAL(stat, STATUS_SUCCESS);
    TEST_ASSERT_NOT_NULL(list);
#else
    createList(&list);
#endif

    stat = add_item_after(list, NULL);
    TEST_ASSERT_EQUAL(stat, STATUS_SUCCESS);
    TEST_ASSERT_EQUAL(list->next, NULL);
    TEST_ASSERT_EQUAL(list->prev, NULL);
}

void test_addThreeListItems(void)
{
    List_Node_t *list = NULL;
    List_Node_t *currItem = NULL;
    int stat = STATUS_SUCCESS;
    Bool_t is_empty = FALSE;
    int list_items[] = { 1, 2, 3 };
    int itemsToAdd;
    int itemCount;

    createList(&list);

    for (itemsToAdd = 0; itemsToAdd < sizeof(list_items)/sizeof(*list_items);
            itemsToAdd++)
    {
        stat = add_item_after(list, (void*)&list_items[itemsToAdd]);
        TEST_ASSERT_EQUAL(stat, STATUS_SUCCESS);
    }
    for (itemCount = 0, currItem = list;
            currItem != NULL;
            itemCount++, currItem = currItem->next)
    {
        printf ("Item[%d] = %d\n", itemCount, *(int *)currItem->list_item);
    }
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
    /* ReaderWriterArgs_t args; */

    /* args.myQueue = myQueue; */
    stat = pthread_create(&thread2, NULL, WriterThread, (void*) myQueue);
    stat = pthread_create(&thread1, NULL, ReaderThread, (void*) myQueue);
    stat = pthread_create(&thread3, NULL, ReaderThread, (void*) myQueue);

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
    Work_Queue *q = (Work_Queue *) arg;
    string queueString = "";
    int tid = 0;

    while (queueString != "EXIT")
    {
        printf ("[%d] Waiting for not empty...\n", tid);
        q->waitForNotEmpty();
        printf ("[%d] Queue not empty, popping front\n", tid);
        queueString = q->pop_front();
        if (queueString != "EXIT")
        {
            printf ("[%d] Read:%s\n", tid, queueString.c_str());
        }
    }
}

void *WriterThread(void *arg)
{
    Work_Queue *q = (Work_Queue *) arg;

    sleep(5);

    q->push("ABC");
    q->push("DEF");
    q->push("GHI");
    q->push("JKL");
    q->push("EXIT");
    q->push("EXIT");

}
