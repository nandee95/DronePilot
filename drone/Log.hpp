#pragma once
#include <ctime>
#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <sys/stat.h>
#include <Windows.h>
#include <SFML/System.hpp>

static class Log
{
private:
	static std::mutex mu;
	static HANDLE console;

	static std::string filename;
	static constexpr int32_t color_red = 12;
	static constexpr int32_t color_white = 15;
	static constexpr int32_t color_yellow = 14;
	static constexpr int32_t color_green = 10;
public:
	static const void Init()
	{
		std::time_t t = std::time(0);
		struct tm now;
		localtime_s(&now, &t);
		std::string date = std::to_string(now.tm_year + 1900) +"_"+ zpad(now.tm_mon + 1) +"_"+ zpad(now.tm_mday);
		CreateDirectory("logs",NULL);
		int32_t index=1;
		struct stat fstat;
		do
		{
			filename="logs/"+date+"_"+zpad(index++,5)+".log";
		} while (stat(filename.c_str(), &fstat) == 0);

		console = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	static const void Error(const std::string text)
	{
		_Log("Error", color_red, text);
	}

	static const void Info(const std::string text)
	{
		_Log("Info", color_white, text);
	}
	static const void Warning(const std::string text)
	{
		_Log("Warning", color_yellow, text);
	}
	static const void Success(const std::string text)
	{
		_Log("Success", color_green, text);
	}

private:

	static const std::string zpad(const int32_t num,const int8_t len=2)
	{
		std::string result(std::to_string(num));
		while (result.length() < len)
		{
			result = "0" + result;
		}
		return result;
	}

	static const void _Log(const std::string level, const int32_t color, const std::string& text)
	{
		std::lock_guard<std::mutex> gu(mu);
		std::ofstream fs(filename.c_str());
		std::time_t t = std::time(0);
		struct tm now;
		localtime_s(&now,&t);
		std::string date = std::to_string(now.tm_year + 1900) + ". " + zpad(now.tm_mon + 1) + ". " + zpad(now.tm_mday) + ". "+
			zpad(now.tm_hour)+":"+zpad(now.tm_min)+":"+zpad(now.tm_min)+":"+zpad(now.tm_sec);

		SetConsoleTextAttribute(console, color);

		std::cout << "[" << level << "]" << " " << date << " - " << text << std::endl;
		fs << "[" << level << "]" << " " << date << " - " << text << std::endl;
	}

};


std::mutex Log::mu;
std::string Log::filename;
HANDLE Log::console;