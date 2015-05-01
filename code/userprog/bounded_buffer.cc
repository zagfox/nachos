#include "bounded_buffer.h"
#include "system.h"

BoundedBuffer::BoundedBuffer(int _size) {
	size = _size;
	start = 0;
	end = 1;
	array = new char[_size];

	readLock = new Lock("readLock");
	writeLock = new Lock("writeLock");
	readAvail = new Semaphore("readAvail", 0);
	writeAvail = new Semaphore("writeAvail", _size-1); // actual avail is size-1
}

BoundedBuffer::~BoundedBuffer() {
	delete array;

	delete readLock;
	delete writeLock;
	delete readAvail;
	delete writeAvail;
}

// Read At least one bytes
int BoundedBuffer::Read(char* buffer, int read_size) {
	readLock->Acquire();
	
	int i = 0;
	readAvail->P();
	buffer[i++] = array[start];
	start = (start + 1) % size;
	writeAvail->V();

	while (i < read_size && !isEmpty()) {
		readAvail->P();
		buffer[i++] = array[start];
		start = (start + 1) % size;
		writeAvail->V();
	}

	readLock->Release();
	return i;
}

// Must write size bytes
int BoundedBuffer::Write(char* buffer, int write_size) {
	int i;
	writeLock->Acquire();
	printf("bb write thread 1 %d\n", (int)currentThread);

	for (i = 0; i < write_size; i++) {
		writeAvail->P();
		array[end] = buffer[i];
		end = (end + 1) % size;
		readAvail->V();
	}

	printf("bb write thread 2 %d\n", (int)currentThread);
	writeLock->Release();

	return i;
}


bool BoundedBuffer::isFull() {
	return start == end;
}

bool BoundedBuffer::isEmpty() {
	return ((end - start + size) % size) == 1;
}

int BoundedBuffer::getAvailSize() {
	if (start == end) {
		return 0;
	} else {
		return (end - 1 - start + size) % size;
	}
}


