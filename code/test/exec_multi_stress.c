#include "syscall.h"

int main() {
	char *name1 = "/home/zagfox/cse120/nachos/code/test/array";
	char *name2 = "/home/zagfox/cse120/nachos/code/test/sort";
	int x;
	x = Exec(name2, 0, 0, 0);
	x = Exec(name2, 0, 0, 0);
	//x = Exec(name1, 0, 0, 0);
	//x = Exec(name1, 0, 0, 0);
	//x = Exec(name1, 0, 0, 0);
	//x = Exec(name1, 0, 0, 0);
	Exit(x);
}
