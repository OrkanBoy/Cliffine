#include "Event.h"

namespace Clf
{
	const bool Event::Fire()
	{
		for (u32 i = 0; i < listeners.GetLen(); i++)
			if (listeners[i](args))
				return true;
		return false;
	}
}