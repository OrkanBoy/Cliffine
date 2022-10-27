#include "Log.h"
#include "Platform/Platform.h"
#include "Defines.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

namespace Clf
{
	void Log::Out(Level level, const char* message, ...) {
		constexpr const char* levelStrings[LOG_LEVEL_MAX] = { "[FATAL]", "[ERROR]", "[WARN] ", "[INFO] ", "[DEBUG]:", "[TRACE]:" };
		constexpr u32 MAX_LEN = 32000;

		char out[MAX_LEN];
		sprintf(out, "%s", levelStrings[level]);

		va_list args;
		va_start(args, message);
		vsnprintf(out + strlen(*levelStrings), MAX_LEN, message, args);
		va_end(args);

		Platform::ConsoleOut(out, level);
	}
}