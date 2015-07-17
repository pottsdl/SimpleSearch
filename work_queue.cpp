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
#include <stdlib.h>             /* for malloc() */
#include <string.h>             /* for memset() */

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

/**
 *******************************************************************************
 * @brief Work_Queue - Constructor
 *******************************************************************************
 */
Work_Queue::Work_Queue (void)
{
    int stat = 0;

    _mut_init = FALSE;
    _con_init = FALSE;
    _is_locked = FALSE;

    stat = pthread_cond_init (&_con, NULL);
    EXIT_EARLY_ON_ERROR (stat);
    _con_init = TRUE;
    stat = pthread_mutex_init (&_mut, NULL);
    EXIT_EARLY_ON_ERROR (stat);
    _mut_init = TRUE;

  cleanup:
    return;

  error:
    goto cleanup;
}

/**
 *******************************************************************************
 * @brief ~Work_Queue - Destructor
 *******************************************************************************
 */
Work_Queue::~Work_Queue (void)
{
    int stat = 0;

    _lock ();
    while (!_filePathQueue.empty ())
    {
        _filePathQueue.pop ();
    }
    _unlock ();

    stat = pthread_cond_destroy (&_con);
    if (stat != 0)
    {
        fprintf (stderr, "[%s, %d:%s] failed, stat=%d, errno=%d, %s\n",
                 __FILE__, __LINE__, __FUNCTION__, stat, errno,
                 strerror (errno));
    }
    _con_init = FALSE;

    stat = pthread_mutex_destroy (&_mut);
    if (stat != 0)
    {
        fprintf (stderr, "[%s, %d:%s] failed, stat=%d, errno=%d, %s\n",
                 __FILE__, __LINE__, __FUNCTION__, stat, errno,
                 strerror (errno));
    }
    _mut_init = FALSE;

}

/**
 *******************************************************************************
 * @brief _lock - Class private lock method, for class access
 *******************************************************************************
 */
void Work_Queue::_lock (void)
{
    int stat = STATUS_SUCCESS;

    stat = pthread_mutex_lock (&_mut);
    if (stat == STATUS_SUCCESS)
    {
        _is_locked = TRUE;
    }
}

/**
 *******************************************************************************
 * @brief lock - Public lock method.
 *******************************************************************************
 */
void Work_Queue::lock (void)
{
    _lock ();
}

/**
 *******************************************************************************
 * @brief _unlock - Class private unlock method, for class access
 *******************************************************************************
 */
void Work_Queue::_unlock (void)
{
    int stat = STATUS_SUCCESS;

    stat = pthread_mutex_unlock (&_mut);
    if (stat == STATUS_SUCCESS)
    {
        _is_locked = FALSE;
    }
}

/**
 *******************************************************************************
 * @brief unlock - Public unlock method.
 *******************************************************************************
 */
void Work_Queue::unlock (void)
{
    _unlock ();
}

/**
 *******************************************************************************
 * @brief _signal - Class private condition variable signal method, for class access
 *******************************************************************************
 */
void Work_Queue::_signal (void)
{
    _lock ();
    pthread_cond_signal (&_con);
    _unlock ();
}

/**
 *******************************************************************************
 * @brief _wait - Class private condition variable wait method, for class access
 *******************************************************************************
 */
void Work_Queue::_wait (void)
{
    _lock ();
    pthread_cond_wait (&_con, &_mut);
    _unlock ();
}

/**
 *******************************************************************************
 * @brief push - Public push method.
 *******************************************************************************
 */
void Work_Queue::push (string filePath)
{
    _lock ();
    _filePathQueue.push (filePath);
    _unlock ();
    _signal ();
}

/**
 *******************************************************************************
 * @brief pop - Public pop method.
 *******************************************************************************
 */
void Work_Queue::pop (void)
{
    _lock ();
    _filePathQueue.pop ();
    _unlock ();
}

/**
 *******************************************************************************
 * @brief waitForNotEmpty - Public method to pend on an empty queue until new
 * item(s) have appeared.
 *******************************************************************************
 */
void Work_Queue::waitForNotEmpty (void)
{
    if (empty () == TRUE)
    {
        _wait ();
    }
}

/**
 *******************************************************************************
 * @brief pop_front - Public method to pop_front for underlying data structure.
 *******************************************************************************
 */
string Work_Queue::pop_front (void)
{
    string front_item;

    _lock ();
    front_item = _filePathQueue.front ();
    _filePathQueue.pop ();
    _unlock ();
    return (front_item);
}

/**
 *******************************************************************************
 * @brief front - Public method to get front for underlying data structure.
 *******************************************************************************
 */
string Work_Queue::front (void)
{
    string front_item;

    _lock ();
    front_item = _filePathQueue.front ();
    _unlock ();
    return (front_item);
}

/**
 *******************************************************************************
 * @brief size - Public method to get size for underlying data structure.
 *******************************************************************************
 */
unsigned int Work_Queue::size (void)
{
    unsigned int size = 0;

    _lock ();
    size = _filePathQueue.size ();
    _unlock ();
    return (size);
}

/**
 *******************************************************************************
 * @brief empty - Public method to determine if queue is empty.
 *******************************************************************************
 */
Bool_t Work_Queue::empty ()
{
    Bool_t isEmpty = 0;

    _lock ();
    isEmpty = _filePathQueue.empty ();
    _unlock ();
    return (isEmpty);
}

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */
