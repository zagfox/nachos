#ifndef SYS_HANDLER_H
#define SYS_HANDLER_H

#include "system.h"
#include "sys_utility.h"

// Starter function executed by exec
void exec_func(int args) {
	currentThread->space->InitRegisters();
    currentThread->space->RestoreState();		// load page table register

	machine->Run();
	ASSERT(FALSE);
}

SpaceId handleExec(int name_va, int argc, char **argv, int opt) {
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
	t = new Thread("exec thread", opt & 0x1);
	t->setPipeInOut(opt);
	printf("Exec, currentThread %d, user exec Thread %d\n", (int)currentThread, (int)t);
	t->setSpace(space);

	// Manage space Id
	if (0 == (id = spaceIdTable->Alloc((void*)t))) {
		goto err;
	}
	t->setSpaceId(id);

	// context switch
	t->Fork(exec_func, 0);

	delete executable;
	return id;

err:
	delete executable;
	delete space;
	delete t;
	return 0;
}

// if multi-user thread are enabled, just exit one thread
void handleExit(int status) {
	// Release spaceId table
	spaceIdTable->Release(currentThread->getSpaceId());

	// Exit thread
	currentThread->setExitCode(status);
	currentThread->Finish();
}

int handleJoin(SpaceId id) {
	int code = -65535;
	Thread* child_thread = (Thread*)spaceIdTable->Get(id);
	if (child_thread != 0) {
		child_thread->Join();
		code = child_thread->getExitCode();
	}
	return code;
}

void fork_func(int arg) {
	currentThread->space->InitFork(arg);
    currentThread->space->RestoreState();	

	machine->Run();
	ASSERT(FALSE);
}

void handleFork(void (*func)()) {
	Thread *t = NULL;

	t = new Thread("forked thread", 0);
	printf("Exec, currentThread %d, user forked Thread %d\n", (int)currentThread, (int)t);
	t->setSpace(currentThread->space);

	// SpaceId??

	// fork
	t->Fork(fork_func, (int)func);
}

void handleYield() {
	currentThread->Yield();
}

#endif // SYS_HANDLER_H
