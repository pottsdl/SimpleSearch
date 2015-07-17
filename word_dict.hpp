#ifndef __WORD_DICT_H__
#define __WORD_DICT_H__
/**
 * @file           word_dict.hpp
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
#include <pthread.h>            /* for pthread_* calls */
#include <map>
#include <string>

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "common_types.h"

#if defined(TEST)
extern "C"
{
    void wordDictConstruct (void);
    void wordDictAddItems (void);
    void wordDictLock (void);
    void wordDictUnlock (void);
    void wordDictIterItems (void);
    void wordDictGoodFind (void);
    void wordDictGoodMiss (void);
    void wordDictGoodIncrement (void);
}
#endif                          /* defined(TEST) */

/*******************************************************************************
 * Typedefs
 *******************************************************************************
 */

/*******************************************************************************
 * Constants
 *******************************************************************************
 */

/*******************************************************************************
 * Structures
 *******************************************************************************
 */
using namespace std;

class Word_Dict
{
  public:
    Word_Dict (void);
      virtual ~ Word_Dict (void);
    void lock (void);
    void unlock (void);
    Bool_t isLocked (void)
    {
        return (this->_is_locked);
    };

    // void insertWord(char *word, int count);
    void insertWord (string word, int count);
    map < string, int >&getMap (void)
    {
        return (this->_dictionaryMap);
    };
    void begin (void);
    void end (void);
    void getNextWord (string & word, int *count);

    // Bool_t hasWord(char *word);
    Bool_t hasWord (string word);

    // void incrementWordCount(char *word);
    void incrementWordCount (string word);
    void print (void);
    void setDebug (Bool_t enabled);
    Bool_t getDebug (void);
    int getWordCount (char *word);
    void printTopX (int top_X_counts);


  private:
    Bool_t _mut_init;
    Bool_t _is_locked;
    pthread_mutex_t _mut;
    map < string, int >_dictionaryMap;
    map < string, int >::iterator _it;
    Bool_t _showDebugOutput;

    void _lock (void);
    void _unlock (void);
};

/*******************************************************************************
 * Unions
 *******************************************************************************
 */

/*******************************************************************************
 * External Function Prototypes
 *******************************************************************************
 */

/*******************************************************************************
 * Global Variables
 *******************************************************************************
 */
#if _MAIN_
#define GLOBAL_VAR_DECLARE
#else
#define GLOBAL_VAR_DECLARE extern
#endif /* _MAIN_ */

#endif /* __WORD_DICT_H__ */
