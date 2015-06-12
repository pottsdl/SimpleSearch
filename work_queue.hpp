#ifndef __WORK_QUEUE_H__
#define __WORK_QUEUE_H__
/**
 * @file           work_queue.h
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
#include <pthread.h> /* for pthread_* calls */
#include <queue>
#include <string>

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "common_types.h"

/*******************************************************************************
 * Typedefs
 *******************************************************************************
 */

/*******************************************************************************
 * Constants
 *******************************************************************************
 */

/*******************************************************************************
 * Structures
 *******************************************************************************
 */
using namespace std;

typedef struct
{
    Bool_t mut_init;
    Bool_t con_init;
    pthread_mutex_t mut;
    pthread_cond_t  con;
} Work_Queue_t;

class Work_Queue
{
    public:
        Work_Queue(void);
        virtual ~Work_Queue(void);
        void lock(void);
        void unlock(void);
        Bool_t isLocked(void) { return(_is_locked); };
        void push(string filePath);
        void pop(void);
        void waitForNotEmpty(void);
        string pop_front(void);
        string front(void);
        unsigned int size(void);
        Bool_t empty();


    private:
        Bool_t _mut_init;
        Bool_t _con_init;
        Bool_t _is_locked;
        pthread_mutex_t _mut;
        pthread_cond_t  _con;
        queue<string> _filePathQueue;
        void _lock(void);
        void _unlock(void);
        void _signal(void);
        void _wait(void);
};

/*******************************************************************************
 * Unions
 *******************************************************************************
 */

/*******************************************************************************
 * External Function Prototypes
 *******************************************************************************
 */
Work_Queue_t *createWorkQueue(int queue_length);

/*******************************************************************************
 * Global Variables
 *******************************************************************************
 */
#if _MAIN_
#define GLOBAL_VAR_DECLARE
#else
#define GLOBAL_VAR_DECLARE extern
#endif /* _MAIN_ */

#endif /* __WORK_QUEUE_H__ */

