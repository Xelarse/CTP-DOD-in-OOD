#pragma once
#include "MemoryAllocator.h"
#include <vector>

class MemoryManager
{
public:
	MemoryManager() = delete;
	MemoryManager(size_t poolSize);
	~MemoryManager();

	//If there is data thats already stored using the dataId then return the current offset for that data
	template<typename T>
	T* StoreData(std::string id, T data)
	{
		//Check if we already have like data stored and if we do get that block of data, add the variable to it then increase thats blocks offset for the next potential variable to be added
		for (auto& ea : _existingAllocations)
		{
			if (id == ea.dataId)
			{
				MemoryAllocator::MemoryBlock* memBlock = _pAllocator->GetBlockHeader(ea.dataPointer);
				size_t baseDataPointer = reinterpret_cast<size_t>(memBlock->data);
				size_t pointerLoc = baseDataPointer + memBlock->currentOffset;
				//Check to make sure adding a new variable will still be in range of this data block
				if (pointerLoc < baseDataPointer + memBlock->dataSize)
				{
					T* dataPointer = reinterpret_cast<T*>(pointerLoc);
					dataPointer = data;
					memBlock->currentOffset += sizeof(T);
					return dataPointer;
				}
			}
		}

		//If there wasnt a block found mem alloc a new block of memory and for storing it and add it to the existing allocations
		void* dataPtr = _pAllocator->Allocate(sizeof(T));
		_existingAllocations.push_back(ExistingData(id, dataPtr));
		T* castedDataPtr = reinterpret_cast<T*>(dataPtr);
		castedDataPtr = data;
		return castedDataPtr;
	}

private:
	struct ExistingData
	{
		ExistingData(std::string _id, void* _ptr)
		{
			dataId = _id;
			dataPointer = _ptr;
		}

		std::string dataId;
		void* dataPointer;
	};

	std::unique_ptr<MemoryAllocator> _pAllocator;
	std::vector<ExistingData> _existingAllocations;
};