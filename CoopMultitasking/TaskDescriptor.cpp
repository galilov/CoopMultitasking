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
#include "TaskDescriptor.h"

extern "C" MemAddr * __stdcall lowLevelEnqueueTask(void(*)(void*), void*, MemAddr*);

TaskDescriptor::TaskDescriptor(void(__stdcall* task)(void*), void* data)
{
	const size_t nStackEntries = 16384;
	// allocate nStackEntries of MemAddr for a local task stack 
	_stack.resize(nStackEntries);
	for (auto& elem : _stack)
	{
		elem = static_cast<MemAddr>(0xdeadbeeff00dULL); // DeadBeefFood for debug
	}
	_stackPointer = lowLevelEnqueueTask(task, data, &_stack[0] + _stack.size());
}
