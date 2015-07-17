/**
 * @file           listdir.c
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
#include <dirent.h> /* for readdir(), struct dirent, etc. */
#include <string.h> /* for strrchr() */
#include <stdio.h>  /* for fprintf() */
#include <errno.h>  /* for errno */

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "listdir.hpp"

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

using namespace std;

/*******************************************************************************
 ********************* E X T E R N A L  F U N C T I O N S **********************
 *******************************************************************************
 */

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */

/**
 *******************************************************************************
 * @brief listdir - Take a base file path, recurse through the tree and add any
 * .txt file to the work queue.
 *
 * <!-- Parameters -->
 *      @param[in]      dir_name       C-String representation of base directory
 *                                     name.
 *      @param[out]     fileQueue      Pointer to the Work_Queue, on which to
 *                                     put any (*.txt) matching file path.
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
 *      Recurse from base directory (dir_name) on down, and for each file
 *      matching the ".txt" extension, add the full path and file name to the
 *      work queue.
 *******************************************************************************
 */
extern void listdir(const char *dir_name, Work_Queue *fileQueue)
{
    DIR *directory_handle;

    /* Open the directory specified by "dir_name". */
    directory_handle = opendir (dir_name);

    /* Check it was opened. */
    if (directory_handle == NULL)
    {
        fprintf (stderr, "Cannot open directory '%s': %s\n",
                 dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }

    while (1)
    {
        struct dirent *entry; /* directory entry struct */
        const char *d_name;   /* shortcut pointer to name in entry struct */

        /* "Readdir" gets subsequent entries from "directory_handle". */
        entry = readdir (directory_handle);

        /*
         * There are no more entries in this directory, so break out of the while loop.
         * NOTE:  This is the recursion exit condition.
         */
        if (entry == NULL)
        {
            break;
        }

        /* Now that we know entry is safe, point to 'd_name' in it */
        d_name = entry->d_name;

        /* Only need to check for extension if this is a file */
        if ((entry->d_type & DT_DIR) > 0)
        {
            /* Find first '.' starting at the end */
            const char *extension = strrchr(d_name, '.');

            /* If we have a file extension, AND that extension is ".txt" */
            if ((extension != NULL) && (strcmp(extension, ".txt") == 0))
            {

                /*
                 * Construct the full path to the file, appending file name to
                 * path
                 */
                string pathString = string(dir_name) + "/" + string(d_name);

                /* Add new file to the processing queue */
                fileQueue->push(pathString);
            }
        }
        else /* We have a dir, do checks & call recursively to process next */
        {
            /* Check that the directory is not "directory_handle" or directory_handle's parent. */
            if ((strcmp (d_name, "..") != 0 &&
                        strcmp (d_name, ".") != 0))
            {
                int path_length;
                char path[PATH_MAX];
 
                path_length = snprintf (path, PATH_MAX,
                                        "%s/%s", dir_name, d_name);
                if (path_length >= PATH_MAX)
                {
                    fprintf (stderr, "Path length has got too long.\n");
                    exit (EXIT_FAILURE);
                }

                /* Recursively call "listdir" with the new path. */
                listdir (path, fileQueue);
            }
        }
    }
    /* After going through all the entries, close the directory. */
    if (closedir (directory_handle))
    {
        fprintf (stderr, "Could not close '%s': %s\n",
                 dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
}

