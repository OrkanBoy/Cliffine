#include "Event.h"

namespace Clf
{
	Queue<Event*> Event::queue;

	void Event::operator()() {
		for (u32 i = 0; i < listeners.GetLen(); i++)
			if (listeners[i](args))
				return;
	}

	void Event::Queue(Args args) {
		this->args = args;
		Event::queue += this;
	}
}