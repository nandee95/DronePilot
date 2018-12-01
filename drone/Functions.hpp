#pragma once

#include <Windows.h>

static struct Functions
{
	static float Sign(float value)
	{
		return value > 0 ? 1.0f : -1.0f;
	}

	template <typename T>
	static T Min(const T a, const T b)
	{
		return a > b ? b : a;
	}

	template <typename T>
	static T Max(const T a, const T b)
	{
		return a < b ? b : a;
	}
	
	static void PopupMessage(std::string message, std::string title)
	{
		MessageBox(NULL, message.c_str(), title.c_str(), MB_OK);
	}

	uint8_t CharToHalfByte(char ch)
	{
		if (ch >= '0' && ch < '9') return ch - '0';
		return 10 + (ch - 'A');
	}

	uint8_t CharsToByte(char ch1,char ch2)
	{

	}
};