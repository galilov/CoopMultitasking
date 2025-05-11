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

#include "FiberManager.h"
#include "utils.h"


void __stdcall t5(void* data);

void __stdcall t1(void* data)
{
	std::cout << "Start T1:";
	std::cout << static_cast<const char*>(data) << std::endl;
	for (auto i = 0; i < 10; i++)
	{
		mySleep(1000);
		std::cout << "T1:" << i << std::endl;
	}
	std::cout << "End T1" << std::endl;
}

void  __stdcall t2(void* data)
{
	std::cout << "Start T2:";
	std::cout << static_cast<const char*>(data) << std::endl;
	for (auto i = 0; i < 10; i++)
	{
		mySleep(500);
		std::cout << "T2:" << i << std::endl;
	}
	std::cout << "End T2" << std::endl;
}

void __stdcall t3(void* data)
{
	std::cout << "Start T3:";
	std::cout << static_cast<const char*>(data) << std::endl;
	for (auto i = 0; i < 10; i++)
	{
		mySleep(2000);
		std::cout << "T3:" << i << std::endl;
	}
	std::cout << "End T3" << std::endl;
}

void __stdcall t4(void* data)
{
	std::cout << "Start T4:";
	std::cout << static_cast<const char*>(data) << std::endl;
	for (auto i = 0; i < 10; i++)
	{
		mySleep(100);
		std::cout << "T4:" << i << std::endl;
	}
	FiberManager::addFiber(t5, nullptr); // schedule a new fiber from another fiber
	std::cout << "End T4" << std::endl;
}

void __stdcall t5(void* data) {
	std::cout << "Start T5, Enter a text line and press Enter: ";
	std::string s = readString();
	std::cout << std::endl;
	std::cout << s << std::endl;
	std::cout << "DONE" << std::endl;
}

int main()
{
	// Enqueue tasks
	FiberManager::addFiber(t1, (void*)"Hello from task1");
	FiberManager::addFiber(t2, (void*)"Hi there!");
	FiberManager::addFiber(t3, (void*)"Fiber 3 is here");
	FiberManager::addFiber(t4, (void*)"Fiber 4!!!!");

	// run
	FiberManager::start();
	std::cout << "***Exit***" << std::endl;
	return 0;
}