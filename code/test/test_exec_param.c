#include "syscall.h"

int main() {
	char *name = "/home/zagfox/cse120/nachos/code/test/test_exec_param_func";
	char* args[2];
	args[0] = "a\n";
	args[1] = "b\n";
	Exec(name, 2, args, 0);
	Exit(1);
	//Halt();
}
