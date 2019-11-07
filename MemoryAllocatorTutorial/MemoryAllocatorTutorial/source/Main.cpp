#include <iostream>
#include <cassert>
#include "MemoryAllocator.h"

// Carry on from here
// http://dmitrysoshnikov.com/compilers/writing-a-memory-allocator/?fbclid=IwAR1riXugjt6f8_VB2V37c-nkgj-kDvBwJIHz2Gg_rDXBzpxor68fvKUhmvg#best-fit-search

int main()
{
	MemoryAllocator* memAllocator = new MemoryAllocator(512);
	memAllocator->Init();

	//Test case alignment, a request for 10 bytes should be aligned to 16 or 12 for x86 / 32 bit build
	void* newDataPointer = memAllocator->Allocate(10);
	MemoryAllocator::MemoryBlock* newBlockPointer = memAllocator->GetBlockHeader(newDataPointer);
	size_t blocksize = newBlockPointer->dataSize;
	std::cout << newBlockPointer->dataSize << std::endl;

	//Test case alignment a request for 8 bytes shouldnt change at all and stay 8
	void* newDataPointer2 = memAllocator->Allocate(8);
	MemoryAllocator::MemoryBlock* newBlockPointer2 = memAllocator->GetBlockHeader(newDataPointer2);
	blocksize = newBlockPointer2->dataSize;
	std::cout << newBlockPointer2->dataSize << std::endl;

	//Test case for block reuse gonna make another block and free the first one
	memAllocator->Free(newDataPointer);
	memAllocator->Free(newDataPointer2);
	void* newDataPointer3 = memAllocator->Allocate(8);
	std::cout << "Data Pointer 1: " << newDataPointer << std::endl << "Data Pointer 2: " << newDataPointer2 << std::endl << "Data Pointer 3: " << newDataPointer3 << std::endl;
	std::cout << newDataPointer3 << " == Should equal == " << newDataPointer2 << std::endl;

	//Test case to see if block coalesses
	void* newDataPointer4 = memAllocator->Allocate(10);
	std::cout << "Data pointer 4: " << newDataPointer4;
}