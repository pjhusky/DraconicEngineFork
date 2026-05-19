module;

#include <cstddef>

export module core.memory.pageAllocator;
export import core.memory.allocator;
export import core.memory.slice;

export namespace draco::memory
{
	namespace page
	{
		Error alloc(Allocator alloc, Slice *dst, size_t size, size_t align);

		Error free(Allocator alloc, Slice block);

		AllocatorVTbl pageAllocatorVtbl = {
			.alloc = alloc,
			.free = free,
			.freeAll = nilFreeAll,
		};
		Allocator pageAllocator = {
			.vtbl = &pageAllocatorVtbl,
			.allocatorData = nullptr,
		};
	}
}
