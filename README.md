# Cooperative multitasking DEMO for x64 C++ (Visual Studio 2019).
Example:
```c++
#include <iostream>
#include <string>
#include <windows.h>
#include <processthreadsapi.h>

#include "FiberManager.h"
#include "utils.h"

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
```
## Output
```
+Fiber1:10140 5
-Fiber2:10140 0
-Fiber2:10140 1
-Fiber2:10140 2
+Fiber1:10140 4
-Fiber2:10140 3
-Fiber2:10140 4
-Fiber2:10140 5
+Fiber1:10140 3
-Fiber2:10140 6
+Fiber1:10140 2
-Fiber2:10140 7
-Fiber2:10140 8
-Fiber2:10140 9
+Fiber1:10140 1
+Fiber1:10140 0
***Exit***
```
