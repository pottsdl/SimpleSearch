#ifndef __BUFFER_PROCESSING_H__
#define __BUFFER_PROCESSING_H__
/**
 * @file           buffer_processing.hpp
 * @brief:         <description>
 * @verbatim
 *******************************************************************************
 * Author:         Douglas L. Potts
 *
 * Date:           06/12/2015, <SCR #>
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

/*******************************************************************************
 * System Includes
 *******************************************************************************
 */
#include <list>

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "common_types.h"
#include "word_dict.hpp"

#if defined(TEST)
extern "C"
{
    void bufferProcThreeWordsNotAtBegin (void);
    void bufferProcFourWordsAtBegin (void);
    void bufferProcOneWordAtBegin (void);
    void bufferProcOneWordAtEnd (void);
    void bufferProcFullBuffer (void);

    void fileProcess (void);
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

/*******************************************************************************
 * Unions
 *******************************************************************************
 */

/*******************************************************************************
 * External Function Prototypes
 *******************************************************************************
 */
Bool_t isWordChar (const char thisOne);
void processFile (int tid, std::string filePath, Word_Dict * dict);
int processBufferForWords (char *buffer, int buffer_sz, char **word);
int processWholeBuffer (char *buffer, int buffer_sz,
                        std::list < char *>&word_list);

/*******************************************************************************
 * Global Variables
 *******************************************************************************
 */
#if _MAIN_
#define GLOBAL_VAR_DECLARE
#else
#define GLOBAL_VAR_DECLARE extern
#endif /* _MAIN_ */

#endif /* __BUFFER_PROCESSING_H__ */
