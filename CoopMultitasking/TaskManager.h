#pragma once

#include "TaskDescriptor.h"

namespace TaskManager
{
	void addTask(void (*task)(void*), void* data, const char* name);
	void start();
}
