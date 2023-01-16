# Cooperative multitasking DEMO for x64 C++ (Visual Studio 2019).
Video (russian): https://youtu.be/cEGOAZtpfYA

Example:
```c++
        #include <iostream>
        #include "TaskManager.h"

        void  __stdcall t1(void* data)
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
                std::cout << static_cast<const char*>(data) << std::endl;
                for (auto i = 0; i < 5; i++)
                {
                        std::cout << "In T2 " << i << std::endl;
                        yield();
                }
                std::cout << "End T2" << std::endl;
        }

        void  __stdcall t3(void* data)
        {
                std::cout << "Start T3" << std::endl;
                yield();
                std::cout << static_cast<const char*>(data) << std::endl;
                std::cout << "End T3" << std::endl;
        }
        
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
```
## Output
```
Start T1
Hello from task1
In T1 0
Start T2
Hi there!
In T2 0
Start T3
In T1 1
In T2 1
Task 3 is here
End T3
In T1 2
In T2 2
In T1 3
In T2 3
In T1 4
In T2 4
In T1 5
End T2
End T1
```
