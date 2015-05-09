#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include "memory_manager.h"
#include "system.h"
#include "synch.h"

// manage page, handle page fault
class PageManager {
public:
	PageManager(int numPages) {
		memoryMgr = new MemoryManager(numPages);
		pfLock = new Lock("pagefault_Lock");
	}
	~PageManager() {
		delete memoryMgr;
		delete pfLock;
	}

	void handlePageFault(int pageId) {
		pfLock->Acquire();

		stats->numPageFaults++;

		// if full, evict a page
		if (memoryMgr->GetFreePageNum() == 0) {
			int evictPhysPageId = memoryMgr->GetEvictPhysPage();
			int evictVirtPageId = memoryMgr->GetVirtPageId(evictPhysPageId);
			AddrSpace *evictPageSpace = memoryMgr->GetPageSpace(evictPhysPageId);

			memoryMgr->FreePage(evictPhysPageId);   //free the physical page
			evictPageSpace->PageOut(evictVirtPageId);
		}
		
		// then alloc physical page
		int physPageId = memoryMgr->AllocPage(pageId, currentThread->space);
		currentThread->space->PageIn(pageId, physPageId);

		pfLock->Release();

	}

	void FreePage(int physPageId) {
		memoryMgr->FreePage(physPageId);
	}

	void setUseTick(int physPageId, int totalTick) {
		memoryMgr->setUseTick(physPageId, totalTick);
	}

private:
	MemoryManager *memoryMgr;
	Lock *pfLock;
};

#endif // PAGE_MANAGER_H
