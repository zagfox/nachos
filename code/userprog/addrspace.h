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
#include "noff.h"
#include "translate.h"
#include "backingstore.h"

#define UserStackSize		1024 	// increase this as necessary!

#define ARG_MAX_LEN 32

class AddrSpace {
public:
    AddrSpace();	// Create an address space,
    // initializing it with the program
    // stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

	// Alloc space, 
	// leave room for arguments, at the end of va
	int Initialize(OpenFile *_executable, int argc = 0);

	int InitArgs(int argc, char* argv[]);

    void InitRegisters();		// Initialize user-level CPU registers,
    // before jumping to user code

	void InitFork(int func);   // Init when forking user thread
	int IncNumThread() { return (++numThreads); }
	int DecNumThread() { return (--numThreads); }

	// change pageTable
	void PageIn(int pageId, int physPageId);
	void PageOut(int virtPageId);

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch

private:
	// Load segment to page
	int loadSegmentToPage(Segment *seg, bool readonly, int pageId);

	// copy the arg from virtual address from parent thread
	// then write to the end of space
	int InitSingleArg(int va_ptr);

	// write memory of space, silimiar to machine->WriteMem
	bool writeMem(int va, int size, int value);

	// virtual memory related
	OpenFile *executable;
	NoffHeader *noffH;

	// backing store on disk for page
	BackingStore *store;

    TranslationEntry *pageTable;	
	int *pageTableInit;
    unsigned int numPages;		// Number of pages in the virtual
    // address space
	int args_num;
	unsigned int args_size;
	int args_ptr_pos, args_ptr_pos_base;
	int args_pos, args_pos_base;

	int numThreads;   // Number of threads running in the addrspace
};

#endif // ADDRSPACE_H
