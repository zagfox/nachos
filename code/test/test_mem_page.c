#include "syscall.h"

int A[32][32];	

int main()
{
    int i;

    for (i = 0; i < 16; i++) {
		A[i][0] = 0; // 16 page fault
	}

	Halt();
}
