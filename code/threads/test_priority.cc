#include "test_priority.h"

#include "system.h"
#include "synch.h"


void dummy_thread(int args) {
	printf("begin %s\n", currentThread->getName());
	currentThread->Yield();
	printf("end %s\n", currentThread->getName());
}

//0 0 0
void PriorityTest0() {
	Thread *t;
	t = new Thread("t1_0");
	t->setPriority(0);
	t->Fork(dummy_thread, 0);
	t = new Thread("t2_0");
	t->setPriority(0);
	t->Fork(dummy_thread, 0);
	t = new Thread("t3_0");
	t->setPriority(0);
	t->Fork(dummy_thread, 0);
}

//1 2 3
void PriorityTest1() {
	Thread *t;
	t = new Thread("t1_0");
	t->setPriority(1);
	t->Fork(dummy_thread, 0);
	t = new Thread("t2_0");
	t->setPriority(2);
	t->Fork(dummy_thread, 0);
	t = new Thread("t3_0");
	t->setPriority(3);
	t->Fork(dummy_thread, 0);
}

void get_lock_thread(int _args) {
	int *args = (int*)_args;
	Lock *lock = (Lock*)args[0];

	printf("begin acquire %s\n", currentThread->getName());
	lock->Acquire();
	printf("end acquire %s\n", currentThread->getName());
	lock->Release();
}

//higher priority get lock
void PriorityTest2() {
	Lock *lock = new Lock("lock");
	lock->Acquire();
	int *args = new int[2];
	args[0] = (int)lock;

	Thread *t;
	t = new Thread("t1_1");
	t->setPriority(1);
	t->Fork(get_lock_thread, (int)args);
	currentThread->Yield();

	t = new Thread("t2_2");
	t->setPriority(2);
	t->Fork(get_lock_thread, (int)args);
	currentThread->Yield();

	//begin compete
	lock->Release();
}
