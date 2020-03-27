#pragma once
#include "MemoryManager.h"

template<class T>
size_t AAHash(T toHash)
{
	static std::hash<T> hasher;
	return hasher(toHash);
}

template<class T>
class AAVariable
{
public:
	AAVariable() = delete;
	AAVariable(const size_t& hashId) : _hashId(hashId)
	{
		Initialise();
	}

	AAVariable(const size_t& hashId, const T& rhs) : _hashId(hashId)
	{
		Initialise();
		if (_ptrToVar != nullptr)
		{
			*_ptrToVar = rhs;
		}
	}

	~AAVariable()
	{

	}

	T& Get()
	{
		return *_ptrToVar;
	}

	//Returns the base ptr to the alloc'd block, use this along with get length to scrub through the data
	T* GetBasePtr()
	{
		if (MemoryManager::_pInstance != nullptr)
		{
			return reinterpret_cast<T*>(MemoryManager::_pInstance->GetBlockInfo(_hashId).dataPointer);
		}
	}

	//Returns the current amount allocated under the given KEY
	const unsigned int GetLength()
	{
		if (MemoryManager::_pInstance != nullptr)
		{
			return MemoryManager::_pInstance->GetBlockInfo(_hashId).currentCount;
		}
	}

	//Returns the stride, just incase you wanted to scrub via Char* instead of the types Ptr
	const unsigned int GetStride()
	{
		if (MemoryManager::_pInstance != nullptr)
		{
			MemoryManager::_pInstance->GetBlockInfo(_hashId).stride;
		}
	}

private:

	void Initialise()
	{
		if (MemoryManager::_pInstance != nullptr)
		{
			_ptrToVar = MemoryManager::_pInstance->InitialiseVariable<T>(_hashId);
		}
	}

	T* _ptrToVar = nullptr;
	const size_t _hashId;
};