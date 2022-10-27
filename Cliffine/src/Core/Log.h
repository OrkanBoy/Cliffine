#pragma once

namespace Clf
{
	class Log
	{
	public:
		enum Level
		{
			LOG_LEVEL_FATAL = 0,
			LOG_LEVEL_ERROR = 1,
			LOG_LEVEL_WARN = 2,
			LOG_LEVEL_INFO = 3,
			LOG_LEVEL_DEBUG = 4,
			LOG_LEVEL_TRACE = 5,
			
			LOG_LEVEL_MAX = 6
		};

		static void Init() {}
		static void Deinit() {}

		static void Out(Level level, const char* format, ...);
	};
}

#define CLF_FATAL(...) Clf::Log::Out(Clf::Log::LOG_LEVEL_FATAL, __VA_ARGS__)
#define CLF_ERROR(...) Clf::Log::Out(Clf::Log::LOG_LEVEL_ERROR, __VA_ARGS__)
#define CLF_WARN(...)  Clf::Log::Out(Clf::Log::LOG_LEVEL_WARN, __VA_ARGS__)
#define CLF_INFO(...)  Clf::Log::Out(Clf::Log::LOG_LEVEL_INFO, __VA_ARGS__)
#define CLF_DEBUG(...) Clf::Log::Out(Clf::Log::LOG_LEVEL_DEBUG, __VA_ARGS__)
#define CLF_FATAL(...) Clf::Log::Out(Clf::Log::LOG_LEVEL_FATAL, __VA_ARGS__)