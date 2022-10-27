#pragma once
#include "Log.h"

#define CLF_ASSERTS

#ifdef CLF_ASSERTS
	#if _MSC_VER
		#include <intrin.h>
		#define CLF_DEBUG_BREAK() __debugbreak();
	#else
		#define CLF_DEBUG_BREAK() __builtin_trap();
	#endif

	#define CLF_ASSERT(expr, message)\
	{\
		if (expr);\
		else\
		{\
			Clf::Log::Out(Clf::Log::LOG_LEVEL_MAX, "Assertion Failure: %s\n\t Message: %s\n\t File: %s\n\t Line: %d\n", #expr, message, __FILE__, __LINE__);\
			CLF_DEBUG_BREAK();\
		}\
	}

#else
	#define CLF_DEBUG_BREAK()
#endif			