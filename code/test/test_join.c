#include "syscall.h"

int main() {
	int id = 0, status = 0;
	char *name = "/home/zagfox/cse120/nachos/code/test/exit";
	id = Exec(name, 0, 0, 1);
	status = Join(id);
	Exit(status);
}
