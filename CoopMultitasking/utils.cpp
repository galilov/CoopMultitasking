#include "utils.h"
#include <conio.h>
#include <windows.h>

#include "FiberManager.h"

void mySleep(uint32_t milliseconds) {
	// Retrieve the number of milliseconds that have elapsed since the system was started.
	// See https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-gettickcount64
	ULONGLONG t = GetTickCount64();
	while (GetTickCount64() < t + milliseconds) {
		yield();
	}
}

std::string myReadString()
{
	std::string result;
	for (;;)
	{
		if (_kbhit())
		{
			char c = static_cast<char>(_getch_nolock());
			if (c == '\r')
			{
				break;
			}
			_putch_nolock(c);
			result += c;
		}
		yield();
	}
	return result;
}
