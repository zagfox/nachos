#include "test_whale.h"

#include "whale.h"
#include "system.h"

void maleThread(int _args) {
	int *args = (int*)_args;
	Whale *whale = (Whale*)args[0];

	whale->Male();
	printf("complete %s\n", currentThread->getName());
}

void femaleThread(int _args) {
	int *args = (int*)_args;
	Whale *whale = (Whale*)args[0];

	whale->Female();
	printf("complete %s\n", currentThread->getName());
}

void makerThread(int _args) {
	int *args = (int*)_args;
	Whale *whale = (Whale*)args[0];

	whale->Matchmaker();
	printf("complete %s\n", currentThread->getName());
}

void WhaleTest0() {
	Whale *whale = new Whale();
	int *args = new int;
	args[0] = (int)whale;

	Thread *t;
	t = new Thread("male1");
	t->Fork(maleThread, (int)args);
	t = new Thread("male2");
	t->Fork(maleThread, (int)args);

	t = new Thread("female1");
	t->Fork(femaleThread, (int)args);
}

void WhaleTest1() {
	Whale *whale = new Whale();
	int *args = new int;
	args[0] = (int)whale;

	Thread *t;
	t = new Thread("male1");
	t->Fork(maleThread, (int)args);
	t = new Thread("male2");
	t->Fork(maleThread, (int)args);

	t = new Thread("female1");
	t->Fork(femaleThread, (int)args);

	t = new Thread("maker1");
	t->Fork(makerThread, (int)args);
}

void WhaleTest2() {
	Whale *whale = new Whale();
	int *args = new int;
	args[0] = (int)whale;

	Thread *t;
	t = new Thread("male1");
	t->Fork(maleThread, (int)args);
	t = new Thread("male2");
	t->Fork(maleThread, (int)args);

	t = new Thread("female1");
	t->Fork(femaleThread, (int)args);

	//currentThread->Yield();
	
	t = new Thread("female2");
	t->Fork(femaleThread, (int)args);

	t = new Thread("male3");
	t->Fork(maleThread, (int)args);

	t = new Thread("maker1");
	t->Fork(makerThread, (int)args);

	//currentThread->Yield();

	t = new Thread("maker2");
	t->Fork(makerThread, (int)args);
}
