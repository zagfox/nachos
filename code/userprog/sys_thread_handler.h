#ifndef SYS_HANDLER_H
#define SYS_HANDLER_H

#include "system.h"
#include "sys_utility.h"

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

#endif // SYS_HANDLER_H
