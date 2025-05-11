#pragma once

#include "FiberDescriptor.h"

namespace FiberManager
{
	void addFiber(void (__stdcall *fiber)(void*), void* data);
	void start();
}
