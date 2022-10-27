#pragma once
#include "Structs/DArray.h"
#include "Structs/Queue.h"

namespace Clf
{
	//TODO: currently assume never fully full
	class Event
	{
	public:
		struct Args {};
		using Listener = bool(*)(Args);

		static Queue<Event> eventQueue;
		
		DArray<Listener> listeners;
		Args args;
		Event(DArray<Listener> listeners);
		
		const bool Fire();
	};
}