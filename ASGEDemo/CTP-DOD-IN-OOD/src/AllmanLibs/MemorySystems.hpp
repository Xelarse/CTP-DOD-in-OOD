#pragma once
#include <vector>
#include <iostream>
#include <list>
#include <stdlib.h>

/*
	This is the class that handles communication with the OS for Memory allocations.
	On construction it mallocs a large block of total memory.
	When allocations are requested it splits that larger block into smaller allocs.
	Works using a Free Linked list, if allocations are freed they can be reused.
	This is the low level backend for the Memory Manager.
*/
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
		if (CanMergeAdjacentBlocks(block)) { block = MergeAdjacentBlocks(block); }
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
	size_t Align(size_t n) { return (n + sizeof(void*) - 1) & ~(sizeof(void*) - 1); }

	/*
		Returns the total size of an allocation.
		AKA the size of the header struct + the data to be stored
	*/
	size_t AllocSize(size_t size) { return size + sizeof(MemoryBlock) - sizeof(std::declval<MemoryBlock>().data); }

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

/*
	The Memory Manager is used to interface with the Memory allocator.
	It requests large allocations to store multiple related variables within.
	Related variables are defined by a unique hash.
	The developer doesnt interface directly with Memory Manager,
	instead they utilise the AllmanVariable.
*/
class MemoryManager
{
	template<class T>
	friend class AllmanVariable;

public:
	MemoryManager(const size_t elementSize, const size_t elementCount) : _totalObjectCount(elementCount)
	{
		InitialiseAllocator(elementSize, elementCount);
	}

	~MemoryManager()
	{
		FreeAllExistingBlocks();
	}

private:
	struct ExistingBlocks
	{
		ExistingBlocks(size_t _id, void* _ptr, size_t _stride, size_t _capacity)
		{
			dataId = _id;
			currentCount = 0;
			maxCount = _capacity;
			stride = _stride;
			dataPointer = _ptr;
		}

		size_t dataId;					//The Hash id for the Block
		size_t currentCount;		//Current amount of variables stored in block
		size_t maxCount;			//The max capacity the block can hold
		size_t stride;					//The stride needed to move between variables
		void* dataPointer;				//The Base Ptr to the block of memory
	};

	MemoryManager() = delete;

	template <typename T>
	T* InitialiseVariable(size_t id)
	{
		//First check if an id exists, if it doesnt, create it
		if (!CheckIfIdExists(id))
		{
			InitialiseNewMemoryBlock<T>(id, _totalObjectCount);
		}

		auto& existingBlock = _existingAllocations[static_cast<size_t>(GetIndexOfId(id))];

		//Check if the block has space for another allocation
		if (existingBlock.currentCount + 1 > existingBlock.maxCount)
		{
			//TODO Add something here to make a new block of data and link them
			return nullptr;
		}
		else
		{
			//Figure out how far to stride along from the base pointer, store T into it and return the pointer to it
			char* baseBlockPointer = reinterpret_cast<char*>(existingBlock.dataPointer);
			char* newDataPointer = baseBlockPointer + existingBlock.stride * (existingBlock.currentCount);

			//Cast the newDataPointer to the desired variable and return the ptr after uping the count
			T* castedDataPointer = reinterpret_cast<T*>(newDataPointer);
			++existingBlock.currentCount;
			return castedDataPointer;
		}
	}

	template<typename T>
	T* InitialiseVariable(size_t id, T data)
	{
		T* outPtr = InitialiseVariable<T>(id);
		if (outPtr != nullptr)
		{
			*outPtr = data;
		}
		return outPtr;
	}

	//Initilises the memory block and returns the base ptr
	template<typename T>
	T* InitialiseNewMemoryBlock(size_t id, size_t maxCapacity)
	{
		void* dataPtr = _pAllocator->Allocate(sizeof(T) * maxCapacity);
		_existingAllocations.push_back(ExistingBlocks(id, dataPtr, sizeof(T), maxCapacity));
		return reinterpret_cast<T*>(dataPtr);
	}

	const ExistingBlocks& GetBlockInfo(size_t id)
	{
		return _existingAllocations[static_cast<size_t>(GetIndexOfId(id))];
	}

	bool CheckIfIdExists(size_t id)
	{
		for (const auto& ea : _existingAllocations)
		{
			if (ea.dataId == id)
			{
				return true;
			}
		}
		return false;
	}

	int GetIndexOfId(size_t id)
	{
		for (int i = 0; i < static_cast<int>(_existingAllocations.size()); i++)
		{
			if (_existingAllocations[static_cast<size_t>(i)].dataId == id)
			{
				return i;
			}
		}

		return -1;
	}

	void FreeExistingBlock(size_t id)
	{
		int index = GetIndexOfId(id);

		if (index != -1)
		{
			ExistingBlocks removedAllocation = _existingAllocations[static_cast<size_t>(index)];
			void* dataPtr = removedAllocation.dataPointer;
			_pAllocator->Free(dataPtr);

			if (_existingAllocations.size() - 1 == 0)
			{
				_existingAllocations.clear();
			}
			else
			{
				std::vector<ExistingBlocks> remainingAllocations;
				remainingAllocations.reserve(_existingAllocations.size() - 1);

				for (const auto ea : _existingAllocations)
				{
					if (ea.dataId != removedAllocation.dataId)
					{
						remainingAllocations.push_back(ea);
					}
				}

				_existingAllocations.clear();
				_existingAllocations.reserve(remainingAllocations.size());
				_existingAllocations = std::move(remainingAllocations);
			}
		}
	}

	void FreeAllExistingBlocks()
	{
		for (const auto& al : _existingAllocations)
		{
			_pAllocator->Free(al.dataPointer);
		}
		_existingAllocations.clear();
	}

	void InitialiseAllocator(const size_t elementSize, const size_t elementCount)
	{
		_pAllocator = std::make_unique<MemoryAllocator>(elementSize, elementCount);
		_pAllocator->Init();
	}

	std::unique_ptr<MemoryAllocator> _pAllocator;
	std::vector<ExistingBlocks> _existingAllocations;

	//Used in the addition of objects later on to know how much to allocate
	size_t _totalObjectCount = 0;
};