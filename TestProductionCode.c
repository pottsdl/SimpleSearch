#include "unity.h"

/* Include things to test */
#include "error_macros.h"
#include "work_queue.h"
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

/* static const int STATUS_SUCCESS = 0; */

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
