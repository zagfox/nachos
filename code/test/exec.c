// Exec halt.c
#include "syscall.h"

int main() {
	char *name = "/home/zagfox/cse120/nachos/code/test/halt";
	Exec(name, 0, 0, 0);
	//Halt(); // not reached
	Exit(0);
}
