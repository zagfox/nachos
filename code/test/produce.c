#include <syscall.h>

int main() {
	char buffer[5] = "ping\n";
	while (1) {
		Write(buffer, 5, ConsoleOutput);
	}
	Exit(0);
}
