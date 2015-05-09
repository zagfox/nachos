#include "backingstore.h"

#include "addrspace.h"
#include "system.h"

BackingStore::BackingStore() {
	file = NULL;
}

BackingStore::~BackingStore() {
	delete file;
	fileSystem->Remove(filename);
}

int BackingStore::Initialize(AddrSpace *as, int size) {
	filename[0] = (0xff000000 & (int)as) + 1;  
	filename[1] = (0x00ff0000 & (int)as) + 1;  
	filename[2] = (0x0000ff00 & (int)as) + 1;  
	filename[3] = (0x000000ff & (int)as) + 1;  
	filename[4] = 0;

	if (!fileSystem->Create(filename, size)) {
		return -1;
	}
	if (NULL == (file = fileSystem->Open(filename))) {
		return -1;
	}

	int length = file->Length();
	printf("file length %d\n", length);

	return 0;
}

void BackingStore::PageOut(TranslationEntry *pte) {
	// page out
	int virtAddr = pte->virtualPage * PageSize;
	int physAddr = pte->physicalPage * PageSize;
	int byte_write = file->WriteAt(&machine->mainMemory[physAddr], PageSize, virtAddr);
	ASSERT(byte_write != 0);
}

void BackingStore::PageIn(TranslationEntry *pte) {
	// page in
	int virtAddr = pte->virtualPage * PageSize;
	int physAddr = pte->physicalPage * PageSize;
	int byte_read = file->ReadAt(&machine->mainMemory[physAddr], PageSize, virtAddr);
	ASSERT(byte_read != 0);
}

