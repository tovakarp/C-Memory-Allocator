#include <stdio.h>

#include "memory_allocator.h"

int main()
{
	void *memoryPool = malloc(1024);
	MemoryAllocator *allocator;
	
	printf("--- Implicit Free List ---\n");
	
	if (memoryPool)
	{
		allocator = MemoryAllocator_init(memoryPool, 1024);
		
		if (allocator)
		{
			memoryPool = MemoryAllocator_release(allocator);
		}
		
		free(memoryPool);
	}
	
	return 0;
}



