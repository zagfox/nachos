#include "page_manager.h"

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
		int evictPhysPageId = memoryMgr->GetEvictPhysPage();
		PageMeta *evictPageMeta = &pageMetaArray[evictPhysPageId];

		int evictVirtPageId = evictPageMeta->virtPageId;
		AddrSpace *evictPageSpace = (AddrSpace*)(evictPageMeta->spacePtr);

		memoryMgr->FreePage(evictPhysPageId);   //free the physical page
		evictPageSpace->PageOut(evictVirtPageId);
	}
	
	// then alloc physical page
	int physPageId = memoryMgr->AllocPage();
	pageMetaArray[physPageId].virtPageId = virtPageId;
	pageMetaArray[physPageId].spacePtr = (void*)currentThread->space;
	currentThread->space->PageIn(virtPageId, physPageId);

	pfLock->Release();
}

void PageManager::FreePage(int physPageId) {
	memoryMgr->FreePage(physPageId);
}

void PageManager::setUseTick(int physPageId, int totalTick) {
	memoryMgr->setUseTick(physPageId, totalTick);
}

