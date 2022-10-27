#include "Mem.h"
#include "Platform/Platform.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

namespace Clf
{
	u64 Mem::allocs[MEM_TYPE_MAX] {};
	u64 Mem::totalAllocs = 0;

	void Mem::Init() {
	}

	void Mem::Deinit() {
	}

	void* Mem::Alloc(Type type, u64 bytes) {
		if (type == MEM_TYPE_UNKNOWN)
			CLF_WARN("Allocating unknown memory type.");

		allocs[type] += bytes;
		totalAllocs += bytes;
		return Platform::AllocMem(bytes);
	}

	void Mem::Free(Type type, void* block, u64 bytes) {
		if (type == MEM_TYPE_UNKNOWN)
			CLF_WARN("Deallocating unknown memory type.");

		allocs[type] -= bytes;
		totalAllocs -= bytes;
		Platform::FreeMem(block);
	}

	void Mem::OutAllocs() {
		constexpr const char* MemTypeStrings[MEM_TYPE_MAX] {
			"UNKNOWN",
			"GAME   ",
			"STRING ",
			"DARRAY ",
			"LIST   ",
			"QUEUE  "
		};

		constexpr u32 KiB = 1024;
		constexpr u32 MiB = KiB * 1024;
		constexpr u32 GiB = MiB * 1024;

		char out[800] = "Memory Usage:\n";
		char offset = strlen(out);
		for (u8 memType = 0; memType < MEM_TYPE_MAX; memType++) {
			
			char unit[4] = "BiB";
			f32 amount = allocs[memType];
			if (allocs[memType] > GiB) {
				unit[0] = 'G';
				amount /= GiB;
			}
			else if (allocs[memType] > MiB) {
				unit[0] = 'M';
				amount /= MiB;
			}
			else if (allocs[memType] > KiB) {
				unit[0] = 'K';
				amount /= KiB;
			}
			else unit[1] = '\0';

			u32 len = sprintf(out + offset, "\t%s: %.2f%s\n", MemTypeStrings[memType], amount, unit);
			offset += len;
		}

		CLF_INFO(out);
	}
}