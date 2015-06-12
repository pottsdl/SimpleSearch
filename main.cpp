/**
 * @file           main.c
 * @verbatim
 *******************************************************************************
 * File:           main.c
 *
 * Description:    <description>
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
#include <sys/types.h>
#include <stdio.h>

#include <errno.h> /* for errno */
#include <stdlib.h> /* for exit() */
#include <limits.h> /* for LONG_MAX/LONG_MIN */
#include <pthread.h> /* for pthread_* calls */
#include <assert.h>  /* for assert() */
#include <string.h> /* for strerror() */

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "common_types.h"
#include "listdir.hpp"
#include "work_queue.hpp"
#include <iostream>


/*******************************************************************************
 * Local Constants 
 *******************************************************************************
 */
/* #define EXIT_FAILURE (-1) */
#define BASE_TEN (0) /* Used for strtol */


typedef struct
{
    Work_Queue *myQueue;
    int thread_idx;
} ReaderWriterArgs_t;

/*******************************************************************************
 * Local Function Prototypes 
 *******************************************************************************
 */
void *workerThread(void *arg);

/*******************************************************************************
 * File Scoped Variables 
 *******************************************************************************
 */

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

    Work_Queue *fileProcessingQueue = new Work_Queue();

    while ((opt = getopt(argc, argv, "t:")) != -1) {
        switch (opt) {
            case 't':
                tmp_long = strtol(optarg, &endptr, BASE_TEN);
                if ((errno == ERANGE && (tmp_long == LONG_MAX || tmp_long == LONG_MIN))
                        || (errno != 0 && tmp_long == 0))
                {
                    perror("strtol");
                    exit(EXIT_FAILURE);
                }

                if (endptr == optarg)
                {
                    fprintf(stderr, "No digits were found\n");
                    exit(EXIT_FAILURE);
                }
                num_worker_threads = tmp_long;
                break;
            default:
                fprintf(stderr, "Usage: %s [-t num_threads] <first_dir_path>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    /* Now optind (declared extern int by <unistd.h>) is the index of the first non-option argument. */
    /* If it is >= argc, there were no non-option arguments. */
    if (optind >= argc)
    {
        fprintf(stderr, "Usage: %s [-t num_threads] <first_dir_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    first_dir = argv[optind];

    printf ("Number of threads:  %li\n", num_worker_threads);
    printf ("Based dir:          %s\n", first_dir);

    thread_array = (pthread_t *) malloc(num_worker_threads * sizeof(pthread_t));
    args_array = (ReaderWriterArgs_t *) malloc(num_worker_threads * sizeof(ReaderWriterArgs_t));
    if (thread_array == NULL)
    {
        fprintf(stderr, "ERROR: Failed to allocate thread_array, exitting. (%d, %s)\n",
                errno, strerror(errno));
        exit(1);
    }
    if (args_array == NULL)
    {
        fprintf(stderr, "ERROR: Failed to allocate args_array, exitting. (%d, %s)\n",
                errno, strerror(errno));
        exit(1);
    }
    for (thread_idx = 0; thread_idx < num_worker_threads; thread_idx++) 
    {
        args_array[thread_idx].myQueue = fileProcessingQueue;
        args_array[thread_idx].thread_idx = thread_idx;
        stat = pthread_create(&thread_array[thread_idx], NULL, workerThread, (void*) &args_array[thread_idx]);
    }

    listdir(first_dir, fileProcessingQueue);

    // for (thread_idx = 0; thread_idx < num_worker_threads*2; thread_idx++) 
    for (thread_idx = 0; thread_idx < num_worker_threads; thread_idx++) 
    {
        fileProcessingQueue->push("EXIT");
    }
    for (thread_idx = 0; thread_idx < num_worker_threads; thread_idx++) 
    {
        printf ("Joining thread idx=%ld\n", thread_idx);
        pthread_join(thread_array[thread_idx], NULL);
    }



    return 0;
} /* main */

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */

void *workerThread(void *arg)
{
    ReaderWriterArgs_t *_arg = (ReaderWriterArgs_t *) arg;
    Work_Queue *q = _arg->myQueue;
    string queueString = "";
    int tid = _arg->thread_idx;

    printf ("Worker Thread #%d starting...\n", tid);
    sleep(1); /* To allow threads to get started */
    while (queueString != "EXIT")
    {
        if (q->empty())
        {
            printf ("[%d] Waiting for not empty...\n", tid);
            q->waitForNotEmpty();
        }
        printf ("[%d] Queue not empty, popping front\n", tid);
        queueString = q->pop_front();
        printf ("[%d] Processing:%s\n", tid, queueString.c_str());
    }
    printf ("Worker Thread #%d exitting procesing loop\n", tid);
}
