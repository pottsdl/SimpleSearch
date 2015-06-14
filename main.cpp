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
#include <vector>
#include <algorithm> /* for std::sort */


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
#define BASE_TEN (0) /* Used for strtol */


typedef struct
{
    Work_Queue *myQueue;
    Word_Dict  *wordDictionary;
    int thread_idx;
} ReaderWriterArgs_t;

/*******************************************************************************
 * Local Function Prototypes 
 *******************************************************************************
 */
void *workerThread(void *arg);
static void processFile(int tid, std::string filePath, Word_Dict *dict);
static void _lock_printing (void);
static void _unlock_printing (void);
static void print_read_performance (std::vector<int> &read_counts);
void printWordList(list<char*> word_list);

/*******************************************************************************
 * File Scoped Variables 
 *******************************************************************************
 */
static pthread_mutex_t g_printMutex = PTHREAD_MUTEX_INITIALIZER;
Bool_t g_debug_output = false;

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

#if 0
    char *word = NULL;
    int wordCount = -1;
#endif

    Work_Queue *fileProcessingQueue = new Work_Queue();
    Word_Dict  *wordDictionary = new Word_Dict();

    while ((opt = getopt(argc, argv, "t:v")) != -1) {
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
            case 'v':
                g_debug_output = TRUE;
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

#if 0
    {
        char mybuf[] = "!!!zzz=abc!!!555++++Doug";
        int buflen = strlen(mybuf);
        std::list<char *> word_list;
        int ret = -1;

        ret = processWholeBuffer(mybuf, buflen, word_list);
        printf ("  Processed: %d characters\n", ret);
        for (std::list<char *>::iterator it=word_list.begin(); it != word_list.end(); ++it)
        {
            printf ("Found word: %s\n", *it);

        } /* end for */
    }
    exit(0);
#endif

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
        args_array[thread_idx].wordDictionary = wordDictionary;
        stat = pthread_create(&thread_array[thread_idx], NULL, workerThread, (void*) &args_array[thread_idx]);
        if (stat != 0)
        {
            fprintf(stderr, "ERROR: Failed to spawn thread index: %d. (%d, %s)\n",
                thread_idx, errno, strerror(errno));
        }
    }

    listdir(first_dir, fileProcessingQueue);

    for (thread_idx = 0; thread_idx < num_worker_threads; thread_idx++) 
    {
        fileProcessingQueue->push("EXIT");
    }
    for (thread_idx = 0; thread_idx < num_worker_threads; thread_idx++) 
    {
        printf ("Joining thread idx=%d\n", thread_idx);
        pthread_join(thread_array[thread_idx], NULL);
    }


    // wordDictionary->print();
    wordDictionary->printTopX(10);

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
    Word_Dict *dict = _arg->wordDictionary;
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
            processFile(tid, queueString, dict);
        }
        else
        {
            printf ("[%d] Received EXIT msg\n", tid);
        }
    }
    printf ("Worker Thread #%d exitting procesing loop\n", tid);

    return(NULL);
}

bool int_compare (int i,int j)
{
    return (i<j);
}

static void processFile(int tid, std::string filePath, Word_Dict *dict)
{
    static const int INITIAL_COUNT = 1;
    char buffer[512] = { 0 };

    int fIn;
    ssize_t bytes = 0;
    ssize_t total_bytes = 0;
    list<char *> word_list;
    vector<int> read_counts;

    //open a file
    fIn = open (filePath.c_str(), O_RDONLY);
    if (fIn == -1) {
        fprintf(stderr, "Failed to open file: %s, errno=%d,%s",
               filePath.c_str(), errno, strerror(errno));
        return;
    }

    //read from file
    printf("Processing file: %s\n", filePath.c_str());
    while ((bytes = read (fIn, buffer, sizeof(buffer))) > 0)
    {
        int processed_bytes = 0;

        read_counts.push_back(bytes);
        word_list.clear();
        processed_bytes = processWholeBuffer(buffer, bytes, word_list);

        if (g_debug_output == TRUE)
        {
            printWordList(word_list);
        }

        /*
         * Foreach word:
         *     - Try and find it in the list
         *     - if in the list, increment count
         *     - otherwise add to the list with a count of 1
         */
        for (std::list<char *>::iterator it=word_list.begin();
                it != word_list.end();
                ++it)
        {
            char *word = NULL;

            word = *it;
            printf ("Finding word: %s\n", word);

            if (dict->hasWord(word) == FALSE)
            {
                printf ("[%d] Word(%s) is not in dict. adding it\n",
                        tid, *it);
                dict->insertWord(word, INITIAL_COUNT);
            }
            else
            {
                printf ("[%d] Word(%s) IS in dict. incrementing it\n",
                        tid, *it);
                dict->incrementWordCount(word);
            }
        } /* end for */
        if (g_debug_output == TRUE)
        {
            printf ("[%d] Processed %d bytes this loop\n", tid, processed_bytes);
        }

        total_bytes += bytes;
    }

    if (g_debug_output == TRUE)
    {
        print_read_performance(read_counts);
    }


    //and close it
    close (fIn);

    printf ("[%d] Finished processing file: %s, %lu bytes\n",
            tid, filePath.c_str(), total_bytes);

    return;
}

static void _lock_printing (void)
{
    (void) pthread_mutex_lock(&g_printMutex);
}
static void _unlock_printing (void)
{
    (void) pthread_mutex_unlock(&g_printMutex);
}

static void print_read_performance (std::vector<int> &read_counts)
{
    std::sort (read_counts.begin(), read_counts.end(), int_compare);
    printf ("Summarizing file read counts:\n");
    for (std::vector<int>::iterator v_it=read_counts.begin();
            v_it != read_counts.end();
            ++v_it)
    {
        printf (" %d,", *v_it);
    }
    printf ("\n");

    /* Histogram */
    /*
     * if count == last_count
     *   print *
     * else
     *   print \ncount
     *
     */
    _lock_printing();
    int last_count = -1;
    printf ("     Read Counts Histogram\n");
    printf ("     +-----------------------------------------------------");
    for (std::vector<int>::iterator v_it=read_counts.begin();
            v_it != read_counts.end();
            ++v_it)
    {

        // printf ("---->v_it=%d,  last_count=%d\n", *v_it, last_count);
        if (*v_it == last_count)
        {
            printf ("*");
        }
        else
        {
            printf ("\n%4d | *", *v_it);
            last_count = *v_it;
        }
    }
    printf ("\n     +-----------------------------------------------------\n");
    _unlock_printing();
}

void printWordList(list<char*> word_list)
{
    printf ("Processed words:  ");
    for (std::list<char *>::iterator it=word_list.begin();
            it != word_list.end();
            ++it)
    {
        printf ("%s, ", *it);
    } /* end for */
    printf ("\n");

    return;
}
