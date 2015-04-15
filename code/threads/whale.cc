#include <whale.h>

/*
/------------------------------------------------------
/ Whale
/                            _   _ 
/                           _ \ / _ 
/                          / \ | / \
/                             \|/ 
/                         _.---v---._
/                /\_/\   /           \
/                \_ _/  /             \
/                  \ \_|          @ __|
/                   \               \_
/                    \     ,_/       /
/        ~~~~~~~~~~~~~ ~~~~~~~~~~~~~/~~~~~~~~~~~
/
//-----------------------------------------------------
*/


Whale::Whale() {
	maleReady = 0;
	femaleReady = 0;
	makerReady = 0;

	whaleLock = new Lock("whaleLock");
	maleCv = new Condition("maleCv");
	femaleCv = new Condition("femaleCv");
	makerCv = new Condition("makerCv");
}

Whale::~Whale() {
	delete whaleLock;
	delete maleCv;
	delete femaleCv;
	delete makerCv;
}

void Whale::Male() {
	whaleLock->Acquire();
	maleReady++;
	if (!(maleReady && femaleReady && makerReady)) {
		maleCv->Wait(whaleLock);
	} else {
		femaleCv->Signal(whaleLock);
		makerCv->Signal(whaleLock);
	}

	maleReady--;
	whaleLock->Release();
}

void Whale::Female() {
	whaleLock->Acquire();
	femaleReady++;
	if (!(maleReady && femaleReady && makerReady)) {
		femaleCv->Wait(whaleLock);
	} else {
		maleCv->Signal(whaleLock);
		makerCv->Signal(whaleLock);
	}

	femaleReady--;
	whaleLock->Release();
}

void Whale::Matchmaker() {
	whaleLock->Acquire();
	makerReady++;
	if (!(maleReady && femaleReady && makerReady)) {
		printf("maker wait\n");
		makerCv->Wait(whaleLock);
	} else {
		maleCv->Signal(whaleLock);
		femaleCv->Signal(whaleLock);
	}

	makerReady--;
	whaleLock->Release();
}
