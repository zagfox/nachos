// need to set physical page number larger
// As long as able to be hold in memory
// Ok to run
#include "syscall.h"

int main() {
	char *name = "/home/zagfox/cse120/nachos/code/test/exit";
	int x;
	x = Exec(name, 0, 0, 0);
	x = Exec(name, 0, 0, 0);
	Exit(x);
}
