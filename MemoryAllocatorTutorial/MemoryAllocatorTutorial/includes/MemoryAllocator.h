#pragma once
#include <iostream>

class MemoryAllocator
{
public:
	//Struct that defines our block of memory itself, contains the objects header structure and a pointer to the actual payload
	struct MemoryBlock
	{
		//Object header
		size_t dataSize;
		size_t currentOffset;
		bool inUse;
		MemoryBlock* nextBlock;

		//Pointer to the payload data
		void* data[1];
	};

	MemoryAllocator(const std::size_t totalSize);

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
		BEST_FIT
	};

	size_t Align(size_t n);
	size_t AllocSize(size_t size);
	MemoryBlock* FindBlock(size_t size, SearchType searchType);

	MemoryBlock* FirstFitSearch(size_t size);
	MemoryBlock* NextFitSearch(size_t size);
	MemoryBlock* BestFitSearch(size_t size);

	//Mem start holds the pointer location of the start of the memory allocated, offset is used to work out how far along to scrub to get the next allocation space
	void* _memStart = nullptr;
	void* _top = nullptr;

	MemoryBlock* _lastSuccessLocation = nullptr;

	size_t _totalSize = 0;
	size_t _currentlyAllocated = 0;
};

