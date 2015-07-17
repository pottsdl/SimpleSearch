#ifndef __ERROR_MACROS_H__
#define __ERROR_MACROS_H__
/**
 * @file           error_macros.h
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
#include <stdio.h>              /* for fprintf() */
#include <errno.h>              /* for errno() */

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */

/*******************************************************************************
 * Typedefs
 *******************************************************************************
 */

/*******************************************************************************
 * Constants
 *******************************************************************************
 */
typedef enum
{
    STATUS_SUCCESS = 0,
    STATUS_INVALID_PTR = 1,
    STATUS_UNKNOWN
} Status_Codes_t;

#define EXIT_EARLY_ON_ERROR(stat) \
    do \
    { \
        if (stat != 0) \
        { \
            fprintf(stderr, "[%s, %d:%s] stat = %d\n", __FILE__, __LINE__, __FUNCTION__, stat); \
            goto error; \
        } \
    } while(0)

#define EXIT_ON_NULL_PTR(ptr, err_stat) \
    do \
    { \
        if (NULL == ptr) \
        { \
          err_stat = STATUS_INVALID_PTR; \
          EXIT_EARLY_ON_ERROR(err_stat); \
        } \
    } while(0)

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

/*******************************************************************************
 * Global Variables
 *******************************************************************************
 */
#if _MAIN_
#define GLOBAL_VAR_DECLARE
#else
#define GLOBAL_VAR_DECLARE extern
#endif /* _MAIN_ */

#endif /* __ERROR_MACROS_H__ */
