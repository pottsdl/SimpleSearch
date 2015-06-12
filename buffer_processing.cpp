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
#include "buffer_processing.hpp"

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

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
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
        char *curr;
        curr = &buffer[char_index];

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
error:
    goto cleanup;
}

int processWholeBuffer(char *buffer, int buffer_sz, std::list<char *> &word_list)
{
    int chars_processed = 0;
    char *buffer_start = buffer;
    char *word_found = NULL;
    int processed_this_round = 0;

    printf ("Buffer size: %d\n", buffer_sz);
    while (chars_processed < buffer_sz)
    {
        processed_this_round = processBufferForWords (buffer_start, (buffer_sz - chars_processed),
                &word_found);
        buffer_start += processed_this_round;
        chars_processed += processed_this_round;
        printf ("  this round: %d\n", processed_this_round);
        printf ("  total     : %d\n", chars_processed);
        if (word_found != NULL)
        {
            printf ("---->Found word: %s\n", word_found);
            word_list.push_back(word_found);
        }
    }

    return(chars_processed);
}

#if 0
int test_processWholeBuffer(char *buffer, char **result_words)
{
    int buflen = strlen(buffer);
    std::list<char *> word_list;

    int ret = processWholeBuffer(buffer, buflen, word_list);

    int list_length = word_list.size();
    char *word_ptr_ary = (char *) calloc (list_length, sizeof(char *));
    for (std::list<char *>::iterator it=word_list.begin(); it != word_list.end(); ++it)
    {
        char *word;
        int word_length = 0;

        word_length = strlen((*it)->c_str());
        // word = calloc(*it.size());
    }

    return(ret);
}
#endif
