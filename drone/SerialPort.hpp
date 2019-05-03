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

#include <SFML/Network/Packet.hpp>

#include "Protocol.hpp"

class SerialPortException
{
public:
	enum : uint8_t
	{
		Error_Unknown=0,
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
		Baud_9600=9600,
		Baud_19200=19200,
		Baud_38400=38400,
		Baud_57600=57600,
		Baud_115200=115200
	};

	enum EventType
	{
		Event_Connected,
		Event_Disconnected,
		Event_Data,
	};

	SerialPort()
	{
		
	}

	~SerialPort()
	{
		if(connected)
			Disconnect();
	}

	const void SetCallback(std::function<void(EventType)> func)
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
		callback(Event_Connected);
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
		callback(Event_Disconnected);
	}

	const uint8_t* GetData() const
	{
		return buffer;
	}

	const size_t& GetDataSize() const
	{
		return buffer_len;
	}

	const sf::Packet GetPacket()
	{
		sf::Packet packet;
		packet.append(buffer, buffer_len);
		return packet;
	}


	static const uint8_t FindDevice()
	{
		SP_DEVINFO_DATA data;
		data.cbSize = sizeof(data);

		HDEVINFO devInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS,NULL,nullptr,DIGCF_PRESENT);
		if (devInfo == INVALID_HANDLE_VALUE)
		{
			return 0;
		}

		int d = 0;
		while (SetupDiEnumDeviceInfo(devInfo,d++,&data))
		{
			DWORD regDataType;
			DWORD size = 0;

			// find the size required to hold the device info
			SetupDiGetDeviceRegistryProperty(devInfo, &data, SPDRP_HARDWAREID, nullptr, nullptr, NULL, &size);
			BYTE* hardwareId = new BYTE[(size > 1) ? size : 1];
			// now store it in a buffer
			if (SetupDiGetDeviceRegistryProperty(devInfo, &data, SPDRP_HARDWAREID, &regDataType, hardwareId, sizeof(hardwareId) * size, nullptr))
			{
				// find the size required to hold the friendly name
				size = 0;
				SetupDiGetDeviceRegistryProperty(devInfo, &data, SPDRP_FRIENDLYNAME, nullptr, nullptr, 0, &size);
				BYTE* friendlyName = new BYTE[(size > 1) ? size : 1];
				// now store it in a buffer
				if (!SetupDiGetDeviceRegistryProperty(devInfo, &data, SPDRP_FRIENDLYNAME, nullptr, friendlyName, sizeof(friendlyName) * size, nullptr))
				{
					// device does not have this property set
					memset(friendlyName, 0, size > 1 ? size : 1);
				}
				// use friendlyName here
				delete[] friendlyName;
			}
			delete[] hardwareId;
		}
	}

	uint64_t SendString(const std::string data)
	{
		if (!connected)
			throw SerialPortException(SerialPortException::Error_NotConnected, "Serial port is not connected");
		
		DWORD written;
		if (!WriteFile(port, data.data(), data.length(), &written, NULL))
			throw SerialPortException(SerialPortException::Error_FailedToWrite,"Failed to write to serial port");
		return written;
	}

	uint64_t SendByteArray(const void* data, const size_t length)
	{
		if (!connected)
			throw SerialPortException(SerialPortException::Error_NotConnected, "Serial port is not connected");

		DWORD written;
		if (!WriteFile(port, data, length, &written, NULL))
			throw SerialPortException(SerialPortException::Error_FailedToWrite, "Failed to write to serial port");
		return written;
	}
	uint64_t SendPacket(const sf::Packet& packet)
	{
		if (!connected)
			throw SerialPortException(SerialPortException::Error_NotConnected, "Serial port is not connected");

		DWORD written;
		if (!WriteFile(port, packet.getData(), packet.getDataSize(), &written, NULL))
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
	static void SerialPortThread(SerialPort* t)
	{
		while (t->connected)
		{
			if (ReadFile(t->port, &t->buffer, 256, (LPDWORD)&t->buffer_len, NULL))
			{
				t->callback(Event_Data);
			}
			else
			{
				break;
			}
		}

		if (t->connected)
		{
			t->connected = false;
			CloseHandle(t->port);
			t->callback(Event_Disconnected);
		}
	}

	std::function<void(EventType)> callback;
	std::shared_ptr<std::thread> thread;
	bool connected = false;
	HANDLE port;
	uint8_t buffer[256];
	size_t buffer_len;
};