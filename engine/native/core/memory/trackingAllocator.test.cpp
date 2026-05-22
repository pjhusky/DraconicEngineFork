#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

import core.memory;
import core.memory.slice;
import core.stdtypes;

TEST_CASE("Tracking allocator basic functions")
{
	using namespace draco::memory;
	bump::BumpAllocator bumpAlloc;
	tracking::TrackingAllocator trackingAlloc;
	tracking::Analytics analytics;
	tracking::AllocationDetails details[2];
	Allocator alloc;
	Slice aSlice;
	Slice bSlice;
	Slice cSlice;
	Error err;
	bump::init(&bumpAlloc, page::pageAllocator);
	bump::asAllocator(&alloc, &bumpAlloc);
	tracking::init(&trackingAlloc, alloc);
	tracking::asAllocator(&alloc, &trackingAlloc);
	err = alloc.alloc(&aSlice, 5, 1);
	REQUIRE(err == Error::Okay);
	err = alloc.alloc(&bSlice, 8, 8);
	REQUIRE(err == Error::Okay);
	err = alloc.alloc(&cSlice, 4, 4);
	REQUIRE(err == Error::Okay);

	REQUIRE(tracking::getActiveAllocations(trackingAlloc, 0, nullptr) == 3);
	alloc.free(bSlice);
	REQUIRE(tracking::getActiveAllocations(trackingAlloc, 2, details) == 2);
	REQUIRE(details[1].data.data == aSlice.data);
	REQUIRE(details[0].data.data == cSlice.data);
	tracking::getAnalytics(trackingAlloc, &analytics);
	REQUIRE(analytics.totalAllocatedBytes >= 9);
	REQUIRE(analytics.activeAllocationsCount == 2);
	alloc.freeAll();
	tracking::getAnalytics(trackingAlloc, &analytics);
	REQUIRE(analytics.activeAllocationsCount == 0);
	bump::deinit(&bumpAlloc);
}
