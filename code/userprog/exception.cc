// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define FILE_NAME_MAX_LEN 80

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

int copyFileName(int src_virt_addr, char *dst_phys_addr, int max_len) {
	char c;
	int i, v;
	//name[FILE_NAME_MAX_LEN] = 0;
	for (i = 0; i < max_len; i++) {
		if (!machine->ReadMem(src_virt_addr + i, 1, &v)) {  //not 100% sure...
			return -1;
		}
		//machine->WriteMem(dst_phys_addr + i, 1, v);
		c = (char)v;
		dst_phys_addr[i] = c;
		if (c == 0) {
			return 0;
		}
	}
	return -1; // error
}

void exec_func(int args) {
	currentThread->space->InitRegisters();
    currentThread->space->RestoreState();		// load page table register

	machine->Run();
	ASSERT(FALSE);
}

SpaceId handleExec(int name_va, int argc, char **argv, int willJoin) {
	char name[FILE_NAME_MAX_LEN + 1];
	OpenFile *executable = NULL;
	AddrSpace *space = NULL;
	Thread *t = NULL;
	int id;
	
	// copy name to kernel memory
	if (copyFileName(name_va, name, FILE_NAME_MAX_LEN) != 0) {
		printf("Parse Filename Error\n");
		goto err;
	}
	//printf("handleExec copied_name: %s\n", name);

	// Create address space
	executable = fileSystem->Open(name);
	if (executable == NULL) {
		printf("Unable to open file %s\n", name);
		goto err;
	}
	space = new AddrSpace();
	if (0 != space->Initialize(executable, argc)) {
		printf("Exec, unable to init space\n");
		goto err;
	}
	// copy args
	if (0 != space->InitArgs(argc, argv)) {
		printf("Exec, unable to init args\n");
		goto err;
	}

    // Create thread
	t = new Thread("exec thread", willJoin);
	printf("Exec, currentThread %d, forkedThread %d\n", (int)currentThread, (int)t);
	t->space = space;

	// Manage space Id
	if (0 == (id = spaceIdTable->Alloc((void*)t))) {
		goto err;
	}

	// context switch
	t->Fork(exec_func, (int)space);

	delete executable;
	return id;

err:
	delete executable;
	delete space;
	delete t;
	return 0;
}

void handleExit(int status) {
	//Todo, release spaceId table

	currentThread->Finish();
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    int arg1 = machine->ReadRegister(4);
    int arg2 = machine->ReadRegister(5);
    int arg3 = machine->ReadRegister(6);
    int arg4 = machine->ReadRegister(7);

    if ((which == SyscallException) && (type == SC_Halt)) {
        printf("Shutdown, initiated by user program.\n");
        interrupt->Halt();
    } else if ((which == SyscallException) && (type == SC_Exec)) {
		printf("Syscall Exec, args %d %d %d %d\n", arg1, arg2, arg3, arg4);
		int ret = handleExec(arg1, arg2, (char**)arg3, arg4);
		machine->WriteRegister(2, ret);
    } else if ((which == SyscallException) && (type == SC_Exit)) {
        printf("Syscall Exit, Thread %d, code %d\n", (int)currentThread, arg1);
		handleExit(arg1);
    } else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
    }

	// Inc PC
	int PC, nextPC;
	PC = machine->ReadRegister(PCReg);
	nextPC = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PrevPCReg, PC);
	machine->WriteRegister(PCReg, nextPC);
	machine->WriteRegister(NextPCReg, nextPC + 4);
}
