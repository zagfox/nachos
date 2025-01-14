// Thread Safe Table

#ifndef TABLE_H
#define TABLE_H

class Lock;

/* Create a table to hold at most "size" entries. */
class Table {
public:
	Table(int _size);

	~Table();

	/* Allocate a table slot for "object", returning the "index" of the
	   allocated entry; otherwise, return 0 if no free slots are available. */
	int Alloc(void *object);

	/* Retrieve the object from table slot at "index", or NULL if that
	   slot has not been allocated. */
	void *Get(int index); 

	/* Free the table slot at index. */
	void Release(int index); 

private:
	int size;
	int *array;
	Lock *lock;
};

#endif // TABLE_H
