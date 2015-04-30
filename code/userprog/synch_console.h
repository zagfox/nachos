#ifndef SYNCH_CONSOLE_H
#define SYNCH_CONSOLE_H

#include "synch.h"
#include "console.h"

class SynchConsole {
public:
	SynchConsole();
	~SynchConsole();

	// read from console to buffer, return actual bytes read
	int ReadConsole(char* buffer, int size);

	// write from buffer to console
	int WriteConsole(char *buffer, int size);

	void readAvailFunc();
	void writeDoneFunc();

private:
	Lock *readLock;  // to ensure one thread read/write at a time
	Lock *writeLock;
	Semaphore *writeDone;
	Semaphore *readAvail;
	Console *console;
};

#endif // SYNCH_CONSOLE_H
