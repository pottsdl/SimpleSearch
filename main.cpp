/**
 * @file           main.c
 * @verbatim
 *******************************************************************************
 * File:           main.c
 *
 * Description:    Main program for the Super Simple File Indexer program (ssfi)
 *
 * Author:         Douglas L. Potts
 *
 * Date:           09-Jun-2015
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
#include <unistd.h>
#include <stdio.h>

#include <errno.h>              /* for errno */
#include <stdlib.h>             /* for exit() */
#include <limits.h>             /* for LONG_MAX/LONG_MIN */
#include <pthread.h>            /* for pthread_* calls */
#include <assert.h>             /* for assert() */
#include <string.h>             /* for strerror() */
#include <iostream>
#include <list>
#include <vector>


/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "common_types.h"
#include "listdir.hpp"
#include "work_queue.hpp"
#include "buffer_processing.hpp"
#include "word_dict.hpp"

/*******************************************************************************
 * Local Constants 
 *******************************************************************************
 */
#define BASE_TEN (0)            /* Used for strtol */

/*******************************************************************************
 * Local Macros
 *******************************************************************************
 */
#define DEBUG_PRINTF(...) \
    if (g_debug_output == TRUE) \
        printf(__VA_ARGS__);

/*******************************************************************************
 * Local Structs
 *******************************************************************************
 */
/**
 * Convenience structure for passing Work_Queue, Word_Dict, etc. to the worker
 * threads.
 */
typedef struct
{
    Work_Queue *myQueue;        /**< Pointer to Thread-safe signaled work queue to use */
    Word_Dict *wordDictionary;  /**< Pointer to Thread-safe word dictionary for
                                  thread */
    int thread_idx;             /**< Thread index, used in debug output to tell
                                  which thread is doing what operation */
} ReaderWriterArgs_t;

/*******************************************************************************
 * Local Function Prototypes 
 *******************************************************************************
 */
void *workerThread (void *arg);

/*******************************************************************************
 * File Scoped Variables 
 *******************************************************************************
 */

/** If set extending print output will be written to the screen */
Bool_t g_debug_output = FALSE;

/*******************************************************************************
 ********************* E X T E R N A L  F U N C T I O N S **********************
 *******************************************************************************
 */

int main (int argc, char *argv[])
{
    int opt = 1;
    char *endptr = NULL;
    long tmp_long = 1;
    long num_worker_threads = 1;
    char *first_dir = NULL;
    pthread_t *thread_array = NULL;
    ReaderWriterArgs_t *args_array = NULL;
    int stat = 0;
    int thread_idx = 0;


    Work_Queue *fileProcessingQueue = new Work_Queue ();
    Word_Dict *wordDictionary = new Word_Dict ();

    while ((opt = getopt (argc, argv, "t:v")) != -1)
    {
        switch (opt)
        {
        case 't':
            tmp_long = strtol (optarg, &endptr, BASE_TEN);
            if ((errno == ERANGE
                 && (tmp_long == LONG_MAX || tmp_long == LONG_MIN))
                || (errno != 0 && tmp_long == 0))
            {
                perror ("strtol");
                exit (EXIT_FAILURE);
            }

            if (endptr == optarg)
            {
                fprintf (stderr, "No digits were found\n");
                exit (EXIT_FAILURE);
            }
            num_worker_threads = tmp_long;
            break;
        case 'v':
            g_debug_output = TRUE;
            printf ("=========== VERBOSE DEBUG OUPUT SET ===========\n");
            break;
        default:
            fprintf (stderr, "Usage: %s [-t num_threads] <first_dir_path>\n",
                     argv[0]);
            exit (EXIT_FAILURE);
        }
    }

    /*
     * Now optind (declared extern int by <unistd.h>) is the index of the first non-option argument. 
     */
    /*
     * If it is >= argc, there were no non-option arguments. 
     */
    if (optind >= argc)
    {
        fprintf (stderr, "Usage: %s [-t num_threads] <first_dir_path>\n",
                 argv[0]);
        exit (EXIT_FAILURE);
    }
    first_dir = argv[optind];

    DEBUG_PRINTF ("Number of threads:  %li\n", num_worker_threads);
    DEBUG_PRINTF ("Based dir:          %s\n", first_dir);

    thread_array =
        (pthread_t *) malloc (num_worker_threads * sizeof (pthread_t));
    args_array =
        (ReaderWriterArgs_t *) malloc (num_worker_threads *
                                       sizeof (ReaderWriterArgs_t));
    if (thread_array == NULL)
    {
        fprintf (stderr,
                 "ERROR: Failed to allocate thread_array, exitting. (%d, %s)\n",
                 errno, strerror (errno));
        exit (EXIT_FAILURE);
    }
    if (args_array == NULL)
    {
        fprintf (stderr,
                 "ERROR: Failed to allocate args_array, exitting. (%d, %s)\n",
                 errno, strerror (errno));
        exit (EXIT_FAILURE);
    }
    for (thread_idx = 0; thread_idx < num_worker_threads; thread_idx++)
    {
        args_array[thread_idx].myQueue = fileProcessingQueue;
        args_array[thread_idx].thread_idx = thread_idx;
        args_array[thread_idx].wordDictionary = wordDictionary;
        stat =
            pthread_create (&thread_array[thread_idx], NULL, workerThread,
                            (void *) &args_array[thread_idx]);
        if (stat != 0)
        {
            fprintf (stderr,
                     "ERROR: Failed to spawn thread index: %d. (%d, %s)\n",
                     thread_idx, errno, strerror (errno));
        }
    }

    listdir (first_dir, fileProcessingQueue);

    for (thread_idx = 0; thread_idx < num_worker_threads; thread_idx++)
    {
        fileProcessingQueue->push ("EXIT");
    }
    for (thread_idx = 0; thread_idx < num_worker_threads; thread_idx++)
    {
        // printf ("Joining thread idx=%d\n", thread_idx);
        DEBUG_PRINTF ("Joining thread idx=%d\n", thread_idx);
        pthread_join (thread_array[thread_idx], NULL);
    }
    free (thread_array);
    free (args_array);


    wordDictionary->printTopX (10);

    delete fileProcessingQueue;
    delete wordDictionary;

    return 0;
}                               /* main */

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */

/**
 *******************************************************************************
 * @brief workerThread - Thread to process a single file path for word count.
 *
 * <!-- Parameters -->
 *      @param[in]      arg            Pointer to structure containing
 *                                     thread arguments.
 *
 * <!-- Returns -->
 *      None (if return type is void)
 *
 * @par Pre/Post Conditions:
 *      @pre     Work_Queue and Word_Dict have been already successfully
 *      constructed.
 *
 * @par Global Data:
 *      @li g_debug_output - Debug output flag
 *
 * @par Description:
 *      Description of what the function does from
 *      the caller's point of view.
 *
 * @par Algorithm:
 *      Using the Work_Queue, waits to be signaled that there are file paths
 *      which are read to be searched for words.  Once a path appears, thread
 *      pulls it off of the queue and starts reading and processing it.  If the
 *      path is instead the "EXIT" command, then thread gracefullys exits to be
 *      harvested by thread_join.
 *******************************************************************************
 */
void *workerThread (void *arg)
{
    ReaderWriterArgs_t *_arg = (ReaderWriterArgs_t *) arg;
    Work_Queue *q = _arg->myQueue;
    Word_Dict *dict = _arg->wordDictionary;
    string queueString = "";
    int tid = _arg->thread_idx;

    DEBUG_PRINTF ("Worker Thread #%d starting...\n", tid);
    sleep (1);                  /* To allow threads to get started */
    while (queueString != "EXIT")
    {
        if (q->empty ())
        {
            DEBUG_PRINTF ("[%d] Waiting for not empty...\n", tid);
            q->waitForNotEmpty ();
        }
        DEBUG_PRINTF ("[%d] Queue not empty, popping front\n", tid);
        queueString = q->pop_front ();

        if (queueString != "EXIT")
        {
            DEBUG_PRINTF ("[%d] Processing:%s\n", tid, queueString.c_str ());
            processFile (tid, queueString, dict);
        }
        else
        {
            DEBUG_PRINTF ("[%d] Received EXIT msg\n", tid);
        }
    }
    DEBUG_PRINTF ("Worker Thread #%d exitting procesing loop\n", tid);

    return (NULL);
}
