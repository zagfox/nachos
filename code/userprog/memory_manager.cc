#include "memory_manager.h"

#include "synch.h"
#include "addrspace.h"

MemoryManager::MemoryManager(int numPages) {
	mem_page_size = numPages;
	free_page_num = numPages;
	mem_map = new BitMap(numPages);
	pv_map = new int[numPages];
	fifo_list = new List();	
	pspace_map = new int[numPages];
	mem_lock = new Lock("mem_mgr_lock");

	for (int i = 0; i < numPages; i++) {
		pv_map[i] = -1;
		pspace_map[i] = 0;
	}
}

MemoryManager::~MemoryManager() {
	delete mem_map;
	delete pv_map;
	delete fifo_list;
	delete pspace_map;
	delete mem_lock;
}

int MemoryManager::AllocPage(int virtPageId, AddrSpace *space) {
	mem_lock->Acquire();
	int page_id = mem_map->Find();
	if (page_id != -1) {
		free_page_num--;
		pv_map[page_id] = virtPageId;
		fifo_list->Append((void*)page_id);
		pspace_map[page_id] = (int)space;
	}
	mem_lock->Release();
	return page_id;
}

int MemoryManager::GetFreePageNum() {
	return free_page_num;
}

void MemoryManager::FreePage(int physPageNum) {
	ASSERT(physPageNum < mem_page_size);

	mem_lock->Acquire();
	if (mem_map->Test(physPageNum)) {
		mem_map->Clear(physPageNum);
		free_page_num++;
		pv_map[physPageNum] = -1;
		//should free fifo_list, now it is done in GetEvictPhysPage
		pspace_map[physPageNum] = 0;
	}
	mem_lock->Release();
}

bool MemoryManager::PageIsAllocated(int physPageNum) {
	ASSERT(physPageNum < mem_page_size);

	mem_lock->Acquire();
	bool allocated = mem_map->Test(physPageNum);
	mem_lock->Release();

	return allocated;
}

int MemoryManager::GetVirtPageId(int physPageId) {
	return pv_map[physPageId];
}

int MemoryManager::GetEvictPhysPage() {
	mem_lock->Acquire();
	int page = (int)fifo_list->Remove();
	mem_lock->Release();
	return page;
}

AddrSpace* MemoryManager::GetPageSpace(int physPageId) {
	return (AddrSpace*)pspace_map[physPageId];
}
