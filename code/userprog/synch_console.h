#ifndef SYNCH_CONSOLE_H
#define SYNCH_CONSOLE_H

#include "synch.h"
#include "console.h"

class SynchConsole {
public:
	SynchConsole();
	~SynchConsole();

	void ReadConsole(char* buffer, int size);
	void WriteConsole(char *buffer, int size);
};

#endif // SYNCH_CONSOLE_H
