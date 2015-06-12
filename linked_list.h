#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__
/**
 * @file           linked_list.h
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

/*******************************************************************************
 * Structures
 *******************************************************************************
 */
struct List_Node
{
    void *list_item;
    struct List_Node *next;
    struct List_Node *prev;
};
typedef struct List_Node List_Node_t;


/*******************************************************************************
 * Unions
 *******************************************************************************
 */

/*******************************************************************************
 * External Function Prototypes
 *******************************************************************************
 */
int create_list(List_Node_t **head);
int is_list_empty(List_Node_t *head, Bool_t *is_empty);
int add_item_after(List_Node_t *head, void *item);

/*******************************************************************************
 * Global Variables
 *******************************************************************************
 */
#if defined(TEST)
extern Bool_t fakeMallocFailure;
#endif /* defined(TEST) */

#if _MAIN_
#define GLOBAL_VAR_DECLARE
#else
#define GLOBAL_VAR_DECLARE extern
#endif /* _MAIN_ */

#endif /* __LINKED_LIST_H__ */

