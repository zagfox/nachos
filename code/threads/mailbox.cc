#include "mailbox.h"

Mailbox::Mailbox(char *debugName) {
	name = debugName;
	buffer = new List;
	cv = new Condition("cv");
	lock = new Lock("lock");
}

Mailbox::~Mailbox() {
	delete buffer;
	delete cv;
	delete lock;
}

void Mailbox::Send(int message) {
	lock->Acquire();

	cnt++;
	buffer->Append((void*)message);
	if (cnt > 0) {
		cv->Wait(lock);
	} else {
		cv->Signal(lock);
	}

	lock->Release();
}

void Mailbox::Receive(int* message) {
	lock->Acquire();

	cnt--;
	if (cnt < 0) {
		cv->Wait(lock);
	} else {
		cv->Signal(lock);
	}

	ASSERT(!buffer->IsEmpty());
	*message = (int)buffer->Remove();

	lock->Release();
}


