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
	AAVariable(MemoryManager* manRef, const size_t& hashId) : _hashId(hashId), _managerRef(manRef)
	{
		Initialise();
		_ptrToBase = reinterpret_cast<T*>(_managerRef->GetBlockInfo(_hashId).dataPointer);
	}

	AAVariable(MemoryManager* manRef, const size_t& hashId, const T& rhs) : _hashId(hashId), _managerRef(manRef)
	{
		Initialise();
		if (_ptrToVar != nullptr)
		{
			*_ptrToVar = rhs;
		}
		_ptrToBase = reinterpret_cast<T*>(_managerRef->GetBlockInfo(_hashId).dataPointer);
	}

	~AAVariable()
	{

	}

	T& Get()
	{
		return *_ptrToVar;
	}

	void Set(const T& var)
	{
		if (_ptrToVar != nullptr)
		{
			*_ptrToVar = var;
		}
	}

	//Returns the base ptr to the alloc'd block, use this along with get length to scrub through the data
	inline T* GetBasePtr()
	{
		return _ptrToBase;
	}

	//Returns the current amount allocated under the given KEY
	inline const unsigned int GetLength()
	{
		return _managerRef->GetBlockInfo(_hashId).currentCount;
	}

	//Returns the stride, just incase you wanted to scrub via Char* instead of the types Ptr
	inline const unsigned int GetStride()
	{
		return _managerRef->GetBlockInfo(_hashId).stride;
	}

private:
	void Initialise()
	{
		_ptrToVar = _managerRef->InitialiseVariable<T>(_hashId);
	}

	T* _ptrToVar = nullptr;
	T* _ptrToBase = nullptr;
	MemoryManager* _managerRef = nullptr;
	const size_t _hashId;
};