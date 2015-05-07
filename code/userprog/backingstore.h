#ifndef BACKING_STORE_H
#define BACKING_STORE_H

#include "translate.h"
#include "openfile.h"

class AddrSpace;  

/* Create a backing store file for an AddrSpace */
class BackingStore {
public:
	BackingStore();   
	
	~BackingStore();

	int Initialize(AddrSpace *as, int size);   //plan to remove AddrSpace in init

	/* Write the virtual page referenced by pte to the backing store */
	/* Example invocation: PageOut(&machine->pageTable[virtualPage]) or */
	/*                     PageOut(&space->pageTable[virtualPage]) */
	void PageOut(TranslationEntry *pte);

	/* Read the virtual page referenced by pte from the backing store */
	void PageIn(TranslationEntry *pte);

private:
	char filename[5];
	OpenFile *file;
};

#endif // BACKING_STORE_H
