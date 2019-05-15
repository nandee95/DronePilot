#pragma once
#include <thread>
#include <memory>
#include <string>
#include <vector>
#include <functional> 
#include <Windows.h>
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <mutex>
#include <chrono>

#include <iostream>

class SerialPortException
{
public:
	enum : uint8_t
	{
		Error_Unknown = 0,
		Error_PortNotFound,
		Error_PortInUse,
		Error_CommStateFail,
		Error_CommTimeoutFail,
		Error_CallbackNotSet,
		Error_AlreadyConnected,
		Error_AlreadyDisconnected,
		Error_NotConnected,
		Error_FailedToWrite
	};
	SerialPortException(uint8_t err, std::string message = "")
	{
		this->err = err;
		this->message = message;
	}

	const std::string& Message() const
	{
		return message;
	}

	const uint8_t& Error() const
	{
		return err;
	}
private:
	uint8_t err;
	std::string message;
};

class SerialPort
{
public:

	enum BaudRate
	{
		Baud_9600 = 9600,
		Baud_19200 = 19200,
		Baud_38400 = 38400,
		Baud_57600 = 57600,
		Baud_115200 = 115200
	};

	enum EventType
	{
		Event_Connected,
		Event_Disconnected,
		Event_Data,
	};

	typedef std::function<void(EventType, uint8_t*, size_t)> CallbackType;

	SerialPort()
	{

	}

	~SerialPort()
	{
		if (connected)
			Disconnect();
	}

	const void SetCallback(CallbackType func)
	{
		callback = func;
	}

	const void Connect(uint8_t comport, BaudRate baud = Baud_9600)
	{
		if (connected)
			throw SerialPortException(SerialPortException::Error_AlreadyConnected, "This serialport is already connected!");

		if (!callback)
			throw SerialPortException(SerialPortException::Error_CallbackNotSet, "Callback not set!");

		port = CreateFile(std::string("COM" + std::to_string(comport)).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (port == INVALID_HANDLE_VALUE)
		{
			switch (GetLastError())
			{
			case 2: throw SerialPortException(SerialPortException::Error_PortNotFound, "Serial port not found!");
			case 5: throw SerialPortException(SerialPortException::Error_PortInUse, "Serial port is in use!");
			default:  throw SerialPortException(SerialPortException::Error_Unknown, "Unknown error! (#" + std::to_string(GetLastError()) + ")");
			}
		}

		DCB dcb;
		ZeroMemory(&dcb, sizeof(DCB));
		dcb.DCBlength = sizeof(DCB);

		if (!GetCommState(port, &dcb))
		{
			CloseHandle(port);
			throw SerialPortException(SerialPortException::Error_CommStateFail, "Failed to get comm state!");
		}

		dcb.BaudRate = CBR_9600;
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;

		if (!SetCommState(port, &dcb))
		{
			CloseHandle(port);
			throw SerialPortException(SerialPortException::Error_CommStateFail, "Failed to set comm state!");
		}

		COMMTIMEOUTS touts;

		if (!GetCommTimeouts(port, &touts))
		{
			CloseHandle(port);
			throw SerialPortException(SerialPortException::Error_CommTimeoutFail, "Failed to get comm timeout!");
		}

		//Need to be as close as real time data as possible
		touts.ReadIntervalTimeout = 1;
		touts.ReadTotalTimeoutConstant = 1;
		touts.ReadTotalTimeoutMultiplier = 1;
		touts.ReadTotalTimeoutMultiplier = 1;
		touts.WriteTotalTimeoutMultiplier = 1;

		if (!SetCommTimeouts(port, &touts))
		{
			CloseHandle(port);
			throw SerialPortException(SerialPortException::Error_CommTimeoutFail, "Failed to get comm timeout!");
		}


		connected = true;
		if (thread)
		{
			if (thread->joinable())
				thread->join();
			thread.reset();
		}

		thread = std::make_shared<std::thread>(&SerialPort::SerialPortThread, this);
		callback(Event_Connected, nullptr, 0);
	}

	const void Disconnect()
	{
		if (!connected)
		{
			throw SerialPortException(SerialPortException::Error_AlreadyDisconnected, "Already disconnected!");
		}

		connected = false;
		CloseHandle(port);
		if (thread && thread->joinable()) thread->join();
		callback(Event_Disconnected, nullptr, 0);
	}

	uint64_t SendString(const std::string data)
	{
		if (!connected)
			throw SerialPortException(SerialPortException::Error_NotConnected, "Serial port is not connected");

		DWORD written;
		std::lock_guard<std::mutex> gu(mu);
		if (WriteFile(port, data.c_str(), data.size(), &written, NULL) == FALSE)
			throw SerialPortException(SerialPortException::Error_FailedToWrite, "Failed to write to serial port");

		return written;
	}

	uint64_t SendByteArray(const void* data, const size_t length)
	{
		if (!connected)
			throw SerialPortException(SerialPortException::Error_NotConnected, "Serial port is not connected");

		DWORD written;
		std::lock_guard<std::mutex> gu(mu);
		if (WriteFile(port, data, length, &written, NULL) == FALSE)
			throw SerialPortException(SerialPortException::Error_FailedToWrite, "Failed to write to serial port");
		return written;
	}

	static std::vector<uint8_t> GetAvailablePorts()
	{
		std::vector<uint8_t> result;
		TCHAR target[5000];
		for (int p = 0; p < 256; p++)
		{
			if (QueryDosDevice(std::string("COM" + std::to_string(p)).c_str(), (LPSTR)target, 5000))
			{
				result.push_back(p);
			}
		}

		return result;
	}

	const bool& IsConnected()
	{
		return connected;
	}

private:
	static void SerialPortThread(SerialPort * t)
	{
		while (t->connected)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			std::lock_guard<std::mutex> gu(t->mu);
			if (ReadFile(t->port, &t->buffer, 256, (LPDWORD)& t->bufferLen, NULL)) {
				if (t->bufferLen != 0) t->callback(Event_Data, t->buffer, t->bufferLen);
			}
			else break;
		}

		if (t->connected)
		{
			t->connected = false;
			CloseHandle(t->port);
			t->callback(Event_Disconnected, nullptr, 0);
		}
	}

	CallbackType callback;
	std::shared_ptr<std::thread> thread;
	bool connected = false;
	HANDLE port;
	uint8_t buffer[256];
	size_t bufferLen;

	std::mutex mu;
};