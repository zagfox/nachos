#include "sys_utility.h"

#include "machine.h"
#include "system.h"

// Preread to get read of page fault
// Allow one time for page Fault
bool machineReadMem(int addr, int size, int *value) {
	if (machine->ReadMem(addr, size, value)) {
		return TRUE;
	} else {
		return machine->ReadMem(addr, size, value);
	}
}

// Allow one time for page Fault
bool machineWriteMem(int addr, int size, int value) {
	if (machine->WriteMem(addr, size, value)) {
		return TRUE;
	} else {
		return machine->WriteMem(addr, size, value);
	}
}

// copy size bytes from user space to kernel space
// return 0 if not success
void u2k_memcpy(void *dst, const void* src, int size) {
	ASSERT(dst != 0);
	int i, v;
	for (i = 0; i < size; i++) {
		if (!machineReadMem((int)src + i, 1, &v)) {
			//return 0;
			ASSERT(FALSE);
		}
		((char*)dst)[i] = (char)v;
	}
}

void k2u_memcpy(void *dst, const void* src, int size) {
	int i;
	char c;
	for (i = 0; i < size; i++) {
		c = ((char*)src)[i];
		if(!machineWriteMem((int)dst + i, 1, (int)c)) {
			ASSERT(FALSE);
		}	
	}
	//return dst;
}

int copyFileName(int src_virt_addr, char *dst_phys_addr, int max_len) {
	char c;
	int i, v;
	for (i = 0; i < max_len; i++) {
		if (!machineReadMem(src_virt_addr + i, 1, &v)) {
			return -1;
		}
		c = (char)v;
		dst_phys_addr[i] = c;
		if (c == 0) {
			return 0;
		}
	}
	return -1; // error
}


