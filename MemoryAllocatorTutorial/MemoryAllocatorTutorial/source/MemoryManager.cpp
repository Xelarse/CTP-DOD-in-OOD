#include "..\includes\MemoryManager.h"

MemoryManager::MemoryManager(size_t poolSize) : _pAllocator(std::make_unique<MemoryAllocator>(poolSize))
{
	_pAllocator->Init();
}

MemoryManager::~MemoryManager()
{
}
