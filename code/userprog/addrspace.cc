// addrspace.cc
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader (NoffHeader *noffH)
{
    noffH->noffMagic = WordToHost(noffH->noffMagic);
    noffH->code.size = WordToHost(noffH->code.size);
    noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
    noffH->initData.size = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical
//	memory.  For now, this is really simple (1:1), since we are
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace()
{
	numThreads = 0;
}

void AddrSpace::loadSegment(OpenFile *executable, Segment *seg, bool readonly) {
	int filePos, fileAddr;
	int virtPageId, physAddr, pageOffset;
	int readSize;

	for (filePos = 0; filePos < seg->size;) {
		virtPageId = (seg->virtualAddr + filePos) / PageSize;
		pageOffset = (seg->virtualAddr + filePos) % PageSize;
		physAddr = pageTable[virtPageId].physicalPage * PageSize + pageOffset;
		fileAddr = seg->inFileAddr + filePos;

		readSize = min(PageSize, seg->size - filePos);  // handle end of file
		readSize = min(readSize, PageSize - pageOffset); // handle start of file
		ASSERT(readSize > 0);

		if (readSize == PageSize && readonly) {
			pageTable[virtPageId].readOnly = TRUE;
		}
		executable->ReadAt(&(machine->mainMemory[physAddr]), readSize, fileAddr);
		filePos += readSize;
	}
}

int AddrSpace::Initialize(OpenFile *executable, int argc) {
	ASSERT(memoryMgr != NULL);

    NoffHeader noffH;
    unsigned int i, size;
	args_num = argc;
	// make it multiple of 4
	args_size = divRoundUp(argc * (4 + ARG_MAX_LEN + 1), 4) * 4;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
            (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

	// how big is address space?
	// we need to increase the size to leave room for the stack
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
           + UserStackSize + args_size;
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
	args_ptr_pos_base = size - args_size;
	args_pos_base = args_ptr_pos_base + 4 * argc;

    ASSERT(numPages <= NumPhysPages);		// check we're not trying
	if ((int)numPages > memoryMgr->GetFreePageNum()) {
		printf("addrspace init, memory shortage\n");
		return -1;
	}
    // to run anything too big --
    // at least until we have
    // virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n",
          numPages, size);
// first, set up the translation
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i;	
        pageTable[i].physicalPage = memoryMgr->AllocPage();
        pageTable[i].valid = TRUE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE;  // if the code segment was entirely on
        // a separate page, we could set its
        // pages to be read-only
    }

// zero out the entire address space, to zero the unitialized data segment
// and the stack segment
	char *ptr;
	for (i = 0; i < numPages; i++) {
		ptr = &(machine->mainMemory[pageTable[i].physicalPage * PageSize]);
		bzero((void*)ptr, PageSize);
	}

// then, copy in the code and data segments into memory
    DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
          noffH.code.virtualAddr, noffH.code.size);
	loadSegment(executable, &noffH.code, TRUE);	  

    DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
          noffH.initData.virtualAddr, noffH.initData.size);
	loadSegment(executable, &noffH.initData, FALSE);	  
	return 0;
}

int AddrSpace::InitSingleArg(int va_ptr) {
	int i, v;
	for (i = 0; i < ARG_MAX_LEN; i++) {
		if (!machine->ReadMem((int)va_ptr + i, 1, &v)) {
			return -1;
		}

		if (!writeMem(args_pos + i, 1, v)) {
			return -1;
		}
		if ((v & 0xff) == 0) {
			return 0;
		}
	}
	return -1;
}

// return 0 on success
int AddrSpace::InitArgs(int argc, char* argv[]) {
	int i;
	int va_ptr;
	args_ptr_pos = args_ptr_pos_base;
	args_pos = args_pos_base;

	for (i = 0; i < argc; i++) {
		// first, write in args_ptr_pos
		if (!writeMem(args_ptr_pos, 4, args_pos)) {
			return -1;
		}
		// then, write the actual args
		if (!machine->ReadMem((int)argv + i * 4, 4, &va_ptr)) {
			return -1;
		}
		if (0 != InitSingleArg(va_ptr)) {
			return -1;
		}
		args_ptr_pos += 4;
		args_pos += ARG_MAX_LEN + 1;
	}
	return 0;
}

bool AddrSpace::writeMem(int va, int size, int value) {
	int virtPageId, pageOffset;
	int physAddr;

	virtPageId = va / PageSize;
	pageOffset = va % PageSize;
	if (virtPageId < 0 || virtPageId >= (int)numPages) {
		return FALSE;
	}

	physAddr = pageTable[virtPageId].physicalPage * PageSize + pageOffset;

	switch (size) {
	case 1:
		machine->mainMemory[physAddr] = (unsigned char) value & (0xff);
		break;
	case 4:
		ASSERT(physAddr % 4 == 0);
		*(unsigned int*) &machine->mainMemory[physAddr] = WordToMachine((unsigned int)value);
		break;
	default:
		break;
	}
	return TRUE;
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------
AddrSpace::~AddrSpace()
{
	// free the memory in memoryMgr
	unsigned int i;
	int pageId;
	for (i = 0; i < numPages; i++) {
		pageId = pageTable[i].physicalPage;
		memoryMgr->FreePage(pageId);
	}

    delete [] pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;
    for (i = 0; i < NumTotalRegs; i++)
        machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we don't
    // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16 - args_size);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16 - args_size);

	// Init registers 4, 5, they are arguments
	machine->WriteRegister(4, args_num);
	machine->WriteRegister(5, args_ptr_pos_base);
}

void AddrSpace::InitFork(int func) {
	// Split stack for multiple threads
	int stackTop = numPages * PageSize - 16 - args_size - UserStackSize;
	int newstackPos = stackTop + (UserStackSize / numThreads);
	ASSERT(machine->ReadRegister(StackReg) > newstackPos);

	int i;
    for (i = 0; i < NumTotalRegs; i++)
        machine->WriteRegister(i, 0);

    machine->WriteRegister(PCReg, func);
    machine->WriteRegister(NextPCReg, func + 4);

	// set stack register
    machine->WriteRegister(StackReg, newstackPos);
    DEBUG('a', "Initializing stack register to %d\n", machine->ReadRegister(StackReg));

}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState()
{
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}
