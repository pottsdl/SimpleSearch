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
#if 0
#include <string.h> /* for memset() */
#endif

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "common_types.h"
#include "listdir.h"
#include "work_queue.hpp"
#include <iostream>


/*******************************************************************************
 * Local Constants 
 *******************************************************************************
 */
/* #define EXIT_FAILURE (-1) */
#define BASE_TEN (0) /* Used for strtol */


/*******************************************************************************
 * Local Function Prototypes 
 *******************************************************************************
 */
void *print_message_function(void *ptr);
void *ChildThread1(void *arg);
void *ChildThread2(void *arg);
extern "C" {
extern void callListTest2(void);
}

/*******************************************************************************
 * File Scoped Variables 
 *******************************************************************************
 */
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  con = PTHREAD_COND_INITIALIZER;

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
    long thread_idx = 0;

    queue<char*> textFileQueue;

    pthread_t *thread_array = NULL;
    int iret1;

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

    listdir(first_dir, textFileQueue);
    cout << "Dumping file list queue:\n";
    cout << "========================\n";
    while (!textFileQueue.empty())
    {
        char *filenameToFree = NULL;
        cout << "  " << textFileQueue.front() << endl;
        filenameToFree = textFileQueue.front();
        textFileQueue.pop();
        free(filenameToFree);
    }
    std::cout << '\n';
    /* callListTest2(); */


    return 0;
} /* main */

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */

void *print_message_function(void *ptr)
{
    /* char *message; */
    /* message = (char *) ptr; */
    /* printf("%s \n", message); */

    long thread_idx = (long) ptr;
    printf ("Thread %ld\n", thread_idx);

    return(NULL);
}

void *ChildThread1(void *arg)
{
    printf ("In ChildThread1\n"); fflush(stdout);
    sleep(1);
    printf (" ChildThread1 after sleep 1\n"); fflush(stdout);
    pthread_mutex_lock (&mut);
    printf("mutext locked for wait\n");

    printf("In wait\n");
    pthread_cond_wait(&con, &mut);  /* wait for the signal with con as condition variable */
    printf ("Out of wait\n");

    pthread_mutex_unlock (&mut);
    printf("mutex released\n");
    printf("chil1 exit\n");

    return(NULL);
}
 
void *ChildThread2(void *arg)
{
 
    printf ("In ChildThread2\n"); fflush(stdout);
    sleep(2);
    printf (" ChildThread2 after sleep 1\n"); fflush(stdout);
    pthread_mutex_lock (&mut);
    printf("mutex locked for signal\n");
    pthread_cond_signal(&con);  /*wake up waiting thread with condition variable */
    pthread_mutex_unlock (&mut);

    printf("mutex released\n");
    printf("chil2 exit\n");

    return(NULL);
}
