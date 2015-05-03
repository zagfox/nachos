// progtest.cc
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"
#include "table.h"
#include "synch_console.h"
#include "bounded_buffer.h"

MemoryManager *memoryMgr = NULL;
Table *spaceIdTable = NULL;
SynchConsole *synchConsole = NULL;
BoundedBuffer *pipeBuffer = NULL;

//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

void
StartProcess(char *filename)
{
	// init MemoryManager in AddrSpace
	memoryMgr = new MemoryManager(NumPhysPages);
	spaceIdTable = new Table(256);   // a temp number
	synchConsole = new SynchConsole();
	pipeBuffer = new BoundedBuffer(256);

    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

    if (executable == NULL) {
        printf("Unable to open file %s\n", filename);
        return;
    }
    space = new AddrSpace();
	if (0 != space->Initialize(executable)) {
		printf("Unable to init space\n");
		return;
	}
    currentThread->setSpace(space);

    //delete executable;			// close file

	int id = spaceIdTable->Alloc((void*)currentThread);
	currentThread->setSpaceId(id);

    space->InitRegisters();		// set the initial register values
    space->RestoreState();		// load page table register

    machine->Run();			// jump to the user progam
    ASSERT(FALSE);			// machine->Run never returns;
    // the address space exits
    // by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(int arg) {
    readAvail->V();
}
static void WriteDone(int arg) {
    writeDone->V();
}

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void
ConsoleTest (char *in, char *out)
{
    char ch;

    console = new Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);

    for (;;) {
		printf("1\n");
        readAvail->P();		// wait for character to arrive
		printf("2\n");
        ch = console->GetChar();
		printf("3\n");
        console->PutChar(ch);	// echo it!
		printf("4\n");
        writeDone->P() ;        // wait for write to finish
		printf("5\n");
        if (ch == 'q') return;  // if q, quit
    }
}
