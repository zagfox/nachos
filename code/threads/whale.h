
#include "synch.h"
#include "system.h"

class Whale {
public:
	Whale();
	~Whale();

	void Male();
	void Female();
	void Matchmaker();
private:
	int maleReady, femaleReady, makerReady;
	Lock *whaleLock;
	Condition *maleCv, *femaleCv, *makerCv;
};
