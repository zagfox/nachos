#include "memory_manager.h"

#include "synch.h"

MemoryManager::MemoryManager(int numPages) {
	mem_page_size = numPages;
	free_page_num = numPages;
	mem_map = new BitMap(numPages);
	mem_lock = new Lock("mem_mgr_lock");
}

MemoryManager::~MemoryManager() {
	delete mem_map;
	delete mem_lock;
}

int MemoryManager::AllocPage() {
	mem_lock->Acquire();
	int page_id = mem_map->Find();
	if (page_id != -1) {
		free_page_num--;
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

