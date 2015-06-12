/**
 * @file           list_test.cpp
 * @brief:         <description>
 * @verbatim
 *******************************************************************************
 * Author:         Douglas L. Potts
 *
 * Date:           06/11/2015, <SCR #>
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
#include <list>
#include <stdio.h>

/*******************************************************************************
 * Project Includes
 *******************************************************************************
 */

/*******************************************************************************
 * Local Function Prototypes 
 *******************************************************************************
 */
void listTest (void);

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

extern "C" {
    /// Create an object that can be used to create objects for this class,
    /// and register it with the factory.  The name used as the registration
    /// key is arbitrary, but we'll use the class name so as not to confuse
    /// things.
    void callListTest (void)
    {
        listTest();
        return;
    }
    void callListTest2 (void)
    {
        list<int> intList;
        int intToAdd;
        list<int>::iterator iter;

        for (intToAdd = 1; intToAdd <= 5; intToAdd++) 
        {
            printf ("Adding int %d\n", intToAdd);
            intList.push_front(intToAdd);
        } /* end for */
        for (iter = intList.begin();
                iter != intList.end();
                iter++) 
        {
            printf ("Popping int %d\n", *iter);
        } /* end for */
    }
}

/*******************************************************************************
 ************************ L O C A L  F U N C T I O N S *************************
 *******************************************************************************
 */

void listTest (void)
{
    list<int> intList;
    int intToAdd;
    list<int>::iterator iter;

    for (intToAdd = 5; intToAdd > 0; intToAdd--) 
    {
        printf ("Adding int %d\n", intToAdd);
        intList.push_front(intToAdd);
    } /* end for */

    for (iter = intList.begin();
            iter != intList.end();
            iter++) 
    {
        printf ("Popping int %d\n", *iter);
    } /* end for */

    return;
}
