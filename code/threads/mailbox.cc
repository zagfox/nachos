#include "mailbox.h"

Mailbox::Mailbox(char *debugName) {
	name = debugName;
	buffer = new int;
	cvSend = new Condition("cvSend");
	cvRecv = new Condition("cvRecv");
	lock = new Lock("lock");
}

Mailbox::~Mailbox() {
	delete buffer;
	delete cvSend;
	delete cvRecv;
	delete lock;
}

void Mailbox::Send(int message) {
	lock->Acquire();

	cntSend++;
	*buffer = message;

	if (cntRecv == 0) {
		cvSend->Wait(lock);
	} else {
		cvRecv->Signal(lock);
		cntRecv = 0;
	}

	lock->Release();
}

void Mailbox::Receive(int* message) {
	lock->Acquire();
	cntRecv++;

	if (cntSend == 0) {
		cvRecv->Wait(lock);
	} else {
		//broadcast
		cvSend->Signal(lock);
		cntSend = 0;
	}

	//get info
	*message = *buffer;

	lock->Release();
}


