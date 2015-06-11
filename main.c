/**
 * @file           main.c
 * @verbatim
 *******************************************************************************
 * File:           main.c
 *
 * Description:    <description>
 *
 * Author:         Douglas L. Potts
 *
 * Date:           09-Jun-2015
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
#include <unistd.h>
#include <sys/types.h>
/* #include <dirent.h> */
#include <stdio.h>
/* #include <string.h> */

#include <errno.h> /* for errno */
#include <stdlib.h> /* for exit() */
#include <limits.h> /* for LONG_MAX/LONG_MIN */

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "listdir.h"


/*******************************************************************************
 * Local Constants 
 *******************************************************************************
 */
// #define EXIT_FAILURE (-1)
#define BASE_TEN (0) /* Used for strtol */

/*******************************************************************************
 * Local Function Prototypes 
 *******************************************************************************
 */
// static void listdir(const char *name);

/*******************************************************************************
 * File Scoped Variables 
 *******************************************************************************
 */

/*******************************************************************************
 ********************* E X T E R N A L  F U N C T I O N S **********************
 *******************************************************************************
 */

int main (int argc, char *argv[])
{
    int opt = 1;
    char *endptr = NULL;
    long tmp_long = 1;
    long num_worker_threads = 1;
    char *first_dir = NULL;

    while ((opt = getopt(argc, argv, "t:")) != -1) {
        switch (opt) {
        case 't':
            tmp_long = strtol(optarg, &endptr, BASE_TEN);
            if ((errno == ERANGE && (tmp_long == LONG_MAX || tmp_long == LONG_MIN))
                    || (errno != 0 && tmp_long == 0))
            {
                perror("strtol");
                exit(EXIT_FAILURE);
            }

            if (endptr == optarg)
            {
                fprintf(stderr, "No digits were found\n");
                exit(EXIT_FAILURE);
            }
            num_worker_threads = tmp_long;
            break;
        default:
            fprintf(stderr, "Usage: %s [-t num_threads] <first_dir_path>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Now optind (declared extern int by <unistd.h>) is the index of the first non-option argument.
    // If it is >= argc, there were no non-option arguments.
    if (optind >= argc)
    {
        fprintf(stderr, "Usage: %s [-t num_threads] <first_dir_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    first_dir = argv[optind];

    printf ("Number of threads:  %li\n", num_worker_threads);
    printf ("Based dir:          %s\n", first_dir);

    listdir(first_dir);


    return 0;
} /* main */

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */

#if 0
static void
listdir (const char * dir_name)
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
                printf ("[%c] %s/%s\n", 'f', dir_name, d_name);
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
                // printf ("[%c] %s\n", 'd', path);
                if (path_length >= PATH_MAX) {
                    fprintf (stderr, "Path length has got too long.\n");
                    exit (EXIT_FAILURE);
                }
                /* Recursively call "listdir" with the new path. */
                listdir (path);
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
#endif
