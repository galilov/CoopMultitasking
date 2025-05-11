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
#include "FiberDescriptor.h"

extern "C" MemAddr * lowLevelEnqueueFiber(void(__stdcall *)(void*), void*, MemAddr*); // defined in .asm

FiberDescriptor::FiberDescriptor(void(__stdcall *fiber)(void*), void* data)
{
	const size_t nStackEntries = 16384;
	// allocate nStackEntries of MemAddr for a local fiber stack 
	_stack.resize(nStackEntries);
	// fill the stack with pre-defined pattern
	for (auto& elem : _stack)
	{
		elem = static_cast<MemAddr>(0xdeadbeeff00dULL); // DeadBeefFood for debug
	}
	_stackPointer = lowLevelEnqueueFiber(fiber, data, &_stack[0] + _stack.size());
}
