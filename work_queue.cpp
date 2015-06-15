/**
 * @file           work_queue.c
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
#include "work_queue.hpp"

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
Work_Queue::Work_Queue(void)
{
    int stat = 0;

    _mut_init = FALSE;
    _con_init = FALSE;
    _is_locked = FALSE;

    stat = pthread_cond_init(&_con, NULL);
    EXIT_EARLY_ON_ERROR(stat);
    _con_init = TRUE;
    stat = pthread_mutex_init(&_mut, NULL);
    EXIT_EARLY_ON_ERROR(stat);
    _mut_init = TRUE;

cleanup:
    return;

error:
    goto cleanup;
}

Work_Queue::~Work_Queue(void)
{
    int stat = 0;

    _lock();
    while (!_filePathQueue.empty())
    {
        _filePathQueue.pop();
    }
    _unlock();

    stat = pthread_cond_destroy(&_con);
    if (stat != 0)
    {
        fprintf(stderr, "[%s, %d:%s] failed, stat=%d, errno=%d, %s\n",
                __FILE__, __LINE__, __FUNCTION__, stat, errno, strerror(errno));
    }
    _con_init = FALSE;

    stat = pthread_mutex_destroy(&_mut);
    if (stat != 0)
    {
        fprintf(stderr, "[%s, %d:%s] failed, stat=%d, errno=%d, %s\n",
                __FILE__, __LINE__, __FUNCTION__, stat, errno, strerror(errno));
    }
    _mut_init = FALSE;

}

void Work_Queue::_lock(void)
{
    int stat = STATUS_SUCCESS;

    stat = pthread_mutex_lock (&_mut);
    if (stat == STATUS_SUCCESS)
    {
        _is_locked = TRUE;
    }
}

void Work_Queue::lock(void)
{
    _lock();
}

void Work_Queue::_unlock(void)
{
    int stat = STATUS_SUCCESS;

    stat = pthread_mutex_unlock (&_mut);
    if (stat == STATUS_SUCCESS)
    {
        _is_locked = FALSE;
    }
}
void Work_Queue::unlock(void)
{
    _unlock();
}

void Work_Queue::_signal(void)
{
    _lock();
    pthread_cond_signal(&_con);
    _unlock();
}
void Work_Queue::_wait(void)
{
    _lock();
    pthread_cond_wait(&_con, &_mut);
    _unlock();
}

void Work_Queue::push(string filePath)
{
    _lock();
    _filePathQueue.push(filePath);
    _unlock();
    _signal();
}

void Work_Queue::pop(void)
{
    _lock();
    _filePathQueue.pop();
    _unlock();
}

void Work_Queue::waitForNotEmpty(void)
{
    if (empty() == TRUE)
    {
        _wait();
    }
}

string Work_Queue::pop_front(void)
{
    string front_item;

    _lock();
    front_item =_filePathQueue.front();
    _filePathQueue.pop();
    _unlock();
    return(front_item);
}

string Work_Queue::front(void)
{
    string front_item;

    _lock();
    front_item =_filePathQueue.front();
    _unlock();
    return(front_item);
}

unsigned int Work_Queue::size(void)
{
    unsigned int size = 0;
    _lock();
    size = _filePathQueue.size();
    _unlock();
    return(size);
}

Bool_t Work_Queue::empty()
{
    Bool_t isEmpty = 0;
    _lock();
    isEmpty = _filePathQueue.empty();
    _unlock();
    return(isEmpty);
}

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */


