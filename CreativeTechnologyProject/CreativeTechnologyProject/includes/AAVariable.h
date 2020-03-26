#pragma once
#include <string>
#include "MemoryManager.h"

namespace AA
{
	template<class T, std::string& key>
	class Variable
	{
	public:

		Variable()
		{
			Initialise();
		}

		Variable(const T& rhs)
		{
			Initialise();
			if (_ptrToVar != nullptr)
			{
				*_ptrToVar = rhs;
			}
		}

		~Variable()
		{

		}

		//Setting and getting the var
		inline T operator = (const T& rhs) const
		{
			if (_ptrToVar != nullptr)
			{
				*_ptrToVar = rhs;
			}
		}

		inline T& Get()
		{
			return *_ptrToVar;
		}

		//Returns the base ptr to the alloc'd block, use this along with get length to scrub through the data
		inline static const T* GetBasePtr()
		{
			if (MemoryManager::_pInstance != nullptr)
			{
				return reinterpret_cast<T*>(MemoryManager::_pInstance->GetBlockInfo(key.c_str()).dataPointer);
			}
		}

		//Returns the current amount allocated under the given KEY
		inline static const unsigned int GetLength()
		{
			if (MemoryManager::_pInstance != nullptr)
			{
				return MemoryManager::_pInstance->GetBlockInfo(key.c_str()).currentCount;
			}
		}

		//Returns the stride, just incase you wanted to scrub via Char* instead of the types Ptr
		inline static const unsigned int GetStride()
		{
			if (MemoryManager::_pInstance != nullptr)
			{
				MemoryManager::_pInstance->GetBlockInfo(key.c_str()).stride;
			}
		}

	private:

		void Initialise()
		{
			MemoryManager* inst = MemoryManager::_pInstance;
			if (inst != nullptr)
			{
				_ptrToVar = inst->InitialiseVariable<T>(key.c_str());
			}
		}

		T* _ptrToVar = nullptr;
	};
}