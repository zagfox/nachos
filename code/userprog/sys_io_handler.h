
#ifndef SYS_IO_HANDLER_H
#define SYS_IO_HANDLER_H

#include "sys_utility.h"

// Return -1 on error
int handleWrite(int buffer_va, int size, OpenFileId id) {
	ASSERT(id == 1);
	int size_write;

	char *buffer = new char[size];
	u2k_memcpy(buffer, (void*)buffer_va, size);
	//printf("buffer %c %c\n", buffer[0], buffer[1]);
	size_write = synchConsole->WriteConsole(buffer, size);

	delete buffer;
	return size_write;
}

int handleRead(int buffer_va, int size, OpenFileId id) {
	ASSERT(id == 0);
	int size_read;
	char *buffer = new char[size + 1];

	size_read = synchConsole->ReadConsole(buffer, size);
	k2u_memcpy((void*)buffer_va, buffer, size);

	delete buffer;
	return size_read;
}

#endif // SYS_IO_HANDLER_H
