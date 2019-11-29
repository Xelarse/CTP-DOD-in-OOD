#pragma once
#include <iostream>
#include <list>

class MemoryAllocator
{
public:
	//Struct that defines our block of memory itself, contains the objects header structure and a pointer to the actual payload
	struct MemoryBlock
	{
		//Object header
		size_t dataSize;
		bool inUse;
		MemoryBlock* nextBlock;

		//Pointer to the payload data
		void* data[1];
	};

	MemoryAllocator(const std::size_t elementSize, const size_t elementCount);
	MemoryAllocator(const MemoryAllocator&) = delete;
	~MemoryAllocator();

	void Init();
	void* Allocate(size_t size);
	void Free(void* ptr);
	void Reset();

	MemoryBlock* GetBlockHeader(void* data);

private:
	enum class SearchType
	{
		FIRST_FIT,
		NEXT_FIT,
		BEST_FIT,
		FREE_LIST
	};

	size_t Align(size_t n);
	size_t AllocSize(size_t size);
	MemoryBlock* FindBlock(size_t size, SearchType searchType);

	MemoryBlock* FirstFitSearch(size_t size);
	MemoryBlock* NextFitSearch(size_t size);
	MemoryBlock* BestFitSearch(size_t size);
	MemoryBlock* FreeListSearch(size_t size);

	MemoryBlock* SplitBlock(MemoryBlock* block, size_t size);
	bool CanSplit(MemoryBlock* block, size_t size);
	MemoryBlock* ListAllocate(MemoryBlock* block, size_t size);

	MemoryBlock* MergeAdjacentBlocks(MemoryBlock* block);
	bool CanMergeAdjacentBlocks(MemoryBlock* block);
	void RemoveOldMergedBlockFromFreeList(MemoryBlock* block);

	//Mem start holds the pointer location of the start of the memory allocated, offset is used to work out how far along to scrub to get the next allocation space
	void* _memStart = nullptr;
	void* _top = nullptr;

	MemoryBlock* _lastSuccessLocation = nullptr;

	size_t _totalSize = 0;
	size_t _currentlyAllocated = 0;

	//List of free blocks
	std::list<MemoryBlock*> _freeList;
};

