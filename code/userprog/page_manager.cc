#include "page_manager.h"

#include <limits.h>
#include "system.h"

PageManager::PageManager(int numPages) {
	physPageNum = numPages;
	memoryMgr = new MemoryManager(numPages);
	pfLock = new Lock("pagefault_Lock");
	pageMetaArray = new PageMeta[numPages];
}

PageManager::~PageManager() {
	delete memoryMgr;
	delete pfLock;
	delete []pageMetaArray;
}

void PageManager::handlePageFault(int virtPageId) {
	pfLock->Acquire();

	stats->numPageFaults++;

	// if full, evict a page
	if (memoryMgr->GetFreePageNum() == 0) {
		int evictPhysPageId = getEvictPhysPageLRU();
		//int evictPhysPageId = getEvictPhysPageFIFO();

		PageMeta *evictPageMeta = &pageMetaArray[evictPhysPageId];

		int evictVirtPageId = evictPageMeta->virtPageId;
		AddrSpace *evictPageSpace = (AddrSpace*)(evictPageMeta->spacePtr);

		memoryMgr->FreePage(evictPhysPageId);   //free the physical page
		evictPageSpace->PageOut(evictVirtPageId);
	}
	
	// then alloc physical page
	int physPageId = memoryMgr->AllocPage();
	pageMetaArray[physPageId].SetMeta(virtPageId, 
		stats->totalTicks, stats->totalTicks, (void*)currentThread->space);
	currentThread->space->PageIn(virtPageId, physPageId);

	pfLock->Release();
}

void PageManager::FreePage(int physPageId) {
	memoryMgr->FreePage(physPageId);
}

void PageManager::setUseTick(int physPageId) {
	pageMetaArray[physPageId].usedTick = stats->totalTicks;;
}

int PageManager::getEvictPhysPageFIFO() {
	int i, page = -1;
	int tmp, min_tick = INT_MAX;
	for (i = 0; i < physPageNum; i++) {
		tmp = pageMetaArray[i].initTick;
		if (tmp != 0 && tmp < min_tick) {
			page = i;
			min_tick = tmp;
		}
	}
	ASSERT(page != -1);

	return page;
}

int PageManager::getEvictPhysPageLRU() {
	int i, page = -1;
	int tmp, min_tick = INT_MAX;
	for (i = 0; i < physPageNum; i++) {
		tmp = pageMetaArray[i].usedTick;
		if (tmp != 0 && tmp < min_tick) {
			page = i;
			min_tick = tmp;
		}
	}
	ASSERT(page != -1);

	return page;
}
