#include "memory_manager.h"

#include <limits.h>
#include "synch.h"

MemoryManager::MemoryManager(int numPages) {
	mem_page_size = numPages;
	free_page_num = numPages;
	mem_map = new BitMap(numPages);
	fifo_list = new List();	
	lru_list = new int[numPages];
	mem_lock = new Lock("mem_mgr_lock");

	for (int i = 0; i < numPages; i++) {
		lru_list[i] = 0;
	}
}

MemoryManager::~MemoryManager() {
	delete mem_map;
	delete fifo_list;
	delete lru_list;
	delete mem_lock;
}

int MemoryManager::AllocPage() {
	mem_lock->Acquire();
	int page_id = mem_map->Find();
	if (page_id != -1) {
		free_page_num--;
		fifo_list->Append((void*)page_id);
		lru_list[page_id] = 0;// would update in translate
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
		//should free fifo_list, now it is done in GetEvictPhysPage
		//should free lru_list, now done in evict
	}
	mem_lock->Release();
}

int MemoryManager::GetEvictPhysPage() {
	mem_lock->Acquire();
	int page;
	if (TRUE) {
		page = (int)fifo_list->Remove();
	} else {
		int i, tmp, min_tick = INT_MAX;
		for (i = 0; i < mem_page_size; i++) {
			tmp = lru_list[i];
			if (tmp != 0 && tmp < min_tick) {
				page = i;
				min_tick = tmp;
			}
		}
		lru_list[page] = 0;
	}
	mem_lock->Release();
	return page;
}

bool MemoryManager::PageIsAllocated(int physPageNum) {
	ASSERT(physPageNum < mem_page_size);

	mem_lock->Acquire();
	bool allocated = mem_map->Test(physPageNum);
	mem_lock->Release();

	return allocated;
}
	
void MemoryManager::setUseTick(int physPageId, int totalTick) {
	lru_list[physPageId] = totalTick;
}
