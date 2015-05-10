#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include "synch.h"
#include "memory_manager.h"

class PageMeta {
public:
	PageMeta() {
		virtPageId = -1;
		initTick = 0;
		usedTick = 0;
		spacePtr = 0;
	}

	void SetMeta(int _virtPageId, int _initTick, int _usedTick, void *_spacePtr) {
		virtPageId = _virtPageId;
		initTick = _initTick;
		usedTick = _usedTick;
		spacePtr = _spacePtr;
	}

	int virtPageId;
	int initTick;
	int usedTick;
	void* spacePtr;
};

// manage page, handle page fault
class PageManager {
public:
	PageManager(int numPages);

	~PageManager();

	void handlePageFault(int virtPageId);

	void FreePage(int physPageId);

	void setUseTick(int physPageId);

private:
	int getEvictPhysPageLRU();
	int getEvictPhysPageFIFO();

	int physPageNum;
	MemoryManager *memoryMgr;
	Lock *pfLock;
	PageMeta *pageMetaArray;
};

#endif // PAGE_MANAGER_H
