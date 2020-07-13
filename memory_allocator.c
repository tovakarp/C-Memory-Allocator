#include <stddef.h>
#include "memory_allocator.h"



struct MemoryAllocator
{
	size_t m_size;
	size_t *m_memory;
};

MemoryAllocator* MemoryAllocator_init(void* memoryPool, size_t size)

{
	MemoryAllocator* me = malloc(sizeof(MemoryAllocator));
	
	if (!me)
		return NULL;
		
	me->m_size = size;
	me->m_memory = memoryPool;
	
	/* TODO update metadata */
	
	return me;
}

void* MemoryAllocator_release(MemoryAllocator* allocator)
{
	void* mp_ptr = allocator->m_memory;
	free(allocator);
	
	return mp_ptr;
}


/*void* MemoryAllocator_allocate(MemoryAllocator* allocator, size_t size)
{
		
}*/
