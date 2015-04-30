#ifndef SYS_UTILITY_H
#define SYS_UTILITY_H

#include "machine.h"

#define FILE_NAME_MAX_LEN 80

// copy size bytes from user space to kernel space
// return 0 if not success
void u2k_memcpy(void *dst, const void* src, int size) {
	ASSERT(dst != 0);
	int i, v;
	for (i = 0; i < size; i++) {
		if (!machine->ReadMem((int)src + i, 1, &v)) {
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
		if(!machine->WriteMem((int)dst + i, 1, (int)c)) {
			ASSERT(FALSE);
		}	
	}
	//return dst;
}

int copyFileName(int src_virt_addr, char *dst_phys_addr, int max_len) {
	char c;
	int i, v;
	for (i = 0; i < max_len; i++) {
		if (!machine->ReadMem(src_virt_addr + i, 1, &v)) {
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

#endif // SYS_UTILITY_H
