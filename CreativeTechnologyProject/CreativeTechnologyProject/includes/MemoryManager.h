#pragma once
#include "MemoryAllocator.h"
#include <vector>

class MemoryManager
{
public:
	//This allows the variable to access the existing data struct for referencing data blocks
	friend class AAVariable;

	inline static void InitialiseManager(const size_t elementSize, const size_t elementCount)
	{
		if (MemoryManager::_pInstance != nullptr)
		{
			return;
		}
		_pInstance = new MemoryManager();
		_pInstance->_totalObjectCount = elementCount;
		_pInstance->InitialiseAllocator(elementSize, elementCount);
	}

	inline static void ReleaseManager()
	{
		if (_pInstance != nullptr)
		{
			delete _pInstance;
			_pInstance = nullptr;
		}
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

	MemoryManager() = default;
	~MemoryManager()
	{
		FreeAllExistingBlocks();
	}

	template <typename T>
	T* InitialiseVariable(const char* id)
	{
		//First check if an id exists, if it doesnt, create it
		if (!CheckIfIdExists(id))
		{
			InitialiseNewMemoryBlock(id, _totalObjectCount);
		}

		auto& existingBlock = _existingAllocations[GetIndexOfId(id)];

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
	T* InitialiseVariable(const char* id, T data)
	{
		T* outPtr = InitialiseVariable(id);
		if (outPtr != nullptr)
		{
			*outPtr = data;
		}
		return outPtr;
	}

	//Initilises the memory block and returns the base ptr
	template<typename T>
	T* InitialiseNewMemoryBlock(const char* id, unsigned int maxCapacity)
	{
		void* dataPtr = _pAllocator->Allocate(sizeof(T) * maxCapacity);
		_existingAllocations.push_back(ExistingBlocks(id, dataPtr, sizeof(T), maxCapacity));
		return reinterpret_cast<T*>(dataPtr);
	}

	const ExistingBlocks& GetBlockInfo(const char* id)
	{
		return _existingAllocations[GetIndexOfId(id)];
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

	int GetIndexOfId(const char* id)
	{
		for (int i = 0; i < _existingAllocations.size(); i++)
		{
			if (_existingAllocations[i].dataId == id)
			{
				return i;
			}
		}

		return -1;
	}

	void FreeExistingBlock(const char* id)
	{
		int index = GetIndexOfId(id);

		if (index != -1)
		{
			ExistingBlocks removedAllocation = _existingAllocations[index];
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

	static MemoryManager* _pInstance;
	std::unique_ptr<MemoryAllocator> _pAllocator;
	std::vector<ExistingBlocks> _existingAllocations;

	//Used in the addition of objects later on to know how much to allocate
	size_t _totalObjectCount = 0;
};

MemoryManager* MemoryManager::_pInstance = nullptr;