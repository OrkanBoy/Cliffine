#pragma once
#include "Defines.h"
#include "Structs/String.h"

#include <string>
namespace Clf
{
	class App
	{
	public:
		static App* instance;
		const char* name;
		i32 x, y;
		u32 width, height;

		virtual ~App() {}

		static void Init();
		static void Deinit();

		static void Run();

		static App* Create();
	};
}