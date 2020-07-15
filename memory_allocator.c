#include <stdbool.h>
#include "memory_allocator.h"

#define OCCUPY_BLOCK 1
#define FREE_BLOCK (size_t)~1


struct MemoryAllocator
{
	size_t m_size;
	void *m_memory;
};

/* ----------------------- AUXILIARY FUNCTIONS ----------------------- */

static size_t getAlignedSize(size_t size)
{
	return (size % ALIGN) ? size + ALIGN + (ALIGN - (size % ALIGN)) : size + ALIGN;
}


static size_t getBlockSize(void* memory)
{
	return (memory) ? (*(size_t*)memory & FREE_BLOCK) : 0;
}

static void* getMemory(MemoryAllocator* allocator)
{
	return allocator->m_memory;
}

static void* getEndOfMemory(MemoryAllocator* allocator)
{
	return (char*)getMemory(allocator) + allocator->m_size;
}

static void* getNextBlock(void* block)
{
	return (char*)block + getBlockSize(block);
}

static bool isBlockInMemory(MemoryAllocator* allocator, void* block)
{
	return block < getEndOfMemory(allocator);
}

static bool isBlockFree(void* block)
{
	return (*(size_t*)block | FREE_BLOCK) == FREE_BLOCK;
}

static bool isBlockOccupied(void* block)
{
	return !isBlockFree(block);
}


static void* findFreeBlock(MemoryAllocator* allocator, void* block)
{	
	while(isBlockInMemory(allocator, block) && isBlockOccupied(block))
	{
		block = getNextBlock(block);	
	}
	
	if (!isBlockInMemory(allocator, block))
	{
		return NULL;
	}
				
	return block;
}


static void splitMemory(void* memory, size_t size)
{
	*(size_t*)((char*)memory + size) = getBlockSize(memory) - size;	
	*(size_t*)memory = size;
}

static void occupyBlock(void *memory)
{
	*(size_t*)memory |= OCCUPY_BLOCK;
}

static void freeBlock(void* memory)
{
	*(size_t*)memory &= FREE_BLOCK;
}

static void mergeBlocks(void *block)
{
	*(size_t*)block = getBlockSize(block) + getBlockSize((char*)block + *(size_t*)block);
}


static void mergeAdjacentBlocks(MemoryAllocator* allocator, void* block, size_t* max)
{
	void* end = getEndOfMemory(allocator);
	void* next = getNextBlock(block);
	
	while ( isBlockFree(block) && next <= end)
	{
		*max = *max > getBlockSize(block)? *max : getBlockSize(block);
		if (isBlockOccupied(next)|| next == end)
			break;

		mergeBlocks(block);
		next = getNextBlock(block);
	}
}

static void* getBlockMetadata(void* block)
{
	return (char*)block - ALIGN;
}


static void mergeBlocksIfNeeded(MemoryAllocator *allocator, void* block, size_t size)
{	
	if (getBlockSize(block) < size)
		{	
			while (getBlockSize(block) < size && isBlockFree(getNextBlock(block)) && isBlockInMemory(allocator, getNextBlock(block)))
			{
				mergeBlocks(block);	
			}
		}
}

static bool occupuBlockAndSplitIfNeeded(void *block, size_t size)
{
	if(getBlockSize(block) >= size)
	{
		if (getBlockSize(block) > size)
			splitMemory(block, size);
			
		occupyBlock(block);
		return 1;
	}
	return 0;
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
	void* mp_ptr = getMemory(allocator);
	free(allocator);
	
	return mp_ptr;
}


void* MemoryAllocator_allocate(MemoryAllocator* allocator, size_t size)
{
	void* block = getMemory(allocator);
	
	size = getAlignedSize(size);
	
	while ( isBlockInMemory(allocator, block) )
	{	
		block = findFreeBlock(allocator, block);
		
		if (!block)
			return NULL;

		mergeBlocksIfNeeded(allocator, block, size);
		
		if(occupuBlockAndSplitIfNeeded(block, size))
			break;
		
		block = getNextBlock(block);
	}
	
	if (!isBlockInMemory(allocator, block))	
		return NULL;
			
	return (char*)block + ALIGN;
}


void MemoryAllocator_free(MemoryAllocator* allocator, void* ptr)
{
	ptr = getBlockMetadata(ptr);
	
	freeBlock(ptr);

	if (isBlockFree(getNextBlock(ptr)) && isBlockInMemory(allocator, getNextBlock(ptr)))
	{
		mergeBlocks(ptr);
	}
}


size_t MemoryAllocator_optimize(MemoryAllocator* allocator)
{
	size_t max = 0;
	void *ptr = getMemory(allocator);
	
	while(isBlockInMemory(allocator, ptr))
	{	
		mergeAdjacentBlocks(allocator, ptr, &max);
		
		ptr = getNextBlock(ptr);
	}
	
	return max;
}


