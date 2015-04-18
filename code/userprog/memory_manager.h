#include "system.h"
#include "bitmap.h"
#include "synch.h"

class MemoryManager {
public:
	MemoryManager(int numPages);
	~MemoryManager();

	/* Allocate a free page, returning its physical page number or -1
   	   if there are no free pages available. */
	int AllocPage();

	/* Free the physical page and make it available for future allocation. */
	void FreePage(int physPageNum);

	/* True if the physical page is allocated, false otherwise. */
	bool PageIsAllocated(int physPageNum);

private:
	int mem_page_size;
	BitMap *mem_map;
	Lock *mem_lock;
};
