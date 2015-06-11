/**
 * @file           linked_list.c
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
#include <stdlib.h>

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */
#include "error_macros.h"
#include "common_types.h"
#include "linked_list.h"

/*******************************************************************************
 * Local Function Prototypes 
 *******************************************************************************
 */

/*******************************************************************************
 * Local Constants 
 *******************************************************************************
 */
#if !defined(TEST)
static Bool_t fakeMallocFailure = FALSE;
#else
Bool_t fakeMallocFailure = FALSE;
#endif /* !defined(TEST) */

/*******************************************************************************
 * File Scoped Variables 
 *******************************************************************************
 */

/*******************************************************************************
 ********************* E X T E R N A L  F U N C T I O N S **********************
 *******************************************************************************
 */

int create_list(List_Node_t **head)
{
    int stat = -1;
    List_Node_t *newItem  = NULL;

    EXIT_ON_NULL_PTR(head, stat);

    if (fakeMallocFailure == FALSE)
    {
        newItem = (List_Node_t *) malloc (sizeof(List_Node_t));
    }
    EXIT_ON_NULL_PTR(newItem, stat);

    stat = STATUS_SUCCESS;
    newItem->list_item = NULL; /* No item here yet */

    /* These point to each other, no items yet */
    newItem->next = NULL;
    newItem->prev = NULL;

    *head = newItem;

cleanup:
    return(stat);

error:
    if (newItem != NULL)
    {
        free(newItem);
        newItem = NULL;
    }
    goto cleanup;
}

#if 0
int add_item_before(List_Node_t *head)
{
}
#endif
int add_item_after(List_Node_t *head, void *item)
{
    int stat = STATUS_SUCCESS;
    List_Node_t *newItem  = NULL;

    EXIT_ON_NULL_PTR(head, stat);

    /* if first actual list item, don't need to malloc */
    if ((head->next == NULL) &&
            (head->prev == NULL) && (head->list_item == NULL))
    {
        head->list_item = item;
        /* We point to ourselves */
    }
    else
    {
        /* Create new list item */
        newItem = (List_Node_t *) malloc (sizeof(List_Node_t));
        EXIT_ON_NULL_PTR(newItem, stat);
        newItem->list_item = item;

        /* Setup for where it is in the list */
        newItem->next = head->next;
        newItem->prev = head;

        /* Now place it on the list */
        if (head->next != NULL)
        {
            head->next->prev = newItem;
        }
        head->next = newItem;
    }


cleanup:
    return(stat);
error:
    if (newItem != NULL)
    {
        newItem->next = NULL;
        newItem->prev = NULL;
        newItem->list_item = NULL;
        free(newItem);
        newItem = NULL;
    }
    goto cleanup;
}

int is_list_empty(List_Node_t *head, Bool_t *is_empty)
{
    int stat = STATUS_SUCCESS;

    EXIT_ON_NULL_PTR(head, stat);
    EXIT_ON_NULL_PTR(is_empty, stat);

    if (head->next == head->prev)
    {
        *is_empty = TRUE;
    }
    else
    {
        *is_empty = FALSE;
    }

cleanup:
    return(stat);
error:
    goto cleanup;
}

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */


