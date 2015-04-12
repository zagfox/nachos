#include "test_mailbox.h"

#include "system.h"
#include "mailbox.h"

void send_thread(int _params) {
	int *params = (int*)_params;
	Mailbox *mb = (Mailbox*)params[0];
	int message = (int)params[1];

	printf("send start %d\n", message);
	mb->Send(message);
	printf("send end\n");
}

void recv_thread(int _params) {
	int *params = (int*)_params;
	Mailbox *mb = (Mailbox*)params[0];

	int message = -1;

	printf("recv start\n");
	mb->Receive(&message);
	printf("recv end %d\n", message);
}

//first send, then recv
void MailboxTest1() {
	Mailbox *mb = new Mailbox("mb1");
	int *params = new int[2];
	params[0] = (int)mb;

	Thread *t;

	params[1] = 1;
	t = new Thread("t1");
	t->Fork(send_thread, (int)params);

	t = new Thread("t2");
	t->Fork(recv_thread, (int)params);
}

//first recv, then send
void MailboxTest2() {
	Mailbox *mb = new Mailbox("mb1");
	int *params = new int[2];
	params[0] = (int)mb;

	Thread *t;

	params[1] = 1;
	t = new Thread("t1");
	t->Fork(recv_thread, (int)params);
	t = new Thread("t2");
	t->Fork(send_thread, (int)params);
}
