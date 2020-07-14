#include <stddef.h>
#include <stdio.h>
#include "memory_allocator.h"

#define OCCUPY_BLOCK 1
#define FREE_BLOCK (size_t)~1


struct MemoryAllocator
{
	size_t m_size;
	void *m_memory;
};

/* ----------------------- AUXILIARY FUNCTIONS ----------------------- */

size_t getAlignedSize(size_t size)
{
	return (size % ALIGN) ? size + ALIGN + (ALIGN - (size % ALIGN)) : size + ALIGN;
}

size_t getMemorySize(void* memory)
{
	return (memory) ? (*(size_t*)memory & FREE_BLOCK) : 0;
}


void* findFreeBlock(MemoryAllocator* allocator, void* block)
{
	void* end = (char*)allocator->m_memory + allocator->m_size;
	
	while((block < end) && (*(size_t*)block & OCCUPY_BLOCK))
	{
		block = (char*)block + getMemorySize(block);
		
	}
	
	if (block >= end)
	{
		return NULL;
	}
		
		
	return block;
}


void splitMemory(void* memory, size_t size)
{
	*(size_t*)((char*)memory + size) = getMemorySize(memory) - size;	
	*(size_t*)memory = size;
}

void occupyBlock(void *memory)
{
	*(size_t*)memory |= OCCUPY_BLOCK;
}

void freeBlock(void* memory)
{
	*(size_t*)memory &= FREE_BLOCK;
}

void mergeBlocks(void *block)
{
	*(size_t*)block = getMemorySize(block) + getMemorySize((char*)block + *(size_t*)block);
}

/* ----------------------- API FUNCTIONS ----------------------- */

MemoryAllocator* MemoryAllocator_init(void* memoryPool, size_t size)
{
	MemoryAllocator* me = malloc(sizeof(MemoryAllocator));
	
	if (!me)
		return NULL;
		
	me->m_size = size;
	me->m_memory = memoryPool;
	
	*(size_t*)(me->m_memory) = size - (size % ALIGN);
	
	return me;
}


void* MemoryAllocator_release(MemoryAllocator* allocator)
{
	void* mp_ptr = allocator->m_memory;
	free(allocator);
	
	return mp_ptr;
}


void* MemoryAllocator_allocate(MemoryAllocator* allocator, size_t size)
{
	void* block = allocator->m_memory;
	void* end = (char*)allocator->m_memory + allocator->m_size;
	void* next;
	
	size = getAlignedSize(size);
	
	
	while(block < end)
	{	
		block = findFreeBlock(allocator, block);
		
		if (!block)
		{
			return NULL;
		}
		
		
		if (*(size_t*)block < size)
		{
			next = (char*)block + getMemorySize(block);
			while (*(size_t*)block < size && (*(size_t*)next | FREE_BLOCK) == FREE_BLOCK && next < end)
			{
				mergeBlocks(block);	
				next = (char*)block + getMemorySize(block);
			}
		}
		
		if(*(size_t*)block >= size)
		{
			if (getMemorySize(block) > size)
				splitMemory(block, size);
				
			occupyBlock(block);
			break;
		}
		
		block = (char*)block + getMemorySize(block);
	}
	
	if (block >= end)
	{	
		return NULL;
	}
			
	return (char*)block + ALIGN;
}


void MemoryAllocator_free(MemoryAllocator* allocator, void* ptr)
{
	void* next, *end = (char*)allocator->m_memory + allocator->m_size;
	ptr = (char*)ptr - ALIGN;
	next = (char*)ptr + getMemorySize(ptr);
	
	freeBlock(ptr);

	if (((*(size_t*)next | FREE_BLOCK) == FREE_BLOCK) && next < end)
	{
		mergeBlocks(ptr);
	}
}

size_t MemoryAllocator_optimize(MemoryAllocator* allocator)
{
	size_t max = 0;
	void *ptr = allocator->m_memory;
	void *end = (char*)allocator->m_memory + allocator->m_size;
	void *next; 
	
	while(ptr < end)
	{
		next = (char*)ptr + getMemorySize(ptr);
		
		while(((*(size_t*)ptr | FREE_BLOCK) == FREE_BLOCK) && next <= end)
		{
			max = max > getMemorySize(ptr)? max : getMemorySize(ptr);
			if ((*(size_t*)next | FREE_BLOCK) != FREE_BLOCK || next == end)
				break;
				
			if (((*(size_t*)next | FREE_BLOCK) == FREE_BLOCK))
			{
				mergeBlocks(ptr);
				next = (char*)ptr + getMemorySize(ptr);
			}	
		}
		
		ptr = (char*)ptr + getMemorySize(ptr);
	}
	
	return max;
}


