#include "syscall.h"

void foo() {
	char msg[5] = "msg1\n";
	Write(msg, 5, ConsoleOutput);
	Exit(1); // if comment it, would infinite loop...
}

void bar() {
	char msg[5] = "msg2\n";
	Fork(foo);
	Write(msg, 5, ConsoleOutput);
	return;
}

int main() {
	bar();
	Exit(0);
}
