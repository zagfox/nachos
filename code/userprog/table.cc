#include "table.h"

#include "synch.h"
//#include "system.h"

Table::Table(int _size) {
	size = _size;
	array = new int[size];
	memset(array, 0, sizeof(int) * size);
	lock = new Lock("table lock");
}

Table::~Table() {
	delete array;
	delete lock;
}

int Table::Alloc(void *object) {
	lock->Acquire();
	int i;
	for (i = 0; i < size; i++) {
		if (array[i] == 0) {
			array[i] = (int)object;
			break;
		}
	}
	lock->Release();
	return i == size ? 0 : (i+1);
}

void* Table::Get(int _index) {
	int index = _index - 1;
	return (void*)array[index];
}

void Table::Release(int _index) {
	int index = _index - 1;
	lock->Acquire();
	array[index] = 0;
	lock->Release();
}

