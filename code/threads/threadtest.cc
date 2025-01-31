// threadtest.cc
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield,
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"

#include "badtest.h"
#include "test_cond.h"
#include "test_mailbox.h"
#include "test_join.h"
#include "test_priority.h"
#include "test_whale.h"

// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;

    for (num = 0; num < 5; num++) {
        printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
// LockTest1
//----------------------------------------------------------------------

Lock *locktest1 = NULL;

void
LockThread1(int param)
{
    printf("L1:0\n");
    locktest1->Acquire();
    printf("L1:1\n");
    currentThread->Yield();
    printf("L1:2\n");
    locktest1->Release();
    printf("L1:3\n");
}

void
LockThread2(int param)
{
	printf("L2:0\n");
	locktest1->Acquire();
	printf("L2:1\n");
	currentThread->Yield();
	printf("L2:2\n");
	locktest1->Release();
	printf("L2:3\n");
}

void
LockTest1()
{
	DEBUG('t', "Entering LockTest1");

	locktest1 = new Lock("LockTest1");

	Thread *t = new Thread("one");
	t->Fork(LockThread1, 0);
	t = new Thread("two");
	t->Fork(LockThread2, 0);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 1:
        ThreadTest1();
        break;
    case 2:
        ThreadTestBad();
        break;
    case 110:
        LockTest1();
        break;
    case 112:
        CondTestSignal();
        break;
    case 113:
        CondTestBroadcast();
        break;
    case 121:
        MailboxTest1();
        break;
    case 122:
        MailboxTest2();
        break;
    case 123:
        MailboxTest3();
        break;
    case 124:
        MailboxTest4();
        break;
    case 130:
        TestForkerThread();
        break;
    case 131:
        JoinTest1();
        break;
    case 132:
        JoinTest2();
        break;
    case 133:
        JoinTest3();
        break;
    case 134:
        JoinTest4();
        break;
    case 140:
        PriorityTest0();
        break;
    case 141:
        PriorityTest1();
        break;
    case 142:
        PriorityTest2();
        break;
    case 143:
        PriorityInversionTest1();
        break;
    case 144:
        PriorityInversionTest2();
        break;
    case 150:
        WhaleTest0();
        break;
    case 151:
        WhaleTest1();
        break;
    case 152:
        WhaleTest2();
        break;
    default:
        printf("No test specified.\n");
        break;
    }
}

