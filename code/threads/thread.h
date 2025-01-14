// thread.h
//	Data structures for managing threads.  A thread represents
//	sequential execution of code within a program.
//	So the state of a thread includes the program counter,
//	the processor registers, and the execution stack.
//
// 	Note that because we allocate a fixed size stack for each
//	thread, it is possible to overflow the stack -- for instance,
//	by recursing to too deep a level.  The most common reason
//	for this occuring is allocating large data structures
//	on the stack.  For instance, this will cause problems:
//
//		void foo() { int buf[1000]; ...}
//
//	Instead, you should allocate all data structures dynamically:
//
//		void foo() { int *buf = new int[1000]; ...}
//
//
// 	Bad things happen if you overflow the stack, and in the worst
//	case, the problem may not be caught explicitly.  Instead,
//	the only symptom may be bizarre segmentation faults.  (Of course,
//	other problems can cause seg faults, so that isn't a sure sign
//	that your thread stacks are too small.)
//
//	One thing to try if you find yourself with seg faults is to
//	increase the size of thread stack -- ThreadStackSize.
//
//  	In this interface, forking a thread takes two steps.
//	We must first allocate a data structure for it: "t = new Thread".
//	Only then can we do the fork: "t->fork(f, arg)".
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef THREAD_H
#define THREAD_H

#include "copyright.h"
#include "utility.h"

#ifdef USER_PROGRAM
#include "machine.h"
#include "addrspace.h"
#endif

// CPU register state to be saved on context switch.
// The SPARC and MIPS only need 10 registers, but the Snake needs 18.
// For simplicity, this is just the max over all architectures.
#define MachineStateSize 18


// Size of the thread's private execution stack.
// WATCH OUT IF THIS ISN'T BIG ENOUGH!!!!!
#define StackSize	(4 * 1024)	// in words

class Lock;
class Condition;

// Thread state
enum ThreadStatus { JUST_CREATED, RUNNING, READY, BLOCKED };

// external function, dummy routine whose sole job is to call Thread::Print
extern void ThreadPrint(int arg);

// The following class defines a "thread control block" -- which
// represents a single thread of execution.
//
//  Every thread has:
//     an execution stack for activation records ("stackTop" and "stack")
//     space to save CPU registers while not running ("machineState")
//     a "status" (running/ready/blocked)
//
//  Some threads also belong to a user address space; threads
//  that only run in the kernel have a NULL address space.

class Thread {
private:
    // NOTE: DO NOT CHANGE the order of these first two members.
    // THEY MUST be in this position for SWITCH to work.
    int* stackTop;			 // the current stack pointer
    int machineState[MachineStateSize];  // all registers except for stackTop

public:
    Thread(char* debugName, int join = 0);		// initialize a Thread
    ~Thread(); 				// deallocate a Thread
    // NOTE -- thread being deleted
    // must not be running when delete
    // is called

    // basic thread operations

    void Fork(VoidFunctionPtr func, int arg); 	// Make thread run (*func)(arg)
    void Yield();  				// Relinquish the CPU if any
    // other thread is runnable
    void Sleep();  				// Put the thread to sleep and
    // relinquish the processor
    void Finish();  				// The thread is done executing
	// called by parent, wait child to finish
	void Join();

	void setPriority(int newPriority);
	int  getPriority();
	void upgradePriority(int newPriority);
	void revertPriority();

#ifdef USER_PROGRAM
	void setSpace(AddrSpace* _space);
	void setSpaceId(int id) { spaceId = id; }
	int getSpaceId() { return spaceId; }
	void setExitCode(int code);
	int getExitCode();
	void setPipeInOut(int opt);
	int isPipeIn();
	int isPipeOut();
#endif	

    void CheckOverflow();   			// Check if thread has
    // overflowed its stack
    void setStatus(ThreadStatus st) {
        status = st;
    }
    char* getName() {
        return (name);
    }
    void Print() {
        printf("%s, %d", name, priority);
    }

private:
    // some of the private data for this class is listed above

	// join related, join can only start when join_ok is 0
	// real finish(delete tcb) can only start when join_complete is 1
	int join_need;       //if need to do join
	int join_ready;	     // if ok to do join, 0 not, 1 is ready
	int join_complete;   //if join has complete, 0 not, 1 is complete
	int join_called;     //if join has been called
	Lock *lock_join;
	Condition *cv_join;

	//priority related
	int priority;
	int priority_base;    //priority without upgrade

    int* stack; 	 		// Bottom of the stack
    // NULL if this is the main thread
    // (If NULL, don't deallocate stack)
    ThreadStatus status;		// ready, running or blocked
    char* name;

    void StackAllocate(VoidFunctionPtr func, int arg);
    // Allocate a stack for thread.
    // Used internally by Fork()


#ifdef USER_PROGRAM
// A thread running a user program actually has *two* sets of CPU registers --
// one for its state while executing user code, one for its state
// while executing kernel code.

    int userRegisters[NumTotalRegs];	// user-level CPU register state

	int spaceId;

	// exit_code
	int exit_code;

	// pipe_in_out
	int pipe_in;
	int pipe_out;

public:
    AddrSpace *space;			// User code this thread is running.

    void SaveUserState();		// save user-level register state
    void RestoreUserState();		// restore user-level register state

#endif
};

// Magical machine-dependent routines, defined in switch.s

extern "C" {
// First frame on thread execution stack;
//   	enable interrupts
//	call "func"
//	(when func returns, if ever) call ThreadFinish()
    void ThreadRoot();

// Stop running oldThread and start running newThread
    void SWITCH(Thread *oldThread, Thread *newThread);
}

#endif
// THREAD_H
