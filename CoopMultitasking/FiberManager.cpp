// Copyright(c) 2021, 2025 Alexander Galilov, alexander.galilov@gmail.com
// This code is a part of my cooperative multitasking (fibers) DEMO for x64 C++ (Visual Studio 2019)
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

#include "FiberDescriptor.h"

extern "C" void onFiberFinished();              // called from .asm
extern "C" void fiberManagerYield(MemAddr*);    // called from .asm
extern "C" void lowLevelResume(MemAddr*);       // defined in .asm
extern "C" MemAddr* lowLevelGetCurrentStack();  // defined in .asm

// Pointer to MAIN stack
static MemAddr* _mainSp;

namespace FiberManager {
	typedef std::unique_ptr<FiberDescriptor> FiberDescritporPtr;
	typedef std::list<FiberDescritporPtr> Descriptors;

	Descriptors _fibers;
	FiberDescritporPtr _finishedFiber;
	Descriptors::iterator _itFiber; // points to a current fiber

	void addFiber(void(__stdcall* fiber)(void*), void* data)
	{
		_fibers.emplace_back(std::make_unique<FiberDescriptor>(fiber, data));
	}

	void start()
	{
		_mainSp = nullptr;
		_itFiber = _fibers.begin(); // Select the first fiber
		// Run the first fiber from MAIN stack context.
		// The stack will be switched to a local fiber-related stack.
		yield(); // it returns back to start() when all the fibers will finish.
		_finishedFiber.reset();
	}
}

// This function is called from ASM code as a fiber completion.
// It should ALWAYS call lowLevelResume()
void onFiberFinished()
{
	using namespace FiberManager;
	// Currently, completing fiber is ALWAYS the owner of the current stack. But we mus
	assert((*_itFiber)->isOwnerOfStack(lowLevelGetCurrentStack()));
	// Avoid of auto-destruction the FiberDescriptor by saving it to
	// finishedTask shared pointer. We need this stack to be allocated
	// because it is current stack we are working with right now.
	_finishedFiber.reset(_itFiber->release());
	// Remove completed fiber from the list.
	_itFiber = _fibers.erase(_itFiber);

	MemAddr* sp;
	if (_fibers.empty())
	{
		// Prepare the final completion, we will return the control
		// from yield() to start() function. See yield() call in start().
		sp = _mainSp;
	}
	else
	{
		// Switch to the next fiber.
		if (_itFiber == _fibers.end())
		{
			_itFiber = _fibers.begin();
		}
		sp = (*_itFiber)->getStackPointer();
	}
	lowLevelResume(sp); // it doesn't return control!
	assert(false);
}

// This function is called from ASM code yield().
// @param sp - current stack pointer after all the required CPU 
// registers have been pushed to stack.
void fiberManagerYield(MemAddr* sp)
{
	using namespace FiberManager;
	if (_fibers.empty()) // No fibers in the list?
	{
		// No fibers to switch to, just return back to yield().
		return;
	}
	// Does the current fiber own the stack pointed by sp?
	if ((*_itFiber)->isOwnerOfStack(sp))
	{
		// Save current stack pointer to the fiber descriptor
		(*_itFiber)->saveStackPointer(sp);
		if (_fibers.size() > 1)
		{
			// Select the next fiber.
			if (++_itFiber == _fibers.end()) // Is the last fiber?
			{
				// Go to first.
				_itFiber = _fibers.begin();
			}
		}
	}
	else
	{
		// Execution goes here ONCE, when yield() is first called from the
		// MAIN stack context. We have only one such place in the start() function.
		assert(_mainSp == nullptr);
		// Save MAIN stack pointer to use it in the final completion.
		_mainSp = sp;
	}
	// Switch to the selected fiber using its own stack.
	lowLevelResume((*_itFiber)->getStackPointer());
	assert(false); // The execution must never go here!
}
