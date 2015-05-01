#include "syscall.h"

int main() {
	void (*ptr)();
	ptr = -1;
	ptr();
	
	Exit(1);
	//Halt();
}
