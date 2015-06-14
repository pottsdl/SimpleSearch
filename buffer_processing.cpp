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

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#if defined(TEST)
#include "unity.h"
#endif /* defined(TEST) */
#include "buffer_processing.hpp"
#include "common_types.h"

/*******************************************************************************
 * Local Function Prototypes 
 *******************************************************************************
 */

/*******************************************************************************
 * Local Constants 
 *******************************************************************************
 */
#define DBG(X)

/*******************************************************************************
 * File Scoped Variables 
 *******************************************************************************
 */

/*******************************************************************************
 ********************* E X T E R N A L  F U N C T I O N S **********************
 *******************************************************************************
 */

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */

#if defined(TEST)
extern "C" {

    void bufferProcThreeWordsNotAtBegin(void)
    {
        char mybuf[] = "!!!zzz=abc!!!555++++Doug";
        int buflen = strlen(mybuf);
        std::list<char *> word_list;
        int ret = -1;
        int idx = 0;

        ret = processWholeBuffer(mybuf, buflen, word_list);
        printf ("  Processed: %d characters\n", ret);
        TEST_ASSERT_EQUAL(word_list.size(), 4);
        for (std::list<char *>::iterator it=word_list.begin();
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
    void bufferProcThreeWordsAtBegin(void)
    {
        char mybuf[] = "zzz=abc!!!555++++Doug!!!";
        int buflen = strlen(mybuf);
        std::list<char *> word_list;
        int ret = -1;
        int idx = 0;

        ret = processWholeBuffer(mybuf, buflen, word_list);
        printf ("  Processed: %d characters\n", ret);
        TEST_ASSERT_EQUAL(word_list.size(), 4);
        for (std::list<char *>::iterator it=word_list.begin();
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
        std::list<char *> word_list;
        int ret = -1;
        int idx = 0;

        ret = processWholeBuffer(mybuf, buflen, word_list);
        printf ("  Processed: %d characters\n", ret);
        TEST_ASSERT_EQUAL(word_list.size(), 1);
        TEST_ASSERT_EQUAL_STRING(word_list.front(), "zzz");
    }
    void bufferProcOneWordAtEnd(void)
    {
        char mybuf[] = "=!!!++++zzz";
        int buflen = strlen(mybuf);
        std::list<char *> word_list;
        int ret = -1;
        int idx = 0;

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
        std::list<char *> word_list;
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
        // TEST_ASSERT_EQUAL(word_list.size(), 2);
        for (std::list<char *>::iterator it=word_list.begin();
                it != word_list.end();
                ++it, ++idx)
        {
            printf ("Found word: %s\n", *it);
        }

    }
}
#endif /* defined(TEST) */

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
                /*
                 * Since we are one char past the last 'good char', need to back
                 * the index up by one for the copy
                 */
                // int length = (char_index - 1) - begin_last_word;
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

int processWholeBuffer(char *buffer, int buffer_sz, std::list<char *> &word_list)
{
    int chars_processed = 0;
    char *buffer_start = buffer;
    char *word_found = NULL;
    int processed_this_round = 0;

    DBG(printf ("Buffer size: %d\n", buffer_sz));
    while (chars_processed < buffer_sz)
    {
        processed_this_round = processBufferForWords (buffer_start, (buffer_sz - chars_processed),
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
