#pragma once

#include <spdlog/spdlog.h>

namespace clf
{
	class Log
	{
	public:
		Log();
		static std::shared_ptr<spdlog::logger> GetLogger() { return logger; }

	private:
		static std::shared_ptr<spdlog::logger> logger;
	};
}
