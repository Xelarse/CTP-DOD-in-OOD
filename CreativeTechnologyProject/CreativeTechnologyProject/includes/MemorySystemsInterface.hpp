#pragma once
#include "MemorySystems.hpp"

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

	////---------- Constructors and Destructors ----------////

	AAVariable() = delete;
	AAVariable(MemoryManager* manRef, const size_t& hashId) : _hashId(hashId), _managerRef(manRef)
	{
		Initialise();
		_ptrToBase = reinterpret_cast<T*>(_managerRef->GetBlockInfo(_hashId).dataPointer);
	}

	AAVariable(MemoryManager* manRef, const size_t& hashId, const T& rhs) : _hashId(hashId), _managerRef(manRef)
	{
		Initialise();
		if (_ptrToVar != nullptr) { *_ptrToVar = rhs; }
		_ptrToBase = reinterpret_cast<T*>(_managerRef->GetBlockInfo(_hashId).dataPointer);
	}

	~AAVariable()
	{
		//Currently cannot free the related data as its stored linearly in the manager
		T* _ptrToVar = nullptr;
		T* _ptrToBase = nullptr;
		MemoryManager* _managerRef = nullptr;
	}

	////---------- Variable related Getter and Setter ----------////

	T& Get() { return *_ptrToVar; }
	void Set(const T& var) { if (_ptrToVar != nullptr) *_ptrToVar = var; }

	////---------- Memory Manager Meta-data Accessing Functions ----------////
	/*
		Returns the Ptr to the base of the block of data
		The block is the one related to the inputted HashID
	*/
	T* GetBasePtr() { return _ptrToBase; }

	/*
		Returns the total count for all variables in the alloc'd block
		The block is the one related to the inputted HashID
	*/
	const unsigned int GetLength() { return _managerRef->GetBlockInfo(_hashId).currentCount; }

	//Returns the stride, just incase you wanted to scrub via Char* instead of the types Ptr
	/*
		Returns the stride required to move from one variable to the next in the alloc'd block
		The block is the one related to the inputted HashID
	*/
	const unsigned int GetStride() { return _managerRef->GetBlockInfo(_hashId).stride; }

private:

	/*
		Call on construction of this class
		Gets a ptr from the Memory manager to store its data at
		The location of the ptr in memory is related to the HashID
		Vars with the same HashID are stored locally
	*/
	void Initialise()
	{
		_ptrToVar = _managerRef->InitialiseVariable<T>(_hashId);
	}

	T* _ptrToVar = nullptr;
	T* _ptrToBase = nullptr;
	MemoryManager* _managerRef = nullptr;
	const size_t _hashId;
};