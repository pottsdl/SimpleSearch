/**
 * @file           word_dict.cpp
 * @brief:         <description>
 * @verbatim
 *******************************************************************************
 * Author:         Douglas L. Potts
 *
 * Date:           06/10/2015, <SCR #>
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
#include <stdlib.h> /* for malloc() */
#include <string.h> /* for memset() */
#include <iostream> /* for cout() */

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
static char word1[] = "1";
static char word2[] = "3";
static char word3[] = "7";

/*******************************************************************************
 * File Scoped Variables 
 *******************************************************************************
 */

/*******************************************************************************
 ********************* E X T E R N A L  F U N C T I O N S **********************
 *******************************************************************************
 */

#if defined(TEST)
extern "C" {

    void wordDictConstruct(void)
    {
        Word_Dict *myDictionary = new Word_Dict();
        TEST_ASSERT_NOT_EQUAL(myDictionary, NULL);
        if (myDictionary->isLocked())
        {
            printf ("[%s:%d] myDictionary isLocked\n", __FILE__, __LINE__);
        }
        TEST_ASSERT_EQUAL(myDictionary->isLocked(), FALSE);

        delete myDictionary;
    }
    void wordDictAddItems(void)
    {
        int idx = 0;
        Word_Dict *myDictionary = new Word_Dict();
        if (myDictionary == NULL)
        {
            printf ("[%s:%d] myDictionary is NULL\n", __FILE__, __LINE__);
        }
        TEST_ASSERT_NOT_NULL(myDictionary);

        myDictionary->insertWord(word1, 2);
        myDictionary->insertWord(word2, 5);
        myDictionary->insertWord(word3, 7);
        printf ("myDictionary Inserted WORDS:\n");

        std::map<char*,int>::iterator it = myDictionary->getMap().begin();

        // showing contents:
        std::cout << "myDictionary contains:\n";
        for (idx = 0, it = myDictionary->getMap().begin();
                it != myDictionary->getMap().end();
                ++it, idx++)
        {

            std::cout << it->first << " => " << it->second << '\n';
            char* key = it->first;
            int val = it->second;
            switch (idx)
            {
                case 0:
                    TEST_ASSERT_EQUAL(strcmp(key, "1"), 0);
                    TEST_ASSERT_EQUAL(val,  2);
                    break;
                case 1:
                    TEST_ASSERT_EQUAL(strcmp(key, "3"), 0);
                    TEST_ASSERT_EQUAL(val, 5);
                    break;
                case 2:
                    TEST_ASSERT_EQUAL(strcmp(key, "7"), 0);
                    TEST_ASSERT_EQUAL(val, 7);
                    break;
            }
        }
        delete myDictionary;
    }
    void wordDictLock(void)
    {
        Bool_t isLocked = FALSE;
        Word_Dict *myDictionary = new Word_Dict();
        TEST_ASSERT_NOT_EQUAL(myDictionary, NULL);
        printf ("Successfully constructed myDictionary\n");

        printf ("[%s:%d] locking myDictionary\n", __FILE__,__LINE__);
        myDictionary->lock();
        printf ("checking if myDictionary is locked\n");
        isLocked = myDictionary->isLocked();
        if (isLocked)
        {
            printf ("[%s:%d] myDictionary is locked\n", __FILE__, __LINE__);
        }
        else
        {
            printf ("[%s:%d] myDictionary is UNLOCKED\n", __FILE__, __LINE__);
        }
        printf ("unlocking myDictionary\n");
        myDictionary->unlock();
        printf ("myDictionary is UNLOCKED\n");
        TEST_ASSERT_EQUAL(isLocked, TRUE);

        printf ("deleting myDictionary\n");
        delete myDictionary;
    }
    void wordDictUnlock(void)
    {
        Bool_t isLocked = FALSE;
        Word_Dict *myDictionary = new Word_Dict();
        TEST_ASSERT_NOT_EQUAL(myDictionary, NULL);

        printf ("[%s:%d] locking myDictionary\n", __FILE__,__LINE__);
        myDictionary->lock();
        isLocked = myDictionary->isLocked();
        if (isLocked)
        {
            printf ("[%s:%d] myDictionary is locked\n", __FILE__, __LINE__);
        }
        else
        {
            printf ("[%s:%d] myDictionary is UNLOCKED\n", __FILE__, __LINE__);
        }
        TEST_ASSERT_EQUAL(myDictionary->isLocked(), TRUE);

        printf ("[%s:%d] unlocking myDictionary\n", __FILE__,__LINE__);
        myDictionary->unlock();

        delete myDictionary;
    }
    void wordDictIterItems(void)
    {
        int idx = 0;
        Word_Dict *myDictionary = new Word_Dict();
        char *_word = NULL;
        int  _wordCount = 0;
        if (myDictionary == NULL)
        {
            printf ("[%s:%d] myDictionary is NULL\n", __FILE__, __LINE__);
        }
        TEST_ASSERT_NOT_NULL(myDictionary);

        myDictionary->insertWord(word1, 2);
        myDictionary->insertWord(word2, 5);
        myDictionary->insertWord(word3, 7);
        printf ("myDictionary Inserted WORDS:\n");

        std::cout << "myDictionary contains:\n";
        myDictionary->begin();
        do
        {
            std::cout << "  getting next word: ";
            myDictionary->getNextWord(&_word, &_wordCount);
            if (_word != NULL)
            {
                std::cout << *_word << " => " << _wordCount << '\n';
            }
        } while (_word != NULL);

        delete myDictionary;
    }
}
#endif /* defined(TEST) */

Word_Dict::Word_Dict(void)
{
    int stat = 0;

    _mut_init = FALSE;
    _is_locked = FALSE;

    stat = pthread_mutex_init(&_mut, NULL);
    EXIT_EARLY_ON_ERROR(stat);
    _mut_init = TRUE;
    _it = _dictionaryMap.begin();

cleanup:
    return;

error:
    goto cleanup;
}

Word_Dict::~Word_Dict(void)
{
    int stat = 0;

    printf ("[%s:%d} deleting myDictionary\n", __FILE__, __LINE__);


    _lock();
    if (!_dictionaryMap.empty())
    {
        printf ("[%s:%d} Erasing any existing map entries\n", __FILE__, __LINE__);
        _dictionaryMap.erase(_dictionaryMap.begin(), _dictionaryMap.end());
    }
    else
    {
        printf ("[%s:%d} No map entries to erase\n", __FILE__, __LINE__);
    }
    _unlock();


    printf ("[%s:%d] Destroying mutex\n", __FILE__, __LINE__);
    stat = pthread_mutex_destroy(&_mut);
    if (stat != 0)
    {
        fprintf(stderr, "[%s, %d:%s] failed, stat=%d, errno=%d, %s\n",
                __FILE__, __LINE__, __FUNCTION__, stat, errno, strerror(errno));
    }
    _mut_init = FALSE;

}

void Word_Dict::_lock(void)
{
    int stat = STATUS_SUCCESS;

    stat = pthread_mutex_lock (&_mut);
    if (stat == STATUS_SUCCESS)
    {
        _is_locked = TRUE;
    }
}

void Word_Dict::lock(void)
{
    _lock();
}

void Word_Dict::_unlock(void)
{
    int stat = STATUS_SUCCESS;

    stat = pthread_mutex_unlock (&_mut);
    if (stat == STATUS_SUCCESS)
    {
        _is_locked = FALSE;
    }
}

void Word_Dict::unlock(void)
{
    _unlock();
}

void Word_Dict::insertWord(char *word, int count)
{
    _lock();
    _dictionaryMap.insert(pair<char*,int>(word, count));
    _unlock();
}

void Word_Dict::begin(void)
{
    _lock();
    _it = _dictionaryMap.begin(); 
    _unlock();
}

void Word_Dict::end(void)
{
    _lock();
    _it = _dictionaryMap.begin(); 
    _unlock();
}

void Word_Dict::getNextWord(char **word, int *count)
{
    char *_word = NULL;
    int   _count = -1;
    int stat = STATUS_SUCCESS;

    EXIT_ON_NULL_PTR(word, stat);
    EXIT_ON_NULL_PTR(count, stat);
    _lock();
    // if (((*_it) == NULL) || (_it == _dictionaryMap.end()))
    if (_it == _dictionaryMap.end())
    {
        // fprintf(stderr, "[%s, %d:%s] failed to get next, iterator(%p) or at end\n",
                // __FILE__, __LINE__, __FUNCTION__, (void*) _it);
        fprintf(stderr, "[%s, %d:%s] failed to get next, iterator at end\n",
                __FILE__, __LINE__, __FUNCTION__);
    }
    else
    {
        _word = _it->first;
        *count = _it->second;
        _it++;
    }
cleanup:
    if (word != NULL)
    {
        *word = _word;
    }
    _unlock();
    return;
error:
    goto cleanup;

}

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */


