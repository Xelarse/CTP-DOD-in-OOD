#pragma once
#include "MemorySystems.hpp"


/*
	Used in init of the AllmanVariable.
	Its used to determine related variables.
	if two AllmanVariables share the same HashID 
	they are related and will be located spatially in memory.
*/
template<class T>
size_t AllmanHash(T toHash)
{
	static std::hash<T> hasher;
	return hasher(toHash);
}


/*
	The AllmanVariable is a frontend for the developer to interact with the MemoryManager.
	Each instance of AllmanVariable is a wrapper for a variable and some extra features.
	Those extra features leverage some of Memory Managers metadata.
	This metadata is used to leverage some data oriented features such as iterating over data with the same HashID.
*/
template<class T>
class AllmanVariable
{
public:

	////---------- Constructors and Destructors ----------////

	AllmanVariable() = delete;
	AllmanVariable(MemoryManager* manRef, const size_t& hashId) : _managerRef(manRef), _hashId(hashId)
	{
		Initialise();
		_ptrToBase = reinterpret_cast<T*>(_managerRef->GetBlockInfo(_hashId).dataPointer);
	}

	AllmanVariable(MemoryManager* manRef, const size_t& hashId, const T& rhs) : _managerRef(manRef), _hashId(hashId)
    {
		Initialise();
		if (_ptrToVar != nullptr) { *_ptrToVar = rhs; }
		_ptrToBase = reinterpret_cast<T*>(_managerRef->GetBlockInfo(_hashId).dataPointer);
	}

	~AllmanVariable()
	{
		//Currently cannot free the related data as its stored linearly in the manager
		_ptrToVar = nullptr;
		_ptrToBase = nullptr;
		_managerRef = nullptr;
	}

	////---------- Variable related Getter and Setter ----------////

	T & Get() const { return *_ptrToVar; }
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
	unsigned int GetLength() { return _managerRef->GetBlockInfo(_hashId).currentCount; }

	//Returns the stride, just incase you wanted to scrub via Char* instead of the types Ptr
	/*
		Returns the stride required to move from one variable to the next in the alloc'd block
		The block is the one related to the inputted HashID
	*/
	unsigned int GetStride() { return _managerRef->GetBlockInfo(_hashId).stride; }

private:

	/*
		Called on construction of this class.
		Gets a ptr from the Memory manager to store its data at.
		The location of the ptr in memory is related to the HashID.
		Vars with the same HashID are stored locally.
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
