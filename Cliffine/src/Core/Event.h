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
		
		static const Queue<Event*>& GetQueue() { return queue; }

	private:
		static Queue<Event*> queue;

		DArray<bool(*)(Args)> listeners{};
		void operator()();
	
	public:
		Args args;
		Event(const DArray<Listener>& listeners) : listeners(listeners){}
		void Queue(Args args);
		friend class App;
	};
}