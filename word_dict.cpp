/**
 * @file           word_dict.cpp
 * @brief:         <description>
 * @verbatim
 *******************************************************************************
 * Author:         Douglas L. Potts
 *
 * Date:           06/10/2015, <SCR #>
 *
 * History:
 * Date        SCR #  Name  Description
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************
 * @endverbatim
 */

/*******************************************************************************
 * System Includes
 *******************************************************************************
 */
#include <stdlib.h> /* for malloc() */
#include <string.h> /* for memset() */

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "common_types.h"
#include "error_macros.h"
#include "word_dict.hpp"
#include "unity.h"

/*******************************************************************************
 * Local Function Prototypes 
 *******************************************************************************
 */

/*******************************************************************************
 * Local Constants 
 *******************************************************************************
 */

/*******************************************************************************
 * File Scoped Variables 
 *******************************************************************************
 */

/*******************************************************************************
 ********************* E X T E R N A L  F U N C T I O N S **********************
 *******************************************************************************
 */

extern "C" {

    void wordDictConstruct(void)
    {
        Word_Dict myDictionary = new Word_Dict();
        TEST_ASSERT_NOT_NULL(myDictionary);
        TEST_ASSERT_EQUAL(myDictionary->isLocked, FALSE);
    }
}

Word_Dict::Word_Dict(void)
{
    int stat = 0;

    _mut_init = FALSE;
    _is_locked = FALSE;

    stat = pthread_mutex_init(&_mut, NULL);
    EXIT_EARLY_ON_ERROR(stat);
    _mut_init = TRUE;

cleanup:
    return;

error:
    goto cleanup;
}

Word_Dict::~Word_Dict(void)
{
    int stat = 0;

    _lock();
    while (!_filePathQueue.empty())
    {
        _filePathQueue.pop();
    }
    _unlock();


    stat = pthread_mutex_destroy(&_mut);
    if (stat != 0)
    {
        fprintf(stderr, "[%s, %d:%s] failed, stat=%d, errno=%d, %s\n",
                __FILE__, __LINE__, __FUNCTION__, stat, errno, strerror(errno));
    }
    _mut_init = FALSE;

}

void Word_Dict::_lock(void)
{
    int stat = STATUS_SUCCESS;

    stat = pthread_mutex_lock (&_mut);
    if (stat == STATUS_SUCCESS)
    {
        _is_locked = TRUE;
    }
}

void Word_Dict::lock(void)
{
    _lock();
}

void Word_Dict::_unlock(void)
{
    int stat = STATUS_SUCCESS;

    stat = pthread_mutex_unlock (&_mut);
    if (stat == STATUS_SUCCESS)
    {
        _is_locked = FALSE;
    }
}

void Word_Dict::unlock(void)
{
    _unlock();
}


/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */


