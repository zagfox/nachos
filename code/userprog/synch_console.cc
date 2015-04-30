#include "synch_console.h"

static void synchReadAvailFunc(int arg) {
	SynchConsole *c = (SynchConsole*)arg;
	c->readAvailFunc();
}
static void synchWriteDoneFunc(int arg) {
	SynchConsole *c = (SynchConsole*)arg;
	c->writeDoneFunc();
}

SynchConsole::SynchConsole() {
	readLock = new Lock("read Lock");
	writeLock = new Lock("write Lock");
	writeDone = new Semaphore("write done", 0);
	readAvail = new Semaphore("read avail", 0);
	console = new Console(NULL, NULL, synchReadAvailFunc, synchWriteDoneFunc, (int)this);
}

SynchConsole::~SynchConsole() {
	// May have assertion false in synch
	delete readLock;
	delete writeLock;
	delete writeDone;
	delete readAvail;
	delete console;
}

void SynchConsole::readAvailFunc() {
	readAvail->V();
}

void SynchConsole::writeDoneFunc() {
	writeDone->V();
}

// Read from console
// At least 1 bytes, at most size bytes.
// Return the bytes read
int SynchConsole::ReadConsole(char* buffer, int size) {
	int i = 0;
	char c;

	readLock->Acquire();

	// ensure get at least one char
	c = console->GetChar();
	if (c == EOF) {  
		readAvail->P();
		c = console->GetChar();
		buffer[i++] = c;
	} else {
		readAvail->P();
		buffer[i++] = c;
	}
	//printf("read console %d %c\n", (int)c, c);
	// Get the rest
	while (i < size) {
		c = console->GetChar();
		//printf("read console %d %c\n", (int)c, c);
		if (c == EOF) {
			break;
		}
		buffer[i++] = c;
		readAvail->P();
	}

	readLock->Release();

	return i;
}

int SynchConsole::WriteConsole(char *buffer, int size){
	int i = 0;
	char c;

	writeLock->Acquire();

	for (i = 0; i < size; i++) {
		c = buffer[i];
		console->PutChar(c);
		writeDone->P();
		/*if (c == '\n' || c == '\0') {
			break;
		}*/ // Just write size bytes
	}

	writeLock->Release();

	return i;
}

