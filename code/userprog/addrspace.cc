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
#ifdef HOST_SPARC
#include <strings.h>
#endif
#include "sys_utility.h"

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
	executable = NULL;
	noffH = NULL;
	store = new BackingStore();
	numThreads = 0;
}

void AddrSpace::loadSegment(Segment *seg, bool readonly) {
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

void AddrSpace::loadSegmentToPage(Segment *seg, bool readonly, int pageId) {
	int segStart = seg->virtualAddr;
	int segEnd = seg->virtualAddr + seg->size;
	int pageStart = pageId * PageSize;
	int pageEnd = (pageId + 1) * PageSize;

	int readStart, readEnd, readSize;
	int physAddr, fileAddr;

	if (segStart >= pageEnd || segEnd < pageStart) 
		return;

	readStart = max(segStart, pageStart);
	readEnd = min(segEnd, pageEnd);
	readSize = readEnd - readStart;

	physAddr = pageTable[pageId].physicalPage * PageSize + (readStart % PageSize);
	fileAddr = noffH->code.inFileAddr + readStart;   // always offset of code seg

	// set readonly
	if ((readSize == PageSize) && readonly) {
		pageTable[pageId].readOnly = TRUE;
	}

	executable->ReadAt(&(machine->mainMemory[physAddr]), readSize, fileAddr);
}

void AddrSpace::PageIn(int pageId) {
	// if full, evict a page
	if (memoryMgr->GetFreePageNum() == 0) {
		int evictPageId = 10; // virtual page to be evicted, now always evict page number smallest
		while (evictPageId < numPages && pageTable[evictPageId].valid == FALSE) {
			evictPageId++;
			//evictPageId = rand() % numPages;
		}
		printf("page out vid %d\n", evictPageId);

		if (pageTable[evictPageId].dirty == TRUE) {
			store->PageOut(&pageTable[evictPageId]);
		}
		memoryMgr->FreePage(pageTable[evictPageId].physicalPage);   //free the physical page
		pageTable[evictPageId].valid = FALSE;
		pageTable[evictPageId].use = FALSE;
		pageTable[evictPageId].dirty = FALSE;
		pageTable[evictPageId].physicalPage = -1;
	}
	// then alloc physical page
	pageTable[pageId].physicalPage = memoryMgr->AllocPage();

	if (pageTableInit[pageId] == 0) {
		printf("page in first time id %d\n", pageId);
		// zero the page
		char *ptr = &(machine->mainMemory[pageTable[pageId].physicalPage * PageSize]);
		bzero((void*)ptr, PageSize);

		// Load data
		loadSegmentToPage(&noffH->code, TRUE, pageId);
		loadSegmentToPage(&noffH->initData, FALSE, pageId);

		// mark page as initialized
		pageTableInit[pageId] = 1;
	} else {
		// then load from file
		printf("page in from file id %d\n", pageId);
		store->PageIn(&pageTable[pageId]);
	}

	// mark in pageTable
	pageTable[pageId].valid = TRUE;
}

int AddrSpace::Initialize(OpenFile *_executable, int argc) {
	ASSERT(memoryMgr != NULL);

	executable = _executable;
    noffH = new NoffHeader;
    unsigned int i, size;
	args_num = argc;
	// make it multiple of 4
	args_size = divRoundUp(argc * (4 + ARG_MAX_LEN + 1), 4) * 4;

    executable->ReadAt((char *)noffH, sizeof(NoffHeader), 0);
    if ((noffH->noffMagic != NOFFMAGIC) &&
            (WordToHost(noffH->noffMagic) == NOFFMAGIC))
        SwapHeader(noffH);
    ASSERT(noffH->noffMagic == NOFFMAGIC);

	// how big is address space?
	// we need to increase the size to leave room for the stack
    size = noffH->code.size + noffH->initData.size + noffH->uninitData.size
           + UserStackSize + args_size;
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
	args_ptr_pos_base = size - args_size;
	args_pos_base = args_ptr_pos_base + 4 * argc;

    //ASSERT(numPages <= NumPhysPages);		// check we're not trying
    // to run anything too big --
    // at least until we have
    // virtual memory
	/*if ((int)numPages > memoryMgr->GetFreePageNum()) {
		printf("addrspace init, memory shortage\n");
		return -1;
	}*/

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", numPages, size);
    DEBUG('a', "code size %d, initData size %d, uninitData size %d\n", 
		noffH->code.size, noffH->initData.size, noffH->uninitData.size);
// first, set up the translation
    pageTable = new TranslationEntry[numPages];
	pageTableInit = new int[numPages];
    for (i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i;	
        //pageTable[i].physicalPage = memoryMgr->AllocPage();
        pageTable[i].physicalPage = -1;
        //pageTable[i].valid = TRUE;
        pageTable[i].valid = FALSE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE;  // if the code segment was entirely on
        // a separate page, we could set its
        // pages to be read-only

		pageTableInit[i] = 0;
    }

	// At it very last, init backing store
	if (0 != store->Initialize(this, numPages * PageSize)) {
		return -1;
	}

	return 0;
}

int AddrSpace::InitSingleArg(int va_ptr) {
	int i, v;
	for (i = 0; i < ARG_MAX_LEN; i++) {
		if (!machineReadMem((int)va_ptr + i, 1, &v)) {
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
		if (!machineReadMem((int)argv + i * 4, 4, &va_ptr)) {
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
	if (virtPageId < 0 || virtPageId >= (int)numPages) {  //TODO, maynot need it
		return FALSE;
	}

	if (pageTable[virtPageId].valid == FALSE) {
		// should page in  // TODO
		ASSERT(FALSE); // solve later
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
	delete []pageTableInit;
	delete executable;
	delete noffH;
	delete store;
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
