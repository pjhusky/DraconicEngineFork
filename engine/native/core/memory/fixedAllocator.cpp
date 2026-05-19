module;

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>

module core.memory.fixedAllocator;

namespace draco::memory::fixed
{
	void init(FixedAllocator *alloc, Slice block)
	{
		alloc->buffer = (uint8_t *)block.data;
		alloc->size = block.size;
		alloc->allocated = false;
	}

	Error alloc(
		Allocator alloc,
		Slice *dst,
		size_t size,
		size_t align
	)
	{
		FixedAllocator *allocData = (FixedAllocator*)alloc.allocatorData;
		size_t alignMask = align - 1;
		size_t alignedSize = allocData->size - (
			(align - (((uintptr_t)allocData->buffer) & alignMask))
			& alignMask
		);
		assert(std::popcount(align) == 1);
		if (allocData->allocated | (alignedSize < size))
		{
			return Error::OutOfMemory;
		}
		dst->data = (void *)(
			((uintptr_t)&(allocData->buffer[alignMask])) & ~alignMask
		);
		dst->size = alignedSize;
		allocData->allocated = true;
		return Error::Okay;
	}

	Error freeAll(Allocator alloc)
	{
		FixedAllocator *allocData = (FixedAllocator*)alloc.allocatorData;
		allocData->allocated = false;
		return Error::Okay;
	}
}
