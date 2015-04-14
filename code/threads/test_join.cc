// join-example.cc 
//	Example of using Thread::Join().

#include "test_join.h"

#include "copyright.h"
#include "system.h"
#include "synch.h"

void
Joiner(Thread *joinee)
{
		currentThread->Yield();
		currentThread->Yield();
	
		printf("Waiting for the Joinee to finish executing.\n");

		//currentThread->Yield();
		//currentThread->Yield();

		// Note that, in this program, the "joinee" has not finished
		// when the "joiner" calls Join().  You will also need to handle
		// and test for the case when the "joinee" _has_ finished when
		// the "joiner" calls Join().

		joinee->Join();

		//currentThread->Yield();
		//currentThread->Yield();

		printf("Joinee has finished executing, we can continue.\n");

		currentThread->Yield();
		currentThread->Yield();
}

void
Joinee()
{
		int i;

		for (i = 0; i < 5; i++) {
				printf("Smell the roses.\n");
				currentThread->Yield();
		}

		currentThread->Yield();
		printf("Done smelling the roses!\n");
		currentThread->Yield();
}

void
TestForkerThread()
{
		Thread *joiner = new Thread("joiner", 0);  // will not be joined
		Thread *joinee = new Thread("joinee", 1);  // WILL be joined

		// fork off the two threads and let them do their business
		joiner->Fork((VoidFunctionPtr) Joiner, (int) joinee);
		joinee->Fork((VoidFunctionPtr) Joinee, 0);

		// this thread is done and can go on its merry way
		printf("Forked off the joiner and joiner threads.\n");
}

//child finish first
void
JoinTest1()
{
		Thread *joiner = new Thread("joiner", 0);  // will not be joined
		Thread *joinee = new Thread("joinee", 1);  // WILL be joined

		// fork off the two threads and let them do their business
		joinee->Fork((VoidFunctionPtr) Joinee, 0);
		currentThread->Yield();
		currentThread->Yield();
		currentThread->Yield();
		currentThread->Yield();
		currentThread->Yield();
		currentThread->Yield();

		joiner->Fork((VoidFunctionPtr) Joiner, (int) joinee);

		// this thread is done and can go on its merry way
		printf("Forked off the joiner and joiner threads.\n");
}

void nojoin_thread(int param) {
	printf("in nojoin_thread\n");
	currentThread->Yield();
}

//delete if no join called
void
JoinTest2()
{
		Thread *joiner = new Thread("t1", 0);
		Thread *joinee = new Thread("t2", 0);

		joiner->Fork((VoidFunctionPtr) nojoin_thread, 0);
		joinee->Fork((VoidFunctionPtr) nojoin_thread, 0);

		printf("Forked off the joiner and joiner threads.\n");
}

void
JoinTest3()
{
		Thread *t1 = new Thread("t1", 1);
		Thread *t2 = new Thread("t2", 1);

		t1->Fork((VoidFunctionPtr) nojoin_thread, 0);
		//t1->Join();
		t1->Join();
		
		t2->Fork((VoidFunctionPtr) nojoin_thread, 0);
		//t2->Join();
		t2->Join();
		printf("main end\n");
}

void
JoinTest4()
{
		Thread *t1 = new Thread("t1", 0);

		t1->Fork((VoidFunctionPtr) nojoin_thread, 0);
}
