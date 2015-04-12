#include "synch.h"
#include "system.h"

class Mailbox {
public:
	Mailbox(char* debugName);
	~Mailbox();
	void Send(int message);
	void Receive(int* message);
private:
	char *name;
	int  cntSend; //count of send 
	int  cntRecv;
	int  *buffer;
	Condition *cvSend, *cvRecv;
	Lock *lock;
};
