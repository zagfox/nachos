/* sort.c
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Ideally, we could read the unsorted array off of the file system,
 *	and store the result back to the file system!
 */

#include "syscall.h"

int A[1024];	/* size of physical memory; with code, we'll run out of space!*/

int
main()
{
    int i, j, tmp;
	char msg[4] = "msg\0";

    /* first initialize the array, in reverse sorted order */
	Write(msg, 4, ConsoleOutput);
    for (i = 0; i < 1024; i++) {
        A[i] = 1023 - i;
    }
	Write(msg, 4, ConsoleOutput);

    /* then sort! */
    for (i = 0; i < 1024; i++) {
        for (j = 0; j < i; j++) {
            if (A[i] < A[j]) {	/* out of order -> need to swap ! */
                tmp = A[j];
                A[j] = A[i];
                A[i] = tmp;
            }
        }
		Write(msg, 4, ConsoleOutput);
    }

    Exit(A[0]);		/* and then we're done -- should be 0! */
}
