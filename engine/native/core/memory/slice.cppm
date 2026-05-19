module;

#include <cstddef>

export module core.memory.slice;

export namespace draco::memory
{
	struct Slice
	{
		void *data;
		size_t size;
	};
}
