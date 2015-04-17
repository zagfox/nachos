#include "memory_manager.h"

MemoryManager::MemoryManager(int numPages) {
}

MemoryManager::~MemoryManager() {
}

int MemoryManager::AllocPage() {
	return 0;
}

void MemoryManager::FreePage(int physPageNum) {
}

bool MemoryManager::PageIsAllocated(int physPageNum) {
	return true;
}

