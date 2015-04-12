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
	int  cnt;  //count of send 
	List *buffer;
	Condition *cv;
	Lock *lock;
};
