#include "utils.h"
#include <conio.h>
#include <windows.h>

#include "TaskManager.h"

void mySleep(uint32_t milliseconds) {
	ULONGLONG t = GetTickCount64();
	while (GetTickCount64() < t + milliseconds) {
		yield();
	}
}

std::string readString()
{
	std::string result;
	for (;;)
	{
		if (_kbhit())
		{
			char c = static_cast<char>(_getch_nolock());
			if (c == '\r')
			{
				return result;
			}
			_putch_nolock(c);
			result += c;
		}
		yield();
	}
}
