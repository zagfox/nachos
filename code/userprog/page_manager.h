#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include "synch.h"
#include "memory_manager.h"

class PageMeta {
public:
	PageMeta() {
		virtPageId = -1;
		spacePtr = 0;
	}
	int virtPageId;
	void* spacePtr;
};

// manage page, handle page fault
class PageManager {
public:
	PageManager(int numPages);

	~PageManager();

	void handlePageFault(int virtPageId);

	void FreePage(int physPageId);

	void setUseTick(int physPageId, int totalTick);

private:
	int physPageNum;
	MemoryManager *memoryMgr;
	Lock *pfLock;
	PageMeta *pageMetaArray;
};

#endif // PAGE_MANAGER_H
