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
#include <pthread.h> /* for pthread_* calls */
#include <queue>
#include <string>

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "common_types.h"

extern "C" {
    void wordDictConstruct(void);
}

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
        Word_Dict(void);
        virtual ~Word_Dict(void);
        void lock(void);
        void unlock(void);
        Bool_t isLocked(void) { return(_is_locked); };


    private:
        Bool_t _mut_init;
        Bool_t _is_locked;
        pthread_mutex_t _mut;
        queue<string> _filePathQueue;
        void _lock(void);
        void _unlock(void);
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

