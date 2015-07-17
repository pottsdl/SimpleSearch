/**
 * @file           word_dict.cpp
 * @brief:         <description>
 * @verbatim
 *******************************************************************************
 * Author:         Douglas L. Potts
 *
 * Date:           06/10/2015, <SCR #>
 *
 *==============================================================================
 *==============================================================================
 * Copyright (c) 2015 Douglas Lee Potts
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 *==============================================================================
 *==============================================================================
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
#include <stdlib.h>             /* for malloc() */
#include <string.h>             /* for memset() */
#include <iostream>             /* for cout() */
#include <vector>
#include <algorithm>            /* for std::sort */

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "common_types.h"
#include "error_macros.h"
#include "word_dict.hpp"
#if defined(TEST)
#include "unity.h"
#endif /* defined(TEST) */

/*******************************************************************************
 * Local Function Prototypes 
 *******************************************************************************
 */

/*******************************************************************************
 * Local Constants 
 *******************************************************************************
 */
#define DBG(X)

#if defined(TEST)
static char word1[] = "1";
static char word2[] = "3";
static char word3[] = "7";

static char APPLES[] = "apples";
static char ORANGES[] = "oranges";
static char CHERRIES[] = "cherries";
static char PEARS[] = "pears";
#endif /* defined(TEST) */

/*******************************************************************************
 * File Scoped Variables 
 *******************************************************************************
 */

/*******************************************************************************
 ********************* E X T E R N A L  F U N C T I O N S **********************
 *******************************************************************************
 */

#if defined(TEST)
extern "C"
{

    void wordDictConstruct (void)
    {
        Word_Dict *myDictionary = new Word_Dict ();
          TEST_ASSERT_NOT_EQUAL (myDictionary, NULL);
        if (myDictionary->isLocked ())
        {
            printf ("[%s:%d] myDictionary isLocked\n", __FILE__, __LINE__);
        }
        TEST_ASSERT_EQUAL (myDictionary->isLocked (), FALSE);

        delete myDictionary;
    }
    void wordDictAddItems (void)
    {
        int idx = 0;
        Word_Dict *myDictionary = new Word_Dict ();

        if (myDictionary == NULL)
        {
            printf ("[%s:%d] myDictionary is NULL\n", __FILE__, __LINE__);
        }
        TEST_ASSERT_NOT_NULL (myDictionary);

        myDictionary->insertWord (word1, 2);
        myDictionary->insertWord (word2, 5);
        myDictionary->insertWord (word3, 7);
        printf ("myDictionary Inserted WORDS:\n");

        std::map < string, int >::iterator it =
            myDictionary->getMap ().begin ();

        // showing contents:
        std::cout << "myDictionary contains:\n";
        for (idx = 0, it = myDictionary->getMap ().begin ();
             it != myDictionary->getMap ().end (); ++it, idx++)
        {

            std::cout << it->first << " => " << it->second << '\n';
            string key = it->first;
            const char *key_cstr = key.c_str ();
            int val = it->second;

            switch (idx)
            {
            case 0:
                TEST_ASSERT_EQUAL (strcmp (key_cstr, "1"), 0);
                TEST_ASSERT_EQUAL (val, 2);
                break;
            case 1:
                TEST_ASSERT_EQUAL (strcmp (key_cstr, "3"), 0);
                TEST_ASSERT_EQUAL (val, 5);
                break;
            case 2:
                TEST_ASSERT_EQUAL (strcmp (key_cstr, "7"), 0);
                TEST_ASSERT_EQUAL (val, 7);
                break;
            }
        }
        delete myDictionary;
    }
    void wordDictLock (void)
    {
        Bool_t isLocked = FALSE;
        Word_Dict *myDictionary = new Word_Dict ();

        TEST_ASSERT_NOT_EQUAL (myDictionary, NULL);
        printf ("Successfully constructed myDictionary\n");

        printf ("[%s:%d] locking myDictionary\n", __FILE__, __LINE__);
        myDictionary->lock ();
        printf ("checking if myDictionary is locked\n");
        isLocked = myDictionary->isLocked ();
        if (isLocked)
        {
            printf ("[%s:%d] myDictionary is locked\n", __FILE__, __LINE__);
        }
        else
        {
            printf ("[%s:%d] myDictionary is UNLOCKED\n", __FILE__, __LINE__);
        }
        printf ("unlocking myDictionary\n");
        myDictionary->unlock ();
        printf ("myDictionary is UNLOCKED\n");
        TEST_ASSERT_EQUAL (isLocked, TRUE);

        printf ("deleting myDictionary\n");
        delete myDictionary;
    }
    void wordDictUnlock (void)
    {
        Bool_t isLocked = FALSE;
        Word_Dict *myDictionary = new Word_Dict ();

        TEST_ASSERT_NOT_EQUAL (myDictionary, NULL);

        printf ("[%s:%d] locking myDictionary\n", __FILE__, __LINE__);
        myDictionary->lock ();
        isLocked = myDictionary->isLocked ();
        if (isLocked)
        {
            printf ("[%s:%d] myDictionary is locked\n", __FILE__, __LINE__);
        }
        else
        {
            printf ("[%s:%d] myDictionary is UNLOCKED\n", __FILE__, __LINE__);
        }
        TEST_ASSERT_EQUAL (myDictionary->isLocked (), TRUE);

        printf ("[%s:%d] unlocking myDictionary\n", __FILE__, __LINE__);
        myDictionary->unlock ();

        delete myDictionary;
    }
    void wordDictIterItems (void)
    {
        Word_Dict *myDictionary = new Word_Dict ();
        string _word;
        int _wordCount = 0;

        if (myDictionary == NULL)
        {
            printf ("[%s:%d] myDictionary is NULL\n", __FILE__, __LINE__);
        }
        TEST_ASSERT_NOT_NULL (myDictionary);

        myDictionary->insertWord (word1, 2);
        myDictionary->insertWord (word2, 5);
        myDictionary->insertWord (word3, 7);
        printf ("myDictionary Inserted WORDS:\n");

        std::cout << "myDictionary contains:\n";
        myDictionary->begin ();
        do
        {
            std::cout << "  getting next word: ";
            myDictionary->getNextWord (_word, &_wordCount);
            if (_word.empty () == false)
            {
                std::cout << _word << " => " << _wordCount << '\n';
            }
        }
        while (_word.empty () == false);

        delete myDictionary;
    }
    void wordDictGoodFind (void)
    {
        Word_Dict *myDictionary = new Word_Dict ();

        if (myDictionary == NULL)
        {
            printf ("[%s:%d] myDictionary is NULL\n", __FILE__, __LINE__);
        }
        TEST_ASSERT_NOT_NULL (myDictionary);

        myDictionary->insertWord (APPLES, 2);
        myDictionary->insertWord (ORANGES, 5);
        myDictionary->insertWord (CHERRIES, 7);

        TEST_ASSERT_TRUE (myDictionary->hasWord (APPLES));

        delete myDictionary;
    }
    void wordDictGoodMiss (void)
    {
        Word_Dict *myDictionary = new Word_Dict ();

        if (myDictionary == NULL)
        {
            printf ("[%s:%d] myDictionary is NULL\n", __FILE__, __LINE__);
        }
        TEST_ASSERT_NOT_NULL (myDictionary);

        myDictionary->insertWord (APPLES, 2);
        myDictionary->insertWord (ORANGES, 5);
        myDictionary->insertWord (CHERRIES, 7);

        TEST_ASSERT_FALSE (myDictionary->hasWord (PEARS));

        delete myDictionary;
    }
    void wordDictGoodIncrement (void)
    {
        Word_Dict *myDictionary = new Word_Dict ();

        if (myDictionary == NULL)
        {
            printf ("[%s:%d] myDictionary is NULL\n", __FILE__, __LINE__);
        }
        TEST_ASSERT_NOT_NULL (myDictionary);
        myDictionary->setDebug (TRUE);

        myDictionary->insertWord (APPLES, 2);
        myDictionary->insertWord (ORANGES, 5);
        myDictionary->insertWord (CHERRIES, 7);

        myDictionary->print ();

        if (myDictionary->hasWord (APPLES) == FALSE)
        {
            printf ("Word(%s) is not in dict. adding it\n", APPLES);
            myDictionary->insertWord (APPLES, 1);
        }
        else
        {
            printf ("Word(%s) IS in dict. INCREMENTING COUNT\n", APPLES);
            myDictionary->incrementWordCount (APPLES);
        }
        myDictionary->print ();
        TEST_ASSERT_EQUAL (myDictionary->getWordCount (APPLES), 3);

        delete myDictionary;
    }
}
#endif /* defined(TEST) */

/**
 *******************************************************************************
 * @brief Word_Dict - Constructor
 *******************************************************************************
 */
Word_Dict::Word_Dict (void)
{
    int stat = 0;

    _mut_init = FALSE;
    _is_locked = FALSE;

    stat = pthread_mutex_init (&_mut, NULL);
    EXIT_EARLY_ON_ERROR (stat);
    _mut_init = TRUE;
    _it = _dictionaryMap.begin ();
    _showDebugOutput = FALSE;

  cleanup:
    return;

  error:
    goto cleanup;
}

/**
 *******************************************************************************
 * @brief ~Word_Dict - Destructor
 *******************************************************************************
 */
Word_Dict::~Word_Dict (void)
{
    int stat = 0;

    DBG (printf ("[%s:%d] deleting myDictionary\n", __FILE__, __LINE__));


    _lock ();
    if (!_dictionaryMap.empty ())
    {
        DBG (printf
             ("[%s:%d] Erasing any existing map entries\n", __FILE__,
              __LINE__));
        _dictionaryMap.erase (_dictionaryMap.begin (), _dictionaryMap.end ());
    }
    else
    {
        printf ("[%s:%d] No map entries to erase\n", __FILE__, __LINE__);
    }
    _unlock ();


    DBG (printf ("[%s:%d] Destroying mutex\n", __FILE__, __LINE__));
    stat = pthread_mutex_destroy (&_mut);
    if (stat != 0)
    {
        fprintf (stderr, "[%s, %d:%s] failed, stat=%d, errno=%d, %s\n",
                 __FILE__, __LINE__, __FUNCTION__, stat, errno,
                 strerror (errno));
    }
    _mut_init = FALSE;

}

/**
 *******************************************************************************
 * @brief _lock - Class private lock method, for class access
 *******************************************************************************
 */
void Word_Dict::_lock (void)
{
    int stat = STATUS_SUCCESS;

    stat = pthread_mutex_lock (&_mut);
    if (stat == STATUS_SUCCESS)
    {
        _is_locked = TRUE;
    }
}

/**
 *******************************************************************************
 * @brief lock - Public lock method.
 *******************************************************************************
 */
void Word_Dict::lock (void)
{
    _lock ();
}

/**
 *******************************************************************************
 * @brief _unlock - Class private unlock method, for class access
 *******************************************************************************
 */
void Word_Dict::_unlock (void)
{
    int stat = STATUS_SUCCESS;

    stat = pthread_mutex_unlock (&_mut);
    if (stat == STATUS_SUCCESS)
    {
        _is_locked = FALSE;
    }
}

/**
 *******************************************************************************
 * @brief unlock - Public unlock method.
 *******************************************************************************
 */
void Word_Dict::unlock (void)
{
    _unlock ();
}

/**
 *******************************************************************************
 * @brief insertWord - Insert a word and word count pair into the dictionary.
 *
 * <!-- Parameters -->
 *      @param[in]      word           String representation of word for
 *                                     dictionary.
 *      @param[in]      count          Initial word count for 'word'
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
 *      Using the class access lock, insert the word and word count pair into
 *      the dictionary.
 *******************************************************************************
 */
void Word_Dict::insertWord (string word, int count)
{
    _lock ();
    _dictionaryMap.insert (pair < string, int >(word, count));

    _unlock ();
}

/**
 *******************************************************************************
 * @brief begin - Set the Word_Dict instance iterator to the beginning of the
 * dictionary.
 *******************************************************************************
 */
void Word_Dict::begin (void)
{
    _lock ();
    _it = _dictionaryMap.begin ();
    _unlock ();
}

/**
 *******************************************************************************
 * @brief end - Set the Word_Dict instance iterator to the end of the
 * dictionary.
 *******************************************************************************
 */
void Word_Dict::end (void)
{
    _lock ();
    _it = _dictionaryMap.begin ();
    _unlock ();
}

/**
 *******************************************************************************
 * @brief getNextWord - Using the in-class instance iterator, get the next word
 * and its count.
 *
 * <!-- Parameters -->
 *      @param[in,out]  word           reference to string in which to put the
 *                                     word from the dictionary.
 *      @param[in,out]  count          current word count for this 'word'
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
 *      Using the class access lock, and iterator, return the next word and word
 *      count pair.  Requires that begin() be called prior to calling, and will 
 *      return a -1 word count, and empty string if at the end of the dictionary
 *      list.
 *******************************************************************************
 */
void Word_Dict::getNextWord (string & word, int *count)
{
    int _count = -1;
    int stat = STATUS_SUCCESS;

    EXIT_ON_NULL_PTR (count, stat);
    word = "";
    _lock ();
    if (_it == _dictionaryMap.end ())
    {
        // fprintf(stderr, "[%s, %d:%s] failed to get next, iterator at end\n",
        // __FILE__, __LINE__, __FUNCTION__);
    }
    else
    {
        word = _it->first;
        _count = _it->second;
        ++_it;
    }
  cleanup:
    if (count != NULL)
    {
        *count = _count;
    }
    _unlock ();
    return;
  error:
    goto cleanup;

}

/**
 *******************************************************************************
 * @brief hasWord - Search through dictionary to determine if word is contained.
 * and its count.
 *
 * <!-- Parameters -->
 *      @param[in]      word           String representation of word
 *
 * <!-- Returns -->
 *      @return TRUE if word is already in the dictionary.
 *      @return FALSE if word is not in the dictionary.
 *
 * @par Pre/Post Conditions:
 *      None (if entry/exit conditions do not apply)
 *
 * @par Global Data:
 *      None (if no global data)
 *
 * @par Description:
 *      Using the class access lock, search for the word, and if already present,
 *      return TRUE.
 *******************************************************************************
 */
Bool_t Word_Dict::hasWord (string word)
{
    std::map < string, int >::iterator it;
    Bool_t found = FALSE;
    string wordFound;


    _lock ();
    it = _dictionaryMap.find (word);
    _unlock ();
    if ((it != _dictionaryMap.end ()))
    {
        int count = -1;

        found = TRUE;
        wordFound = it->first;
        count = it->second;
        if (_showDebugOutput == TRUE)
        {
            printf ("%s => %d\n", wordFound.c_str (), count);
        }
    }

    return (found);
}

/**
 *******************************************************************************
 * @brief incrementWordCount - Search through dictionary to determine if word is
 * contained, and if is present increment its word count.
 *
 * <!-- Parameters -->
 *      @param[in]      word           String representation of word
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
 *      Using the class access lock, search for the word, and if already present,
 *      increment its word count.
 *******************************************************************************
 */
void Word_Dict::incrementWordCount (string word)
{
    std::map < string, int >::iterator it;

    // char *wordFound = NULL;
    // int count = -1;

    _lock ();

    it = _dictionaryMap.find (word);
    if (it != _dictionaryMap.end ())
    {
        it->second++;
        // wordFound = it->first;
        // count = it->second;
        // count++;
        // _dictionaryMap.erase(it);
        // _dictionaryMap.insert(pair<string,int>(wordFound, count));
    }

    _unlock ();

    return;
}

/**
 *******************************************************************************
 * @brief print - Iterate through dictionary item list, and print the entries
 * out.
 *******************************************************************************
 */
void Word_Dict::print (void)
{
    string word;
    int wordCount = -1;

    printf ("Dumping word dictionary: =================================\n");
    begin ();
    do
    {
        getNextWord (word, &wordCount);
        if (word != "")
        {
            std::cout << word << " => " << wordCount << '\n';
        }
    }
    while (word != "");
    printf ("==========================================================\n");

    return;
}

/**
 *******************************************************************************
 * @brief setDebug - Set current value of instance debug flag.
 *******************************************************************************
 */
void Word_Dict::setDebug (Bool_t enabled)
{
    _lock ();
    _showDebugOutput = enabled;
    _unlock ();
}

/**
 *******************************************************************************
 * @brief getDebug - Get current value of instance debug flag.
 *******************************************************************************
 */
Bool_t Word_Dict::getDebug (void)
{
    Bool_t isEnabled = FALSE;

    _lock ();
    isEnabled = _showDebugOutput;
    _unlock ();
    return (isEnabled);
}

int Word_Dict::getWordCount (char *word)
{
    int _word_count = -1;

    _lock ();
    std::map < string, int >::iterator it = _dictionaryMap.find (word);

    if (it != _dictionaryMap.end ())
    {
        _word_count = it->second;
    }
    _unlock ();

    return (_word_count);
}

#if 0
/**
 *******************************************************************************
 * @brief wordCmp - Comparitor for sort
 *******************************************************************************
 */
/*
 * If we were sorting in least count to most count, we'd use this 
 */
bool wordCmp (const pair < string, int >&left_count, const pair < string,
              int >&right_count)
{
    return (left_count.second < right_count.second);
}
#endif

/**
 *******************************************************************************
 * @brief wordRevCmp - Comparitor for sort, reversed ordering
 *******************************************************************************
 */
bool wordRevCmp (const pair < string, int >&left_count, const pair < string,
                 int >&right_count)
{
    return (left_count.second > right_count.second);
}


/**
 *******************************************************************************
 * @brief printTopX - Go through all of the dictionary entries, and print out
 * the top 'top_X_counts' counts.
 *******************************************************************************
 */
void Word_Dict::printTopX (int top_X_counts)
{
    vector < pair < string, int >>top_list;
    string word;
    int wordCount = -1;
    int top_count = top_X_counts;

    /*
     * Put dictionary entries into vector for sorting 
     */
    begin ();
    do
    {
        getNextWord (word, &wordCount);
        if (word.empty () == false)
        {
            top_list.push_back (pair < string, int >(word, wordCount));
        }
    }
    while (word != "");

    /*
     * Special cue to print them all, as sorted 
     */
    if (top_count == -1)
    {
        top_count = top_list.size ();
    }


    // sort (top_list.begin(), top_list.end(), wordCmp);
    sort (top_list.begin (), top_list.end (), wordRevCmp);

    int idx = 0;

    for (vector < pair < string, int > >::iterator it = top_list.begin ();
         ((it != top_list.end ()) && (idx < top_count)); ++it, idx++)

    {
        // printf ("[%d]  %s => %d\n", idx + 1, it->first.c_str(), it->second);
        printf ("%s\t%d\n", it->first.c_str (), it->second);
    }                           /* end for */
}

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */
