#include "memory_manager.h"
#include "synch.h"

MemoryManager::MemoryManager(int numPages) {
	mem_page_size = numPages;
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
	mem_lock->Release();
	return page_id;
}

void MemoryManager::FreePage(int physPageNum) {
	ASSERT(physPageNum < mem_page_size);

	mem_lock->Acquire();
	mem_map->Clear(physPageNum);
	mem_lock->Release();
}

bool MemoryManager::PageIsAllocated(int physPageNum) {
	ASSERT(physPageNum < mem_page_size);

	mem_lock->Acquire();
	bool allocated = mem_map->Test(physPageNum);
	mem_lock->Release();

	return allocated;
}

