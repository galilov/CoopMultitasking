#pragma once

#include "TaskDescriptor.h"

namespace TaskManager
{
	void addTask(void (__stdcall *task)(void*), void* data, const char* name);
	void start();
}
