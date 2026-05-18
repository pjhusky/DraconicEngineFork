module;

#include <cassert>
#include <cstddef>
#include <cstdint>

export module core.memory.fixedAllocator;
export import core.memory.allocator;
export import core.memory.slice;

export namespace draco::memory
{
	namespace fixed
	{
		struct FixedAllocator
		{
			uint8_t *buffer;
			size_t size;
			bool allocated;
		};

		void init(FixedAllocator *alloc, Slice block);

		Error alloc(Allocator alloc, Slice *dst, size_t size, size_t align);

		Error freeAll(Allocator alloc);

		AllocatorVTbl fixedAllocatorVtbl = {
			.alloc = alloc,
			.free = nilFree,
			.freeAll = freeAll,
		};

		inline void asAllocator(Allocator *dst, FixedAllocator *alloc)
		{
			asAllocatorVoid(dst, (void*)alloc, &fixedAllocatorVtbl);
		}
	}
}
