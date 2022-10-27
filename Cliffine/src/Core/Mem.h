#pragma once
#include "Defines.h"
#include "Platform/Platform.h"

namespace Clf
{
	class Mem
	{
	public:
		enum Type
		{
			MEM_TYPE_UNKNOWN,
			MEM_TYPE_APP,
			MEM_TYPE_STRING,
			MEM_TYPE_DARRAY,
			MEM_TYPE_MAX
		};
		
	private:
		static u64 allocs[MEM_TYPE_MAX];
		static u64 totalAllocs;

	public:
		static void Init();
		static void Deinit();
		static void OutAllocs();

		static void* Alloc(Type type, u64 bytes);
		static void Free(Type type, void* block, u64 bytes);
		static void* Zero(void* block, u64 bytes) { return Platform::ZeroMem(block, bytes); }
		static void* Copy(void* dst, const void* src, u64 bytes) { return Platform::CopyMem(dst, src, bytes); }
		static void* Set(void* dst, i32 value, u64 bytes) { return Platform::SetMem(dst, value, bytes); }
	};
}