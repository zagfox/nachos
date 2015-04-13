#include "test_priority.h"

#include "system.h"
#include "synch.h"


void dummy_thread(int args) {
	printf("I am %s\n", currentThread->getName());
}

void PriorityTest0() {
	Thread *t;
	t = new Thread("t1_0");
	t->Fork(dummy_thread, 0);
	//t->Join();
}
