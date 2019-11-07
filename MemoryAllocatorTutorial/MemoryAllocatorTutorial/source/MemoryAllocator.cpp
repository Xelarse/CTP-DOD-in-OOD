#include "MemoryAllocator.h"
#include <stdlib.h>			//Malloc, free
#include <algorithm>		//max
#include <cassert>			//asserts on the free

MemoryAllocator::MemoryAllocator(const std::size_t totalSize) : _totalSize(totalSize)
{
}

void MemoryAllocator::Init()
{
	if (_memStart != nullptr)
	{
		Free(_memStart);
	}

	_memStart = malloc(_totalSize);
}

void* MemoryAllocator::Allocate(size_t size)
{
	//Align the size to fit multiples of void*
	size = Align(size);
	//Full allocation size
	size_t fullSize = AllocSize(size);

	size_t nextAddress;
	//Check if its the first memory allocation and if it is use the start address
	if (_top == nullptr)
	{
		nextAddress = reinterpret_cast<size_t>(_memStart);
	}
	//Check if there is already a block available to use thats been freed
	else if (auto block = FindBlock(size, SearchType::BEST_FIT))
	{
		nextAddress = reinterpret_cast<size_t>(block);
	}
	//Get the next address of allocation by shifting the last pointer along by its allocation size
	else
	{
		nextAddress = reinterpret_cast<size_t>(_top) + AllocSize(reinterpret_cast<MemoryBlock*>(_top)->dataSize);
	}
	 
	//Check if we can fit in another MemBlock of said size 
	if (_currentlyAllocated + fullSize > _totalSize)
	{
		return nullptr;
	}

	//cast the current address to a Mem block
	auto block = reinterpret_cast<MemoryBlock*>(nextAddress);

	block->dataSize = size;
	block->inUse = true;
	block->nextBlock = nullptr;
	block->currentOffset = 0;

	//Chain blocks
	if (_top != nullptr)
	{
		reinterpret_cast<MemoryBlock*>(_top)->nextBlock = block;
	}

	_top = block;
	_currentlyAllocated += fullSize;

	//Return the pointer to the data the use is going to use
	return block->data;
}

void MemoryAllocator::Free(void* ptr)
{
	MemoryBlock* block = GetBlockHeader(ptr);
	block->inUse = false;
}

void MemoryAllocator::Reset()
{
	_currentlyAllocated = 0;
	_top = nullptr;
}

size_t MemoryAllocator::Align(size_t n)
{
	//Aligns the size by the machine word
	//What this means is that if the user requests 6 bytes we actually allocate 8 bytes, allocating 4 results in 4 bytes on x32 or 8 bytes on x64
	return (n + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
}

size_t MemoryAllocator::AllocSize(size_t size)
{
	//returns the size of the requested data allocation + the size of the struct which is padding minus the space for the first piece of data
	//as its included in size and not required for the struct padding
	return size + sizeof(MemoryBlock) - sizeof(std::declval<MemoryBlock>().data);
}

MemoryAllocator::MemoryBlock* MemoryAllocator::FindBlock(size_t size, SearchType searchType)
{
	switch (searchType)
	{
		case MemoryAllocator::SearchType::FIRST_FIT:
		{
			return FirstFitSearch(size);
		}
		case MemoryAllocator::SearchType::NEXT_FIT:
		{
			return NextFitSearch(size);
		}
		case MemoryAllocator::SearchType::BEST_FIT:
		{
			return BestFitSearch(size);
		}
		default:
		{
			return nullptr;
		}
	}
}

MemoryAllocator::MemoryBlock* MemoryAllocator::FirstFitSearch(size_t size)
{
	//Starts at the start location of the heap and loops through all the blocks til it finds one of correct size

	MemoryBlock* currentBlock = reinterpret_cast<MemoryBlock*>(_memStart);

	while (currentBlock != nullptr)
	{
		//O(n) search
		if (currentBlock->inUse || currentBlock->dataSize < size)
		{
			currentBlock = currentBlock->nextBlock;
			continue;
		}

		return currentBlock;
	}

	return nullptr;
}

MemoryAllocator::MemoryBlock* MemoryAllocator::NextFitSearch(size_t size)
{
	//Same as First Fit but instead starts at the previous successful location, potentially skipping over blocks that are already allocated
	MemoryBlock* currentBlock = _lastSuccessLocation != nullptr ? _lastSuccessLocation : reinterpret_cast<MemoryBlock*>(_memStart);
	MemoryBlock* initialBlock = currentBlock;

	do
	{
		//O(n) search
		if (currentBlock->inUse || currentBlock->dataSize < size)
		{
			currentBlock = currentBlock->nextBlock != nullptr ? currentBlock->nextBlock : reinterpret_cast<MemoryBlock*>(_memStart);
			continue;
		}

		_lastSuccessLocation = currentBlock;
		return currentBlock;
	} while (currentBlock != initialBlock);

	return nullptr;
}

MemoryAllocator::MemoryBlock* MemoryAllocator::BestFitSearch(size_t size)
{
	//Search through all the blocks similar to next best 
	//get the first block that fits the size requested and cache it
	//Carry on searching the list, if you find on thats smaller than the current choice but still can fit the requested amount replace
	//If nothing is found return nullptr

	MemoryBlock* currentBlock = _lastSuccessLocation != nullptr ? _lastSuccessLocation : reinterpret_cast<MemoryBlock*>(_memStart);
	MemoryBlock* initialBlock = currentBlock;
	MemoryBlock* currentBestOption = nullptr;

	do
	{
		bool inUse = currentBlock->inUse;
		size_t blockSize = currentBlock->dataSize;

		//If the block isnt in use and it has a size bigger or equal to the size wanted
		if (!inUse && blockSize >= size)
		{
			if (blockSize == size)
			{
				//If the size is the size requested then just return it
				_lastSuccessLocation = currentBlock;
				return currentBlock;
			}
			else if (currentBestOption == nullptr)
			{
				//If there isnt a current best option then apply this to the current best as this is the current best fit
				//Set the next block up to continue
				currentBestOption = currentBlock;
				currentBlock = currentBlock->nextBlock != nullptr ? currentBlock->nextBlock : reinterpret_cast<MemoryBlock*>(_memStart);
			}
			else if (blockSize < currentBestOption->dataSize)
			{
				//IF the current block is better fit than the best block then assign that
				//Set up the next block to continue
				currentBestOption = currentBlock;
				currentBlock = currentBlock->nextBlock != nullptr ? currentBlock->nextBlock : reinterpret_cast<MemoryBlock*>(_memStart);
			}
		}
		else
		{
			//Otherwise just move onto the next block
			currentBlock = currentBlock->nextBlock != nullptr ? currentBlock->nextBlock : reinterpret_cast<MemoryBlock*>(_memStart);
		}
	} while (currentBlock != initialBlock);

	_lastSuccessLocation = currentBestOption != nullptr ? currentBestOption : _lastSuccessLocation;
	return currentBestOption;
}

MemoryAllocator::MemoryBlock* MemoryAllocator::GetBlockHeader(void* data)
{
	//Get the pointer position of the start of the memory block and cast it back to MemoryBlock
	const size_t address = reinterpret_cast<size_t>(data) + sizeof(std::declval<MemoryBlock>().data) - sizeof(MemoryBlock);
	return reinterpret_cast<MemoryBlock*>(address);
}
