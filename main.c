#include <stdio.h>
#include <assert.h>

#include "memory_allocator.h"

int main()
{
	void *memoryPool = malloc(64);
	void* ptr, *ptr1, *ptr2;
	MemoryAllocator *allocator;
	
	printf("--- Implicit Free List ---\n");
	
	if (memoryPool)
	{
		allocator = MemoryAllocator_init(memoryPool, 64);
		
		if (allocator)
		{
			ptr = MemoryAllocator_allocate(allocator, 72);
			
			if(ptr)
				assert(1);
				
			ptr = MemoryAllocator_allocate(allocator, 1);
			ptr1 = MemoryAllocator_allocate(allocator, 1);
			ptr2 = MemoryAllocator_allocate(allocator, 1);
			ptr = MemoryAllocator_allocate(allocator, 1);
			MemoryAllocator_free(allocator, ptr1);
			MemoryAllocator_free(allocator, ptr2);
			ptr = MemoryAllocator_allocate(allocator, 1);
			
			ptr = MemoryAllocator_allocate(allocator, 20);
			
			if(ptr)
				assert(1);
			
			printf(MemoryAllocator_optimize(allocator) == (size_t)16 ? "*** Tada! it works... ***\n" : "Oops! Something went wrong...\n");
			memoryPool = MemoryAllocator_release(allocator);
			
		}
		
		free(memoryPool);
	}
	
	return 0;
}



