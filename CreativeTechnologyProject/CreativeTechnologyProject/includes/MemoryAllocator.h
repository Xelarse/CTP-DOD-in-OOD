#pragma once
#include <iostream>
#include <list>
#include <stdlib.h>

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

	////---------- Constructors and Destructors ----------////
	MemoryAllocator() = delete;
	MemoryAllocator(const MemoryAllocator&) = delete;

	MemoryAllocator(const std::size_t elementSize, const size_t elementCount)
	{
		_totalSize = (elementSize + sizeof(MemoryBlock)) * elementCount;
	}

	~MemoryAllocator()
	{
		if (_memStart != nullptr) { free(_memStart); }
	}

	////---------- Public Facing Functions ----------////
	/*
		Used to get the memory block from the OS based on the size inputted via the constructor, 
		If one already exists its frees the previous block.
	*/
	void Init()
	{
		if (_memStart != nullptr) { free(_memStart); }
		_memStart = malloc(_totalSize);
	}

	/*
		Used to get a pointer to store data in. 
		Amount of data that can be stored at the ptr is determined by the size parameter
	*/
	void* Allocate(size_t size)
	{
		size = Align(size);
		size_t fullSize = AllocSize(size);

		char* nextAddress;
		//Check if its the first memory allocation and if it is use the start address
		if (_top == nullptr) { nextAddress = reinterpret_cast<char*>(_memStart); }

		//Check if there is already a block available to use thats been freed
		else if (auto block = FindBlock(size)) { return block->data; }

		//Get the next address of allocation by shifting the last pointer along by its allocation size
		else { nextAddress = reinterpret_cast<char*>(_top) + AllocSize(reinterpret_cast<MemoryBlock*>(_top)->dataSize); }

		//Check if we can fit in another MemBlock of said size 
		if (_currentlyAllocated + fullSize > _totalSize) { return nullptr; }

		//cast the current address to a Mem block
		auto block = reinterpret_cast<MemoryBlock*>(nextAddress);
		block->nextBlock = nullptr;
		block->dataSize = size;
		block->inUse = true;

		//Chain blocks
		if (_top != nullptr) { reinterpret_cast<MemoryBlock*>(_top)->nextBlock = block; }

		_top = block;
		_currentlyAllocated += fullSize;

		//Return the pointer to the data the use is going to use
		return block->data;
	}

	/*
		Used to Free an existing allocation.
		Attempts to coalesce the allocation with the next if it is also free.
		Adds the Free block to the free list to be reused.
	*/
	void Free(void* ptr)
	{
		MemoryBlock* block = GetBlockHeader(ptr);
		if (CanMergeAdjacentBlocks(block))
		{
			block = MergeAdjacentBlocks(block);
		}
		block->inUse = false;

		_freeList.push_back(block);
	}

	/*
		Resets the current memory block of allocations
		Doesn't null the previous data just resets to the top so that allocations will begin from memStart again
	*/
	void Reset()
	{
		_currentlyAllocated = 0;
		_top = nullptr;
		_freeList.clear();
	}

private:
	////---------- Alignment And Utility Functions ----------////
	/*
		Aligns the inputted size based upon OS architecture
		8 bytes on x64 and 4 bytes on x32
	*/
	size_t Align(size_t n)
	{
		return (n + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
	}

	/*
		Returns the total size of an allocation.
		AKA the size of the header struct + the data to be stored
	*/
	size_t AllocSize(size_t size)
	{
		return size + sizeof(MemoryBlock) - sizeof(std::declval<MemoryBlock>().data);
	}

	/*
		Gets the pointer to the start of a memory block
		done by using the ptr to the data then scrubing back the size of memory block
	*/
	MemoryBlock* GetBlockHeader(void* data)
	{
		char* address = reinterpret_cast<char*>(data) + sizeof(std::declval<MemoryBlock>().data) - sizeof(MemoryBlock);
		return reinterpret_cast<MemoryBlock*>(address);
	}

	////---------- Free list Splitting, Coalesence and Search functions ----------////
	/*
		Finds a free block for allocation using a free list search method
	*/
	MemoryBlock* FindBlock(size_t size)
	{
		MemoryBlock* foundBlock = nullptr;
		for (auto& block : _freeList)
		{
			//If the block is too small to use then continue searching
			if (block->dataSize < size) { continue; }

			//If we get a block remove it from the list and 
			//run list allocate to shrink it
			foundBlock = block;
			_freeList.remove(block);
			break;
		}
		if (foundBlock != nullptr) { return ListAllocate(foundBlock, size); }
		return nullptr;
	}

	/*
		Used to split one large block into two smaller ones
		The first block will be of the size requested to fit an allocation
		The leftover will be made into a new block and added to the free list
	*/
	MemoryBlock* SplitBlock(MemoryBlock* block, size_t size)
	{
		char* baseAddress = reinterpret_cast<char*>(block);
		MemoryBlock* block1 = reinterpret_cast<MemoryBlock*>(baseAddress);
		char* block2Address = baseAddress + AllocSize(size);
		MemoryBlock* block2 = reinterpret_cast<MemoryBlock*>(block2Address);

		auto block2DataSize = block->dataSize - size;
		MemoryBlock* block2NextBlock = block->nextBlock;

		block1->inUse = false;
		block1->dataSize = size;
		block1->nextBlock = block2;

		block2->inUse = false;
		block2->dataSize = block2DataSize;
		block2->nextBlock = block2NextBlock;

		_freeList.push_back(block2);

		return block1;
	}

	/*
		Check to see if a block is splittable
		Returns true if the split block will have space for the header + some data
	*/
	bool CanSplit(MemoryBlock* block, size_t size)
	{
		return static_cast<int>(AllocSize(block->dataSize) - size) >= static_cast<int>(sizeof(block));
	}

	/*
		Called on allocation of a block, sets up the block and splits if possible
	*/
	MemoryBlock* ListAllocate(MemoryBlock* block, size_t size)
	{
		if (CanSplit(block, size)) { block = SplitBlock(block, size); }
		block->inUse = true;
		block->dataSize = size;
		return block;
	}

	/*
		Used to merge two free adjacent blocks into one large one
		This is called when an existing allocation is freed
	*/
	MemoryBlock* MergeAdjacentBlocks(MemoryBlock* block)
	{
		MemoryBlock* mergedBlock = nullptr;
		if (!block->nextBlock->inUse)
		{
			//If the adjacent block is the top block, make this the new top
			if (block->nextBlock == _top) { _top = block; }

			mergedBlock = block->nextBlock;
			block->dataSize += block->nextBlock->dataSize;
			block->nextBlock = block->nextBlock->nextBlock;
		}
		if (mergedBlock != nullptr) { _freeList.remove(mergedBlock); }
		return block;
	}

	/*
		Used to check if a merge is possible with the block following the block inputted via parameter
	*/
	bool CanMergeAdjacentBlocks(MemoryBlock* block)
	{
		return block->nextBlock && !block->nextBlock->inUse;
	}

	
	////---------- Private Variables ----------////
	void* _memStart = nullptr;
	void* _top = nullptr;

	MemoryBlock* _lastSuccessLocation = nullptr;

	size_t _totalSize = 0;
	size_t _currentlyAllocated = 0;

	//List of free blocks
	std::list<MemoryBlock*> _freeList;
};

