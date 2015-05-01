#include "syscall.h"

int main() {
	char *name = "/home/zagfox/cse120/nachos/code/test/test_addr_exception";
	Exec(name, 0, 0, 0);
	Exit(2);
}
