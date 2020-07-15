#include <stdio.h>
#include <assert.h>

#include "memory_allocator.h"

int main()
{
	void *memoryPool = malloc(64);
	void* ptr, *ptr1, *ptr2;
	MemoryAllocator *allocator;
	
	if (memoryPool)
	{
		/* Initialize memory */
		allocator = MemoryAllocator_init(memoryPool, 64);
		
		if (allocator)
		{
			/* Allocate impossible size of memory */
			ptr = MemoryAllocator_allocate(allocator, 72);
			
			/* Managed to allocate inpossible size of memory? */
			if(ptr)
				assert(ptr);
			
			/* Allocate four blocks in Memory */	
			ptr = MemoryAllocator_allocate(allocator, 1); 	/* size: 16 */
			ptr1 = MemoryAllocator_allocate(allocator, 2);	/* size: 16 */
			ptr2 = MemoryAllocator_allocate(allocator, 8);	/* size: 16 */
			ptr = MemoryAllocator_allocate(allocator, 5);	/* size: 16 */
			
			/* Release two middle blocks */
			MemoryAllocator_free(allocator, ptr1);
			MemoryAllocator_free(allocator, ptr2);
			
			/* Allocate block in freed memory */
			ptr = MemoryAllocator_allocate(allocator, 7);
			
			/* Allocate impossible size of memory */
			ptr = MemoryAllocator_allocate(allocator, 20);
			
			/* Managed to allocate inpossible size of memory? */
			if(ptr)
				assert(ptr);
			
			/* Optimize memory and validate it! */
			printf(MemoryAllocator_optimize(allocator) == (size_t)16 ? "--- Implicit Free List ---\n" : "Oops! Something went wrong...\n");
			
			/* Destroy memory */
			memoryPool = MemoryAllocator_release(allocator);
		}
		
		free(memoryPool);
	}
	
	return 0;
}



