#ifndef SYS_UTILITY_H
#define SYS_UTILITY_H

#define FILE_NAME_MAX_LEN 80

// Preread to get read of page fault
bool machineReadMem(int addr, int size, int *value);

bool machineWriteMem(int addr, int size, int value);

// copy size bytes from user space to kernel space
// return 0 if not success
void u2k_memcpy(void *dst, const void* src, int size);

void k2u_memcpy(void *dst, const void* src, int size);

int copyFileName(int src_virt_addr, char *dst_phys_addr, int max_len);

#endif // SYS_UTILITY_H
