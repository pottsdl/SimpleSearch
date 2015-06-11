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

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "listdir.h"


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
    long thread_idx = 0;

    /* pthread_t thread1, thread2; */
    pthread_t *thread_array = NULL;
    /* const char *message1 = "Thread 1"; */
    /* const char *message2 = "Thread 2"; */
    /* int iret1, iret2; */
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

    listdir(first_dir);

    thread_array = malloc(num_worker_threads * sizeof(pthread_t));
    assert(thread_array != NULL);
    for (thread_idx = 0; thread_idx < num_worker_threads; thread_idx++) 
    {
        iret1 = pthread_create(&thread_array[thread_idx], NULL, print_message_function, (void*) thread_idx);
        if (iret1)
        {
            fprintf(stderr,"Error - pthread_create() for idx=%ld return code: %d\n", thread_idx, iret1);
            exit(EXIT_FAILURE);
        }
        printf("pthread_create() for thread %ld returns: %d\n", thread_idx, iret1);
    } /* end for */
    printf ("All %ld thread(s) created, now going into join...\n", num_worker_threads);
    for (thread_idx = 0; thread_idx < num_worker_threads; thread_idx++) 
    {
        printf ("Joining thread idx=%ld\n", thread_idx);
        pthread_join(thread_array[thread_idx], NULL);
    }
    free (thread_array);
    thread_array = NULL;
#if 0
    /* Create independent threads each of which will execute function */
    iret1 = pthread_create(&thread1, NULL, print_message_function, (void*) message1);
    if (iret1)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
        exit(EXIT_FAILURE);
    }

    iret2 = pthread_create(&thread2, NULL, print_message_function, (void*) message2);
    if (iret2)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",iret2);
        exit(EXIT_FAILURE);
    }
    printf("pthread_create() for thread 1 returns: %d\n",iret1);
    printf("pthread_create() for thread 2 returns: %d\n",iret2);
    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */
    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);
#endif




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

    long thread_idx = (long *) ptr;
    printf ("Thread %ld\n", thread_idx);

    return(NULL);
}

