#include "..\includes\MemoryManager.h"

MemoryManager::MemoryManager(const size_t elementSize, const size_t elementCount) : _pAllocator(std::make_unique<MemoryAllocator>(elementSize, elementCount))
{
	_pAllocator->Init();
}

MemoryManager::~MemoryManager()
{
}