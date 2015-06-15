/**
 * @file           buffer_processing.cpp
 * @brief:         <description>
 * @verbatim
 *******************************************************************************
 * Author:         Douglas L. Potts
 *
 * Date:           06/12/2015, <SCR #>
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
#include <stdio.h> /* for printf() */
#include <stdlib.h> /* for malloc() */
#include <string.h> /* for strncpy() */
#include <list> /* for std::list */
#include <vector>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h> /* for errno */
#include <algorithm> /* for std::sort */

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#if defined(TEST)
#include "unity.h"
#endif /* defined(TEST) */
#include "buffer_processing.hpp"
#include "common_types.h"

using namespace std;

/*******************************************************************************
 * External Variables
 *******************************************************************************
 */
#if defined(TEST)
/* Need this for Unit Test code, no main */
Bool_t g_debug_output = FALSE;
#else
extern Bool_t g_debug_output;
#endif

/*******************************************************************************
 * Local Function Prototypes 
 *******************************************************************************
 */
static void print_read_performance (vector<int> &read_counts);
static bool int_compare (int i,int j);
static void _lock_printing (void);
static void _unlock_printing (void);

/*******************************************************************************
 * Local Constants 
 *******************************************************************************
 */

/** @def Used to turn on and off debug print statements. */
#define DBG(X)

/*******************************************************************************
 * File Scoped Variables 
 *******************************************************************************
 */

/**
 * File scoped mutex, to keep multi-threaded printing from interleaving and
 * becoming unreadable.
 */
static pthread_mutex_t g_printMutex = PTHREAD_MUTEX_INITIALIZER;

/*******************************************************************************
 ********************* E X T E R N A L  F U N C T I O N S **********************
 *******************************************************************************
 */

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */

#if defined(TEST)

/** File scope, pointer to mocked file data, loaded by mock_set_file_data */
static char *g_mock_file_data = NULL;
/**
 * File scope, pointer to where in the mocked file data we currently are, meant
 * to mimic file pointer.
 */
static char *g_mock_file_ptr = NULL;
/** File scope, pointer to the end of the mock file data (EOF) */
static char *g_mock_file_end = NULL;

/**
 *******************************************************************************
 * @brief mock_set_file_data - Load the mock file data into the file buffer.
 *
 * <!-- Parameters -->
 *      @param[in]      data_buff      Pointer to character buffer to copy into
 *                                     the file buffer.
 *      @param[in]      count          Count of the number of bytes pointed to
 *                                     by data_buff.
 *
 * <!-- Returns -->
 *      None (if return type is void)
 *
 * @par Pre/Post Conditions:
 *      @post     Allocates a buffer on the heap containing a copy of the data
 *      from data_buff.
 *
 * @par Global Data:
 *      @li g_mock_file_data
 *      @li g_mock_file_ptr
 *      @li g_mock_file_end
 *
 * @par Description:
 *      NOT thread safe, mean to only be called by the single threaded unit
 *      tests, to provide for a way to process "files" without having to read
 *      from the file system.
 *******************************************************************************
 */
void mock_set_file_data(char *data_buff, ssize_t count)
{
    /* Create buffer to hold file data */
    g_mock_file_data = (char *) malloc(count * sizeof(char));
    /* Copy in from source */
    memcpy(g_mock_file_data, data_buff, count);
    /* Set file ptr. to the beginning of the buffer */
    g_mock_file_ptr = g_mock_file_data;
    g_mock_file_end = g_mock_file_data + count;
}

/**
 *******************************************************************************
 * @brief mock_open - Essentially a no-operation function.
 *
 * <!-- Parameters -->
 *      @param[in]      pathname      String path name to "open" (not used)
 *      @param[in]      flags         Integer bitmask (not used)
 *
 * <!-- Returns -->
 *      @return 0 - ALWAYS
 *
 * @par Pre/Post Conditions:
 *      None (if entry/exit conditions do not apply)
 *
 * @par Global Data:
 *      None (if no global data)
 *
 * @par Description:
 *      Has the same signature as the system open() call.  Doesn't actually do
 *      anything, as the buffer gets loaded elsewhere, and pointers, etc. are
 *      set there.
 *******************************************************************************
 */
int mock_open(const char *pathname, int flags)
{
    return 0;
}
/**
 *******************************************************************************
 * @brief mock_read - Read a certain number of characters out of the mock file.
 *
 * <!-- Parameters -->
 *      @param[in]      fd            File descriptor (not used)
 *      @param[in,out]  buf           Pointer to a character buffer to put the
 *                                    read "file data" into.
 *      @paran[in]      count         Number of bytes to read from the buffer,
 *                                    should be <= size of 'buf'
 *
 * <!-- Returns -->
 *      @return number of bytes actually copied into 'buf'
 *
 * @par Pre/Post Conditions:
 *      None (if entry/exit conditions do not apply)
 *
 * @par Global Data:
 *      @li g_mock_file_data
 *      @li g_mock_file_ptr
 *      @li g_mock_file_end
 *
 * @par Description:
 *      Has the same signature as the system read() call.  Reads data from our
 *      "mocked file", keeping track of the number of bytes copied into the
 *      buffer and moving the virtual file pointer accordingly.  If the end of
 *      the "file data" is reached, then actual bytes read is truncated to that.
 *******************************************************************************
 */
ssize_t mock_read(int fd, void *buf, size_t count)
{
    ssize_t bytes_used = 0;

    if ((g_mock_file_ptr + count) >= g_mock_file_end)
    {
        bytes_used = g_mock_file_end - g_mock_file_ptr;
    }
    else
    {
        bytes_used = count;
    }

    if (bytes_used > 0)
    {
        /* copy from current file ptr. for 'count' bytes */
        memcpy(buf, g_mock_file_ptr, bytes_used);
        /* Now update the file pointer */
        g_mock_file_ptr += bytes_used;
    }

    return(bytes_used);
}

/**
 *******************************************************************************
 * @brief mock_close - Close the mock file.
 *
 * <!-- Parameters -->
 *      @param[in]      fd            File descriptor (not used)
 *
 * <!-- Returns -->
 *      @return 0 - ALWAYS
 *
 * @par Pre/Post Conditions:
 *      None (if entry/exit conditions do not apply)
 *
 * @par Global Data:
 *      @li g_mock_file_data
 *      @li g_mock_file_ptr
 *      @li g_mock_file_end
 *
 * @par Description:
 *      Has the same signature as the system close() call.  Cleans up usage of
 *      the mock file buffer, resetting the pointers and freeing the mock data
 *      buffer.
 *******************************************************************************
 */
int mock_close(int fd)
{
    /* Free up the bufrer */
    free(g_mock_file_data);

    /* Reset file-globals to NULL */
    g_mock_file_data = NULL;
    g_mock_file_ptr = g_mock_file_data;
    g_mock_file_end = g_mock_file_data;

    return(0);
}

/**
 *******************************************************************************
 * @brief mock_lseek - Seek in the mock file.
 *
 * <!-- Parameters -->
 *      @param[in]      fd            File descriptor (not used)
 *      @param[in]      offset        Byte count to seek into the file.
 *      @param[in]      whence        Code indicating how 'offset' applies
 *                                    (not used).
 *
 * <!-- Returns -->
 *      @return 0 - ALWAYS
 *
 * @par Pre/Post Conditions:
 *      None (if entry/exit conditions do not apply)
 *
 * @par Global Data:
 *      @li g_mock_file_data
 *      @li g_mock_file_ptr
 *      @li g_mock_file_end
 *
 * @par Description:
 *      Has the same signature as the system close() call.  Cleans up usage of
 *      the mock file buffer, resetting the pointers and freeing the mock data
 *      buffer.
 *******************************************************************************
 */
off_t mock_lseek(int fd, off_t offset, int whence)
{
    g_mock_file_ptr = g_mock_file_data + offset;
    return((off_t) g_mock_file_ptr);
}

/*
 * When we are in #if defined(TEST), these will replace the system calls in this
 * file
 */
#define close mock_close
#define open mock_open
#define read mock_read
#define close mock_close
#define lseek mock_lseek

extern "C" {

    void bufferProcFourWordsAtBegin(void)
    {
        char mybuf[] = "zzz=abc!!!555++++Doug!!!";
        int buflen = strlen(mybuf);
        list<char *> word_list;
        int ret = -1;
        int idx = 0;

        ret = processWholeBuffer(mybuf, buflen, word_list);
        printf ("  Processed: %d characters\n", ret);
        TEST_ASSERT_EQUAL(word_list.size(), 4);
        for (list<char *>::iterator it=word_list.begin();
                it != word_list.end();
                ++it, ++idx)
        {
            printf ("Found word: %s\n", *it);
            switch (idx)
            {
                case 0: TEST_ASSERT_EQUAL_STRING(*it, "zzz"); break;
                case 1: TEST_ASSERT_EQUAL_STRING(*it, "abc"); break;
                case 2: TEST_ASSERT_EQUAL_STRING(*it, "555"); break;
                case 3: TEST_ASSERT_EQUAL_STRING(*it, "Doug"); break;
            }

        } /* end for */
    }
    void bufferProcOneWordAtBegin(void)
    {
        char mybuf[] = "zzz=!!!++++";
        int buflen = strlen(mybuf);
        list<char *> word_list;
        int ret = -1;

        ret = processWholeBuffer(mybuf, buflen, word_list);
        printf ("  Processed: %d characters\n", ret);
        TEST_ASSERT_EQUAL(word_list.size(), 1);
        TEST_ASSERT_EQUAL_STRING(word_list.front(), "zzz");
    }
    void bufferProcFullBuffer(void)
    {
#define MYBUF_LEN  512
        char mybuf[MYBUF_LEN] = { 0 };
        int buflen = MYBUF_LEN;
        list<char *> word_list;
        int ret = -1;
        int idx = 0;
        static const char * const BEG_STRING = "ginning"; /*   +------ ... -----+    */
                                                          /* Be|ginning!!!!!!fin|ish */
        static const char * const END_STRING = "fin";     /*   +------ ... -----+    */

        memset(mybuf, ':', (MYBUF_LEN - 1) * sizeof(*mybuf));
        strncpy(mybuf, BEG_STRING, strlen(BEG_STRING));
        strncpy(&mybuf[MYBUF_LEN - strlen(END_STRING)], END_STRING, strlen(END_STRING));

        ret = processWholeBuffer(mybuf, buflen, word_list);
        printf ("  Processed: %d characters\n", ret);
        TEST_ASSERT_EQUAL(word_list.size(), 1);
        for (list<char *>::iterator it=word_list.begin();
                it != word_list.end();
                ++it, ++idx)
        {
            switch (idx)
            {
                case 0:
                    TEST_ASSERT_EQUAL_STRING(*it, "ginning");
                    break;
                default:
                    TEST_FAIL_MESSAGE("Unexpected Word FOUND");
                    break;
            }
        }

    }

    void fileProcess(void)
    {
        static const int my_buf_len = 520;
        int tid = 1; /* Fake thread id */
        string fakeFilePath = "/usr/local";
        Word_Dict *testDict = new Word_Dict();
        char fileData[my_buf_len];

        /* Init fake file data to ALL non-word char's */
        memset(fileData, ':', sizeof(fileData));
        strncpy(fileData, "abc", strlen("abc"));
        strncpy(&fileData[MYBUF_LEN - strlen("xyz")], "xyz", strlen("xyz"));

        mock_set_file_data(fileData, my_buf_len);

        processFile(tid, fakeFilePath, testDict);

        testDict->print();
        int dict_entry_count = 0;
        string word;
        int wordCount;
        testDict->begin();
        do
        {
            testDict->getNextWord(word, &wordCount);
            if (word != "")
            {
                switch (dict_entry_count)
                {
                    case 0: TEST_ASSERT_EQUAL_STRING(word.c_str(), "abc"); break;
                    case 1: TEST_ASSERT_EQUAL_STRING(word.c_str(), "xyz"); break;
                }
                dict_entry_count++;
            }
        } while (word != "");
        TEST_ASSERT_EQUAL(dict_entry_count, 2);
    }
}
#endif /* defined(TEST) */

/**
 *******************************************************************************
 * @brief isWordChar - Check if the given character is considered a "word" char.
 *
 * <!-- Parameters -->
 *      @param[in]      thisOne        Character code to check if it is a "word"
 *                                     char.
 *
 * <!-- Returns -->
 *      @return TRUE    If thisOne is a "word" character
 *      @return FALSE   If thisOne is NOT a "word" character
 *
 * @par Pre/Post Conditions:
 *      None (if entry/exit conditions do not apply)
 *
 * @par Global Data:
 *      None (if no global data)
 *
 * @par Description:
 *      Check if the character falls between a-z, A-Z, or 0-9.
 *******************************************************************************
 */
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

/**
 *******************************************************************************
 * @brief processFile - Take a file path, and parse the file for words putting
 * them in the dictionary.
 *
 * <!-- Parameters -->
 *      @param[in]      tid            Integer thread index, only used in debug
 *                                     output to track which thread is
 *                                     performing what operation.
 *      @param[in]      filePath       String file path to a ".txt" file
 *      @param[in]      dict           Pointer to a Word_Dict which any words
 *                                     processed will be kept.
 *
 * <!-- Returns -->
 *      None (if return type is void)
 *
 * @par Pre/Post Conditions:
 *      None (if entry/exit conditions do not apply)
 *
 * @par Global Data:
 *      None (if no global data)
 *
 * @par Description:
 *      Open the file specified by filePath, and read through it, searching for
 *      words (based on "word char" qualification), and updating the dictionary
 *      for each.  Updating consists of inserting if the word doesn't already
 *      exist, and updating the count for that word if it does.
 *******************************************************************************
 */
void processFile(int tid, string filePath, Word_Dict *dict)
{
    static const int INITIAL_COUNT = 1;
    char buffer[512] = { 0 };

    int fIn;
    ssize_t bytes = 0;
    ssize_t total_bytes = 0;
    list<char *> word_list;
    vector<int> read_counts;
    int processed_bytes = 0;
    int leftover_bytes = 0;

    fIn = open (filePath.c_str(), O_RDONLY);
    if (fIn == -1) {
        fprintf(stderr, "Failed to open file: %s, errno=%d,%s",
               filePath.c_str(), errno, strerror(errno));
        return;
    }

    DBG(printf("Processing file: %s\n", filePath.c_str()));
    Bool_t already_rewound = FALSE;
    while ((bytes = read (fIn, buffer, sizeof(buffer))) > 0)
    {

        read_counts.push_back(bytes);
        word_list.clear();
        processed_bytes = processWholeBuffer(buffer, bytes, word_list);


        DBG(printWordList(word_list));

        /*
         * Foreach word:
         *     - Try and find it in the list
         *     - if in the list, increment count
         *     - otherwise add to the list with a count of 1
         */
        for (list<char *>::iterator it=word_list.begin();
                it != word_list.end();
                ++it)
        {
            // char *word = NULL;
            string word;

            word = *it;
            DBG(printf ("Finding word: %s\n", word.c_str()));

            if (dict->hasWord(word) == FALSE)
            {
                DBG(printf ("[%d] Word(%s) is not in dict. adding it\n",
                        tid, *it));
                dict->insertWord(word, INITIAL_COUNT);
            }
            else
            {
                DBG(printf ("[%d] Word(%s) IS in dict. incrementing it\n",
                        tid, *it));
                dict->incrementWordCount(word);
            }
            free(*it);
        } /* end for */
        DBG(printf ("[%d] Processed %d bytes this loop\n", tid, processed_bytes));

        /*
         * If we didn't process all of it, then only count what we did process
         */
        if (processed_bytes < bytes)
        {
            total_bytes += processed_bytes;
            leftover_bytes = bytes - processed_bytes;
            if (already_rewound == FALSE)
            {
                lseek(fIn, total_bytes, SEEK_SET);
                already_rewound = TRUE;
            }
        }
        else
        {
            total_bytes += bytes;
        }
    }
    /* Process anything left in the buffer, at end of file */
    if ((bytes == 0) && (leftover_bytes > 0))
    {
        int chars_processed = 0;
        char *buffer_start = buffer;
        int buffer_sz_to_process = leftover_bytes;
        char *word_found = NULL;
        int processed_this_round = 0;

        while (chars_processed < buffer_sz_to_process)
        {
            processed_this_round = processBufferForWords (buffer_start,
                    buffer_sz_to_process,
                    &word_found);
            buffer_start += processed_this_round;
            chars_processed += processed_this_round;
            DBG(printf ("  %d: bytes proceesed this loop\n", processed_this_round));
            DBG(printf ("  %d: total bytes processed\n", chars_processed));
            if (word_found != NULL)
            {
                DBG(printf ("---->Found word: %s\n", word_found));
                word_list.push_back(word_found);
            }
            DBG(printf ("\n\n"));
        }
    }

    if (g_debug_output == TRUE)
    {
        print_read_performance(read_counts);
    }


    close (fIn);

    DBG(printf ("[%d] Finished processing file: %s, %lu bytes\n",
            tid, filePath.c_str(), total_bytes));

    return;
}

/**
 *******************************************************************************
 * @brief processBufferForWords - Take a buffer of data read from the file and
 * parse for words.
 *
 * <!-- Parameters -->
 *      @param[in]      buffer         Pointer to the buffer read from file to
 *                                     process.
 *      @param[in]      buffer_sz      Size of 'buffer' in characters
 *      @param[in]      word           Pointer to location in which to put the
 *                                     word found.
 *
 * <!-- Returns -->
 *      @return number of bytes processed.
 *
 * @par Pre/Post Conditions:
 *      None (if entry/exit conditions do not apply)
 *
 * @par Global Data:
 *      None (if no global data)
 *
 * @par Description:
 *      Search through the buffer character by character to find "runs" of "word
 *      characters.  Once a run/word is found it, it is returned to the caller
 *      by setting 'word' to point to it.
 *******************************************************************************
 */
int processBufferForWords(char *buffer, int buffer_sz, char **word)
{
    int char_index;
    int begin_last_word = -1;
    int num_processed = 0;

    if (word == NULL)
    {
        goto cleanup;
    }
    *word = NULL;
    for (char_index = 0; char_index < buffer_sz; char_index++)
    {
        /* If a word character we'll want to keep going */
        if (isWordChar(buffer[char_index]))
        {
            if (begin_last_word == -1)
            {
                begin_last_word = char_index;
            }
        }
        else
        {
            if (begin_last_word != -1)
            {
                int length = char_index - begin_last_word;
                char *tmp_word = (char *) calloc(length, sizeof(char));

                if (tmp_word != NULL)
                {
                    strncpy(tmp_word, &buffer[begin_last_word], length);
                    *word = tmp_word;
                    goto cleanup;
                }
            }
        }
        num_processed++;
    }

    if (begin_last_word != -1)
    {
        /* Buffer size minus where we started should be the length */
        int length = buffer_sz - begin_last_word;
        char *tmp_word = (char *) calloc(length, sizeof(char));

        if (tmp_word != NULL)
        {
            strncpy(tmp_word, &buffer[begin_last_word], length);
            *word = tmp_word;
        }

    }

cleanup:
    return(num_processed);
}

/**
 *******************************************************************************
 * @brief processWholeBuffer - Process a whole buffer's worth of data for all of
 * the words contained therein.
 *
 * <!-- Parameters -->
 *      @param[in]      buffer         Pointer to the buffer read from file to
 *                                     process.
 *      @param[in]      buffer_sz      Size of 'buffer' in characters
 *      @param[in]      word_list      Reference to a stl::list in which to
 *                                     insert all words found in the buffer.
 *
 * <!-- Returns -->
 *      @return count of bytes processed from the buffer.
 *
 * @par Pre/Post Conditions:
 *      None (if entry/exit conditions do not apply)
 *
 * @par Global Data:
 *      None (if no global data)
 *
 * @par Description:
 *      Takes a file read buffer's worth of data, passes it off to
 *      processBufferForWords() to get individual words from the buffer, and add
 *      them to the word_list vector (they will get coallated with the other
 *      words in the caller.
 *******************************************************************************
 */
int processWholeBuffer(char *buffer, int buffer_sz, list<char *> &word_list)
{
    int chars_processed = 0;
    char *buffer_start = buffer;
    char *word_found = NULL;
    int processed_this_round = 0;
    int buffer_sz_to_process = buffer_sz;
    int idx = 0;

    DBG(printf ("Buffer size: %d\n", buffer_sz));
    /*
     * If very last of buffer looks like a word, back off until we are at the
     * beginning of it, and then only process up until that point
     */
    if (isWordChar(buffer[buffer_sz - 1]))
    {
        for (idx = buffer_sz - 1; idx >= 0; idx--) 
        {
            if (isWordChar(buffer[idx]) == FALSE)
            {
                buffer_sz_to_process = idx;
                break;
            }
        } /* end for */
    }
    while (chars_processed < buffer_sz_to_process)
    {
        processed_this_round = processBufferForWords (buffer_start,
                (buffer_sz_to_process - chars_processed),
                &word_found);
        buffer_start += processed_this_round;
        chars_processed += processed_this_round;
        DBG(printf ("  %d: bytes proceesed this loop\n", processed_this_round));
        DBG(printf ("  %d: total bytes processed\n", chars_processed));
        if (word_found != NULL)
        {
            DBG(printf ("---->Found word: %s\n", word_found));
            word_list.push_back(word_found);
        }
        DBG(printf ("\n\n"));
    }

    return(chars_processed);
}

/**
 *******************************************************************************
 * @brief int_compare - comparitor used by vector sort operation.
 *******************************************************************************
 */
static bool int_compare (int i,int j)
{
    return (i<j);
}

/**
 *******************************************************************************
 * @brief print_read_performance - Function to go through the file read byte
 * counts and plot the histogram.
 *
 * <!-- Parameters -->
 *      @param[in]      read_counts    Vector of all of the read operations on a
 *                                     particular file thread.
 *
 * <!-- Returns -->
 *      None (if return type is void)
 *
 * @par Pre/Post Conditions:
 *      None (if entry/exit conditions do not apply)
 *
 * @par Global Data:
 *      None (if no global data)
 *
 * @par Description:
 *      For each file read operation, the size of the buffer returned is stored
 *      in a vector, and then at file close, this function is called to plot the
 *      read peformance.
 *
 *******************************************************************************
 */
static void print_read_performance (vector<int> &read_counts)
{
    sort (read_counts.begin(), read_counts.end(), int_compare);
    printf ("Summarizing file read counts:\n");
    for (vector<int>::iterator v_it=read_counts.begin();
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
    for (vector<int>::iterator v_it=read_counts.begin();
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

/**
 *******************************************************************************
 * @brief _lock_printing - Grab the local file mutex for printing debug output.
 *******************************************************************************
 */
static void _lock_printing (void)
{
    (void) pthread_mutex_lock(&g_printMutex);
}

/**
 *******************************************************************************
 * @brief _unlock_printing - Release the local file mutex for printing debug
 * output.
 *******************************************************************************
 */
static void _unlock_printing (void)
{
    (void) pthread_mutex_unlock(&g_printMutex);
}

