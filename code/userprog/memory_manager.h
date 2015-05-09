#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "bitmap.h"
#include "list.h"

class Lock;
class AddrSpace;

class MemoryManager {
public:
	MemoryManager(int numPages);
	~MemoryManager();

	/* Allocate a free page, returning its physical page number or -1
   	   if there are no free pages available. */
	   // ALso mark the physical page with virtPageId
	int AllocPage(int virtPageId, AddrSpace *space);

	// Get the number of free page
	int GetFreePageNum();

	/* Free the physical page and make it available for future allocation. */
	void FreePage(int physPageNum);

	/* True if the physical page is allocated, false otherwise. */
	bool PageIsAllocated(int physPageNum);

	// Given a physical page, return the mapped virtual page
	int GetVirtPageId(int physPageId);

	// in page swap, get the physical page that should evicted
	int GetEvictPhysPage();

	// Get the page Space according to phys page Id
	AddrSpace *GetPageSpace(int physPageId);

private:
	int mem_page_size;
	int free_page_num;
	BitMap *mem_map;
	int *pv_map;  // a mapping from physical page to virtual page id
	List *fifo_list;  // a fifo list that record the usage of physical page
	int *pspace_map; // mapping from physical page to addrspace
	Lock *mem_lock;
};

#endif // MEMORY_MANAGER_H
