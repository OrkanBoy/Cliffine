#include "App.h"
#include "Asserts.h"
#include "Mem.h"

#include "Core/Event.h"

namespace Clf
{
	App* App::instance = nullptr;

	void App::Init() {
		CLF_ASSERT(!instance, "Client Application already exists!");
		Mem::Init();
		instance = Create();

		Platform::Init(
			instance->name,
			instance->x, instance->y,
			instance->width, instance->height);
		Log::Init();
	}

	void App::Deinit() {
		CLF_ASSERT(instance, "Trying do destroy absent Client Application.");
		instance->~App();

		Log::Deinit();
		Platform::Deinit();
		Mem::Deinit();
	}

	void App::Run() {
		Mem::OutAllocs();

		while (true)
		{
			Platform::PollEvents();

			for ()
			{

			}
			Event::eventQueue--;
		}
	}

	
}