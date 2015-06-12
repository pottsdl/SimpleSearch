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
#include <fcntl.h>
#include <iostream>
#include <list>


/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "common_types.h"
#include "listdir.hpp"
#include "work_queue.hpp"
#include "buffer_processing.hpp"

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
void processFile(std::string filePath, int tid);

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

    {
        char mybuf[] = "!!!zzz=abc!!!555++++Doug";
        int buflen = strlen(mybuf);
        std::list<char *> word_list;
        int ret = -1;

        ret = processWholeBuffer(mybuf, buflen, word_list);
        for (std::list<char *>::iterator it=word_list.begin(); it != word_list.end(); ++it)
        {
            printf ("Found word: %s\n", *it);

        } /* end for */
    }
    exit(0);

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
        if (stat != 0)
        {
            fprintf(stderr, "ERROR: Failed to spawn thread index: %d. (%d, %s)\n",
                thread_idx, errno, strerror(errno));
        }
    }

    listdir(first_dir, fileProcessingQueue);

    // for (thread_idx = 0; thread_idx < num_worker_threads*2; thread_idx++) 
    for (thread_idx = 0; thread_idx < num_worker_threads; thread_idx++) 
    {
        fileProcessingQueue->push("EXIT");
    }
    for (thread_idx = 0; thread_idx < num_worker_threads; thread_idx++) 
    {
        printf ("Joining thread idx=%d\n", thread_idx);
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

        if (queueString != "EXIT")
        {
            printf ("[%d] Processing:%s\n", tid, queueString.c_str());
            processFile(queueString, tid);
        }
        else
        {
            printf ("[%d] Received EXIT msg\n", tid);
        }
    }
    printf ("Worker Thread #%d exitting procesing loop\n", tid);

    return(NULL);
}

void processFile(std::string filePath, int tid)
{
    char buffer[512] = { 0 };

    int fIn;
    ssize_t bytes = 0;
    ssize_t total_bytes = 0;

    //open a file
    fIn = open (filePath.c_str(), O_RDONLY);
    if (fIn == -1) {
        fprintf(stderr, "Failed to open file: %s, errno=%d,%s",
               filePath.c_str(), errno, strerror(errno));
        return;
    }

    //read from file
    while ((bytes = read (fIn, buffer, sizeof(buffer))) > 0)
    {
        total_bytes += bytes;
    }

    //and close it
    close (fIn);

    printf ("[%d] Finished processing file: %s, %lu bytes\n",
            tid, filePath.c_str(), total_bytes);

    return;
}

#if 0
int processBufferForWorks(char *buffer, int buffer_sz)
{
    int begin_last_word = -1;
    for (char_index = 0; char_index < buffer_sz; char_index++)
    {
        char *curr;
        curr = buffer[char_index];

        /* If a word character we'll want to keep going */
        if (isWordChar(curr))
        {
            if (begin_last_word == -1)
            {
                begin_last_word = char_index;
            }
        }
    }
}

Bool_t isWordChar(const char thisOne)
{
    if ((thisOne >= 'A' && thisOne <= 'Z') ||
            (thisOne >= 'a' && thisOne <= 'z') ||
            (thisOne >= '0' && thisOne <= '9'))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif
