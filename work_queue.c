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
#include "work_queue.h"

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
Work_Queue_t *createWorkQueue(int queue_length)
{
    Work_Queue_t *newQueue = NULL;
    Work_Queue_t *tmpQueue = NULL;
    int stat = 0;

    tmpQueue = (Work_Queue_t *) malloc(sizeof(Work_Queue_t));
    if (tmpQueue != NULL)
    {
        /* Wipe memory first so we know what succeeds later */
        memset (tmpQueue, 0, sizeof(Work_Queue_t));

        tmpQueue->queue_empty = TRUE;
        tmpQueue->length = queue_length;
        /*
         * tmpQueue->con_init and tmpQueue>mut_init are FALSE because of
         * memset(), no need to do it again.
         */
        stat = pthread_cond_init(&tmpQueue->con, NULL);
        EXIT_EARLY_ON_ERROR(stat);
        tmpQueue->con_init = TRUE;
        stat = pthread_mutex_init(&tmpQueue->mut, NULL);
        EXIT_EARLY_ON_ERROR(stat);
        tmpQueue->mut_init = TRUE;
    }

    /* If we succeeded, then set newQueue to return to be tmpQueue */
    newQueue = tmpQueue;

cleanup:
    return(newQueue);

error:
    /*
     * Zero out/destroy/cleanup internals of Work_Queue_t
     */
    if (tmpQueue->con_init == TRUE)
    {
        stat = pthread_cond_destroy(&tmpQueue->con);
        if (stat == 0)
        {
            memset(&tmpQueue->mut, 0, sizeof(pthread_mutex_t));
            tmpQueue->con_init = FALSE;
        }
        else
        {
            fprintf(stderr, "[%s, %d:%s] failed, stat=%d, errno=%d, %s\n",
                    __FILE__, __LINE__, __FUNCTION__, stat, errno, strerror(errno));
        }
    }
    if (tmpQueue->mut_init == TRUE)
    {
        stat = pthread_mutex_destroy(&tmpQueue->mut);
        if (stat == 0)
        {
            memset(&tmpQueue->mut, 0, sizeof(pthread_cond_t));
            tmpQueue->mut_init = FALSE;
        }
        else
        {
            fprintf(stderr, "[%s, %d:%s] failed, stat=%d, errno=%d, %s\n",
                    __FILE__, __LINE__, __FUNCTION__, stat, errno, strerror(errno));
        }
    }
    tmpQueue->length = 0;
    
    /* Now free up the space for Work_Queue_t, and NULL it out */
    free(tmpQueue);
    tmpQueue = NULL;

    goto cleanup;
}

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */


