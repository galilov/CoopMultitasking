// Copyright(c) 2021 Alexander Galilov, alexander.galilov@gmail.com
// This code is a part of my cooperative multitasking DEMO for x64 C++ (Visual Studio 2019)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files(the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright noticeand this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
#include <cassert>
#include <iostream>
#include <list>

#include "TaskDescriptor.h"

extern "C" void __stdcall onTaskFinished();
extern "C" void __stdcall taskManagerYield(MemAddr*);
extern "C" void __stdcall lowLevelResume(MemAddr*);
extern "C" MemAddr * __stdcall lowLevelGetCurrentStack();

namespace TaskManager {
	typedef std::shared_ptr<TaskDescriptor> TaskDescritporPtr;
	typedef std::list<TaskDescritporPtr> Descriptors;
	
	Descriptors _activeTasks, _finishedTasks;
	Descriptors::iterator _it;

	void addTask(void (__stdcall *task)(void*), void* data, const char* name)
	{
		_activeTasks.push_back(std::make_shared<TaskDescriptor>(task, data, name));
	}

	void start()
	{
		_it = _activeTasks.begin();
		// Run the first task from MAIN stack context.
		// The stack will be switched to a local task-related stack.
		yield();
		std::cout << "Completed\n"; 
	}
}

// Pointer to MAIN stack
static MemAddr* _mainSp = nullptr;

// This function is called from ASM code as a task completion.
// It should ALWAYS call lowLevelResume()
void __stdcall onTaskFinished()
{
	using namespace TaskManager;
	//size_t n = _activeTasks.size();
	bool finished = false;
	auto* sp = lowLevelGetCurrentStack();
	if ((*_it)->isOwnerOfStack(sp))
	{
		// 
		_finishedTasks.push_back(*_it);
		_it = _activeTasks.erase(_it);
		finished = true;
	}
	if (!finished || _activeTasks.empty())
	{
		// Final completion using saved MAIN stack pointer
		_it = _activeTasks.begin();
		lowLevelResume(_mainSp);
	}
	else
	{
		if (_it == _activeTasks.end())
		{
			_it = _activeTasks.begin();
		}
		sp = (*_it)->getStackPointer();
		lowLevelResume(sp);
	}
}


void __stdcall taskManagerYield(MemAddr* sp)
{
	using namespace TaskManager;
	if (_activeTasks.size() < 2)
	{
		return;
	}
	if ((*_it)->isOwnerOfStack(sp))
	{
		(*_it)->saveStackPointer(sp);
		++_it;
		if (_it == _activeTasks.end())
		{
			_it = _activeTasks.begin();
		}
	}
	else
	{
		// The execution goes here ONE time when the very first task
		// is started from MAIN stack context.
		assert(_mainSp == nullptr);
		// Save MAIN stack pointer to use it in final completion.
		_mainSp = sp;
	}

	lowLevelResume((*_it)->getStackPointer());
	assert(false); // the execution should not go here!
}
