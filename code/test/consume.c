#include <syscall.h>

int main() {
	char buffer[80];
	while (1) {
		Read(buffer, 80, ConsoleInput);
	}
	Exit(0);
}
