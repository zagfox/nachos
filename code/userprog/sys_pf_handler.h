#ifndef SYS_PF_HANDLER_H
#define SYS_PF_HANDLER_H

#include "system.h"

void handlePageFault(int pageId) {
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
}

#endif // SYS_PF_HANDLER_H
