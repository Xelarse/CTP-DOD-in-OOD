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
	T* InitliaseVariable(const char* id, T data)
	{
		//Check if theres already an existing block and if there is store it using that blocks paramters
		for (int i = 0; i < _existingAllocations.size(); i++)
		{
			auto existingAlloc = _existingAllocations[i];

			if (id == existingAlloc.dataId)
			{
				//Check if the allocation can actually fit and if it cant return null pointer
				if (existingAlloc.currentCount + 1 > existingAlloc.maxCount)
				{
					//TODO Add something here to enlargen the current block or make another block and join them
					return nullptr;
				}
				else
				{
					//Figure out how far to stride along from the base pointer, store T into it and return the pointer to it
					size_t baseBlockPointer = reinterpret_cast<size_t>(existingAlloc.dataPointer);
					size_t newDataPointer = baseBlockPointer + existingAlloc.stride * (existingAlloc.currentCount + 1);

					//Cast the newDataPointer to the desired variable, store the variable in it through dereferencing then finally return the casted ptr
					T* castedDataPointer = reinterpret_cast<T*>(newDataPointer);
					*castedDataPointer = data;
					existingAlloc.currentCount++;
					return castedDataPointer;
				}
			}
		}

		//If there wasn't an existing block found return nullptr
		return nullptr;
	}

	template<typename T>
	void* InitliaseNewMemoryBlock(const char* id, unsigned int maxCapacity)
	{
		void* dataPtr = _pAllocator->Allocate(sizeof(T) * maxCapacity);
		_existingAllocations.push_back(ExistingBlocks(id, dataPtr, sizeof(T), maxCapacity));
		return dataPtr;
	}

	bool CheckIfIdExists(const char* id)
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

private:
	struct ExistingBlocks
	{
		ExistingBlocks(const char* _id, void* _ptr, size_t _stride, unsigned int _capacity)
		{
			dataId = _id;
			currentCount = 0;
			maxCount = _capacity;
			stride = _stride;
			dataPointer = _ptr;
		}

		const char* dataId;
		unsigned int currentCount;
		unsigned int maxCount;
		size_t stride;
		void* dataPointer;
	};

	std::unique_ptr<MemoryAllocator> _pAllocator;
	std::vector<ExistingBlocks> _existingAllocations;
};