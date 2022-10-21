#include "Log.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace clf
{
	std::shared_ptr<spdlog::logger> Log::logger = nullptr;
	Log::Log() 
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		logger = spdlog::stdout_color_mt("CLIFFINE");
	}
}