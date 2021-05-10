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

#include <iostream>
#include <string>
#include <conio.h>

#include "TaskManager.h"


//std::string readString();

void __stdcall t1(void* data)
{
	std::cout << "Start T1" << std::endl;
	std::cout << static_cast<const char*>(data) << std::endl;
	for (auto i = 0; i < 6; i++)
	{
		std::cout << "In T1 " << i << std::endl;
		yield();
	}
	std::cout << "End T1" << std::endl;
}

void  __stdcall t2(void* data)
{
	std::cout << "Start T2" << std::endl;
	//auto s = readString();
	//std::cout << std::endl;
	//std::cout << s << std::endl;
	std::cout << static_cast<const char*>(data) << std::endl;
	for (auto i = 0; i < 5; i++)
	{
		std::cout << "In T2 " << i << std::endl;
		yield();
	}
	std::cout << "End T2" << std::endl;
}

void __stdcall t4(void* data);

void __stdcall t3(void* data)
{
	std::cout << "Start T3" << std::endl;
	yield();
	TaskManager::addTask(t4, reinterpret_cast<void*>(5));
	std::cout << static_cast<const char*>(data) << std::endl;
	std::cout << "End T3" << std::endl;
}

void __stdcall t4(void* data)
{
	std::cout << "Start T4" << std::endl;
	yield();
	if (data) t4(reinterpret_cast<void*>(reinterpret_cast<int>(data) - 1));
	std::cout << "End T4" << std::endl;
}

//std::string readString()
//{
//	std::string result;
//	for (;;)
//	{
//		if (_kbhit()) 
//		{
//			char c = static_cast<char>(_getch_nolock());
//			if (c == '\r')
//			{
//				return result;
//			}
//			_putch_nolock(c);
//			result += c;
//		}
//		yield();
//	}
//}

int main()
{
	// Enqueue tasks
	TaskManager::addTask(t1, (void*)"Hello from task1");
	TaskManager::addTask(t2, (void*)"Hi there!");
	TaskManager::addTask(t3, (void*)"Task 3 is here");

	// run
	TaskManager::start();
	return 0;
}