#pragma once
#include "Defines.h"
#include "Core/Log.h"

#define CLF_PLATFORM_WIN32

#ifdef CLF_PLATFORM_WIN32
#include <windows.h>
#endif

namespace Clf
{
	class Platform
	{
	public:
#ifdef CLF_PLATFORM_WIN32
		static HINSTANCE instance;
		static HWND window;

		static u64 clockFrequency;
		static u64 startTime;

		static LRESULT CALLBACK OnEvent(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam);
#endif

		static void Init(const char* appName, i32 x, i32 y, u32 width, u32 height);
		static void Deinit();

		static void PollEvents();

		static void* AllocMem(u64 bytes);
		static void FreeMem(void* block);
		static void* ZeroMem(void* block, u64 bytes);
		static void* CopyMem(void* dst, const void* src, u64 bytes);
		static void* SetMem(void* dst, i32 value, u64 bytes);

		static void ConsoleOut(const char* message, Log::Level level);

		static u64 GetAbsTime();
		static void Sleep(u64 ms);
	};
}