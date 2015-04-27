// addrspace.h
//	Data structures to keep track of executing user programs
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "memory_manager.h"

#define UserStackSize		1024 	// increase this as necessary!

#define ARG_MAX_LEN 32

struct segment;
typedef struct segment Segment;

class AddrSpace {
public:
    AddrSpace();	// Create an address space,
    // initializing it with the program
    // stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

	// Alloc space, 
	// leave room for arguments, at the end of va
	int Initialize(OpenFile *executable, int argc = 0);

	int InitArgs(int argc, char* argv[]);

    void InitRegisters();		// Initialize user-level CPU registers,
    // before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch

private:
	// Load segment to space
	void loadSegment(OpenFile *executable, Segment *seg, bool readonly);

	// copy the arg from virtual address from parent thread
	// then write to the end of space
	int InitSingleArg(int va_ptr);

	// write memory of space, silimiar to machine->WriteMem
	bool writeMem(int va, int size, int value);

    TranslationEntry *pageTable;	// Assume linear page table translation
    // for now!
    unsigned int numPages;		// Number of pages in the virtual
    // address space
	int args_num;
	unsigned int args_size;
	int args_ptr_pos, args_ptr_pos_base;
	int args_pos, args_pos_base;
};

#endif // ADDRSPACE_H
