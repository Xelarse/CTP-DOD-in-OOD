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
	else if (auto block = FindBlock(size, SearchType::FREE_LIST))
	{
		return block->data;
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

	block->nextBlock = nullptr;
	block->dataSize = size;
	block->inUse = true;
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
	if (CanMergeAdjacentBlocks(block))
	{
		block = MergeAdjacentBlocks(block);
	}
	block->inUse = false;
	block->currentOffset = 0;

	_freeList.push_back(block);
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
		case MemoryAllocator::SearchType::FREE_LIST:
		{
			return FreeListSearch(size);
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

		return ListAllocate(currentBlock, size);
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
		return ListAllocate(currentBlock, size);
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
	if (currentBestOption != nullptr)
	{
		return ListAllocate(currentBestOption, size);
	}
	return nullptr;
}

MemoryAllocator::MemoryBlock* MemoryAllocator::FreeListSearch(size_t size)
{
	MemoryBlock* foundBlock = nullptr;
	for (auto& block : _freeList)
	{
		//If the block is too small to use then continue searching
		if (block->dataSize < size)
		{
			continue;
		}
		//If we get a block remove it from the list and run list allocate to shrink it
		foundBlock = block;
		_freeList.remove(block);
		break;
	}
	if (foundBlock != nullptr)
	{
		return ListAllocate(foundBlock, size);
	}
	return nullptr;
}

MemoryAllocator::MemoryBlock* MemoryAllocator::SplitBlock(MemoryBlock* block, size_t size)
{
	size_t baseAddress = reinterpret_cast<size_t>(block);

	MemoryBlock* block1 = reinterpret_cast<MemoryBlock*>(baseAddress);
	MemoryBlock* block2 = reinterpret_cast<MemoryBlock*>(baseAddress + AllocSize(size));

	block1->currentOffset = 0;
	block1->inUse = false;
	block1->dataSize = size;
	block1->nextBlock = block->nextBlock;

	block2->currentOffset = 0;
	block2->inUse = false;
	block2->dataSize = block->dataSize - size;
	block2->nextBlock = block1;

	_freeList.push_back(block2);

	return block1;
}

bool MemoryAllocator::CanSplit(MemoryBlock* block, size_t size)
{
	return AllocSize(block->dataSize) - size >= sizeof(block);
}

MemoryAllocator::MemoryBlock* MemoryAllocator::ListAllocate(MemoryBlock* block, size_t size)
{
	//if (CanSplit(block, size))
	//{
	//	block = SplitBlock(block, size);
	//}

	block->inUse = true;
	block->dataSize = size;

	return block;
}

MemoryAllocator::MemoryBlock* MemoryAllocator::MergeAdjacentBlocks(MemoryBlock* block)
{
	//If the next block is not in use and the next block is the top, make this block the top block
	if (!block->nextBlock->inUse)
	{
		if (block->nextBlock == _top)
		{
			_top = block;
		}

		block->dataSize += block->nextBlock->dataSize;
		block->nextBlock = block->nextBlock->nextBlock;
	}

	return block;
}

bool MemoryAllocator::CanMergeAdjacentBlocks(MemoryBlock* block)
{
	//Can merge if there is a next block and its not being used
	return block->nextBlock && !block->nextBlock->inUse;
}

MemoryAllocator::MemoryBlock* MemoryAllocator::GetBlockHeader(void* data)
{
	//Get the pointer position of the start of the memory block and cast it back to MemoryBlock
	const size_t address = reinterpret_cast<size_t>(data) + sizeof(std::declval<MemoryBlock>().data) - sizeof(MemoryBlock);
	return reinterpret_cast<MemoryBlock*>(address);
}
