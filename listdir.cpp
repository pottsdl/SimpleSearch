/**
 * @file           listdir.c
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
#include <dirent.h> /* for readdir(), struct dirent, etc. */
#include <string.h> /* for strrchr() */
#include <stdio.h>  /* for fprintf() */
#include <errno.h>  /* for errno */

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "listdir.h"

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

extern void listdir(const char *dir_name, queue<char*> &fileQueue)
{
    DIR * directory_handle;

    /* Open the directory specified by "dir_name". */

    directory_handle = opendir (dir_name);

    /* Check it was opened. */
    if (! directory_handle) {
        fprintf (stderr, "Cannot open directory '%s': %s\n",
                 dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
    while (1) {
        struct dirent * entry;
        const char * d_name;

        /* "Readdir" gets subsequent entries from "directory_handle". */
        entry = readdir (directory_handle);
        if (! entry) {
            /* There are no more entries in this directory, so break
               out of the while loop. */
            break;
        }
        d_name = entry->d_name;
#if 0
        /* Print the name of the file and directory. */
        printf ("%s/%s\n", dir_name, d_name);

#else
        /* If you don't want to print the directories, use the
           following line: */

        if (! (entry->d_type & DT_DIR)) {
            const char *extension = strrchr(d_name, '.');
            if ((extension != NULL) && (strcmp(extension, ".txt") == 0))
            {
                // printf ("[%c] %s/%s\n", 'f', dir_name, d_name);
                /*
                 * Make space for a copy of the full file path
                 * dir_name + '/' + d_name + '\0';
                 */
                int pathLen = strlen(dir_name) + strlen(d_name) + 1 + 1;
                char *pathCopy = (char *) calloc(pathLen, sizeof(char));
                snprintf(pathCopy, pathLen, "%s/%s", dir_name, d_name);
                fileQueue.push(pathCopy);
            }
        }
#endif /* 0 */


        if (entry->d_type & DT_DIR) {

            /* Check that the directory is not "directory_handle" or directory_handle's parent. */
            
            if (strcmp (d_name, "..") != 0 &&
                strcmp (d_name, ".") != 0) {
                int path_length;
                char path[PATH_MAX];
 
                path_length = snprintf (path, PATH_MAX,
                                        "%s/%s", dir_name, d_name);
                /* printf ("[%c] %s\n", 'd', path); */
                if (path_length >= PATH_MAX) {
                    fprintf (stderr, "Path length has got too long.\n");
                    exit (EXIT_FAILURE);
                }
                /* Recursively call "listdir" with the new path. */
                listdir (path, fileQueue);
            }
	}
    }
    /* After going through all the entries, close the directory. */
    if (closedir (directory_handle)) {
        fprintf (stderr, "Could not close '%s': %s\n",
                 dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
}

