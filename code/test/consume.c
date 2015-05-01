#include <syscall.h>

int main() {
	char buffer[80];
	int size;
	while (1) {
		size = Read(buffer, 80, ConsoleInput);
		Write(buffer, size, ConsoleOutput);
	}
	Exit(0);
}
