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

#pragma once

#include <vector>
#include <memory>
#include <string>

extern "C" void __stdcall yield();

class TaskDescriptor;

typedef std::shared_ptr<TaskDescriptor> TaskDescriptorPtr;
typedef uint64_t MemAddr;

class TaskDescriptor
{
public:
	TaskDescriptor(const TaskDescriptor&) = delete;
	TaskDescriptor& operator=(const TaskDescriptor&) = delete;
	TaskDescriptor(void(__stdcall* task)(void*), void* data);
	bool isOwnerOfStack(const MemAddr* sp) { return (sp >= &_stack[0]) && (sp < &_stack[0] + _stack.size()); }
	void saveStackPointer(MemAddr* sp) { _stackPointer = sp; }
	MemAddr* getStackPointer() const { return _stackPointer; }
private:
	std::vector<MemAddr> _stack;
	MemAddr* _stackPointer;
};
