#pragma once
#include "Structs/DArray.h"

namespace Clf
{
	class Event
	{
	public:
		struct Args {};
		using Listener = bool(*)(Args);

	private:
		static DArray<Event> eventQueue;

	public:
		DArray<Listener> listeners;

		const bool Fire(const Args& args);
	};
}