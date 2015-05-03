#include "syscall.h"

void foo() {
	int i;
	char msg[5] = "msg1\n";
	for (i = 0; i < 3; i++) {
		Write(msg, 5, ConsoleOutput);
		Yield();
	}
	Exit(1); // if comment it, would infinite loop...
}

int main() {
	int i;
	char msg[5] = "msg2\n";
	Fork(foo);
	for (i = 0; i < 3; i++) {
		Write(msg, 5, ConsoleOutput);
		Yield();
	}
	Exit(2);
}

