#include "test_cond.h"

#include "synch.h"
#include "system.h"

void signal_thread(int _params) {
	int *params = (int*)_params;
	Lock *l = (Lock*)params[0];
	Condition *c = (Condition*)params[1];

	l->Acquire();
	printf("thread signal begin\n");
	c->Signal(l);
	printf("thread signal end\n");
	l->Release();
}

void broadcast_thread(int _params) {
	int *params = (int*)_params;
	Lock *l = (Lock*)params[0];
	Condition *c = (Condition*)params[1];

	l->Acquire();
	printf("thread broadcast begin\n");
	c->Broadcast(l);
	printf("thread broadcast end\n");
	l->Release();
}

void wait_thread(int _params) {
	int *params = (int*)_params;
	Lock *l = (Lock*)params[0];
	Condition *c = (Condition*)params[1];

	l->Acquire();
	printf("thread wait begin\n");
	c->Wait(l);
	printf("thread wait end\n");
	l->Release();
}

//
void CondTestSignal() {
	Lock *l = new Lock("l1");
	Condition *c = new Condition("c1");

	int *params = new int[2];
	params[0] = (int)l;
	params[1] = (int)c;

	Thread *t;
	t = new Thread("t1");
	t->Fork(wait_thread, (int)params);
	t = new Thread("t2");
	t->Fork(wait_thread, (int)params);

	t = new Thread("t3");
	t->Fork(signal_thread, (int)params);
}

void CondTestBroadcast() {
	Lock *l = new Lock("l1");
	Condition *c = new Condition("c1");

	int *params = new int[2];
	params[0] = (int)l;
	params[1] = (int)c;

	Thread *t;
	t = new Thread("t1");
	t->Fork(wait_thread, (int)params);
	t = new Thread("t2");
	t->Fork(wait_thread, (int)params);

	t = new Thread("t3");
	t->Fork(broadcast_thread, (int)params);
}
