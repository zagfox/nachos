#include "syscall.h"

int main() {
	char* ptr = -1;
	*ptr = 'a';
	
	Exit(1);
	//Halt();
}
