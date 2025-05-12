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

#include <iostream>
#include <string>
#include <windows.h>
#include <processthreadsapi.h>

#include "FiberManager.h"
#include "utils.h"


void __stdcall f5(void* data);

void __stdcall f1(void* data)
{
	std::cout << "Start F1:";
	std::cout << static_cast<const char*>(data) << std::endl;
	for (auto i = 0; i < 10; i++)
	{
		mySleep(1000);
		std::cout << "F1:" << i << std::endl;
	}
	std::cout << "End F1" << std::endl;
}

void  __stdcall f2(void* data)
{
	std::cout << "Start F2:";
	std::cout << static_cast<const char*>(data) << std::endl;
	for (auto i = 0; i < 10; i++)
	{
		mySleep(500);
		std::cout << "F2:" << i << std::endl;
	}
	std::cout << "End F2" << std::endl;
}

void __stdcall f3(void* data)
{
	std::cout << "Start F3:";
	std::cout << static_cast<const char*>(data) << std::endl;
	for (auto i = 0; i < 10; i++)
	{
		mySleep(2000);
		std::cout << "F3:" << i << std::endl;
	}
	std::cout << "End F3" << std::endl;
}

void __stdcall f4(void* data)
{
	std::cout << "Start F4:";
	std::cout << static_cast<const char*>(data) << std::endl;
	for (auto i = 0; i < 10; i++)
	{
		mySleep(100);
		std::cout << "F4:" << i << std::endl;
	}
	FiberManager::addFiber(f5, nullptr); // schedule a new fiber from another fiber
	//yield();
	std::cout << "End F4" << std::endl;
}

void __stdcall f5(void* data) {
	std::cout << "Start F5, Enter a text line and press Enter: ";
	std::string s = myReadString();
	std::cout << std::endl;
	std::cout << s << std::endl;
	std::cout << "End F5" << std::endl;
}

void __stdcall fiber1(void* data)
{
	for (auto i = 5; i >= 0; --i)
	{
		std::cout << "+Fiber1:" << ::GetCurrentThreadId() << " " << i << std::endl;
		mySleep(300);
	}
}

void __stdcall fiber2(void* data)
{
	for (auto i = 0; i < 10; i++)
	{
		std::cout << "-Fiber2:" << ::GetCurrentThreadId() << " " << i << std::endl;
		mySleep(100);
	}
}

int main()
{
	// register our fibers
	FiberManager::addFiber(fiber1, nullptr);
	FiberManager::addFiber(fiber2, nullptr);
	// run
	FiberManager::start();
	// done
	std::cout << "***Exit***" << std::endl;
	return 0;
}



// Enqueue tasks
//FiberManager::addFiber(f1, (void*)"Hello from task1");
//FiberManager::addFiber(f2, (void*)"Hi there!");
//FiberManager::addFiber(f3, (void*)"Fiber 3 is here");
//FiberManager::addFiber(f4, (void*)"Fiber 4!!!!");
