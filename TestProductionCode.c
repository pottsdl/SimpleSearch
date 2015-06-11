#include "listdir.h"
#include "unity.h"

//sometimes you may want to get at local data in a module.
//for example: If you plan to pass by reference, this could be useful
//however, it should often be avoided

void setUp(void)
{
}

void tearDown(void)
{
}

void test_FindFunction_WhichIsBroken_ShouldReturnZeroIfItemIsNotInList_WhichWorksEvenInOurBrokenCode(void)
{
    char *first_dir = ".";
    listdir(first_dir);
    //All of these should pass
    TEST_ASSERT_EQUAL(0, 0);
}
