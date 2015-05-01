#ifndef BOUNDED_BUFFER_H
#define BOUNDED_BUFFER_H

#include "synch.h"

class BoundedBuffer {
public:
	BoundedBuffer(int _size);
	~BoundedBuffer();

	int Read(char* buffer, int size);
	int Write(char* buffer, int size);

private:
	bool isFull();
	bool isEmpty();
	int getAvailSize();

	int size;
	int start;
	int end;
	char *array;

	Lock *readLock;
	Lock *writeLock;
	Semaphore *writeAvail;
	Semaphore *readAvail;
};
#endif // BOUNDED_BUFFER_H
