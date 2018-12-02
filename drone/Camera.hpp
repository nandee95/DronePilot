#pragma once

#include <memory>
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>

#include "escapi.h"

class CameraException
{
public:
	enum : uint8_t
	{
		Error_Unknown = 0,
		Error_NoDeviceFound,
		Error_AlreadyInUse,
		Error_CallbackNotSet,
		Error_AlreadyConnected,
		Error_AlreadyDisconnected,
		Error_DeviceNotFound,
	};
	CameraException(uint8_t err, std::string message = "")
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

class Camera
{
public:
	enum EventType
	{
		Event_Connected,
		Event_Disconnected,
		Event_Frame
	};
private:
	std::shared_ptr<std::thread> thread;
	uint8_t device=0;
	std::string friendlyname;
	uint32_t timeout = 500;

	std::function<void(EventType)> callback;
	bool connected = false;


	SimpleCapParams capture;

public:
	Camera()
	{
	}

	~Camera()
	{
		if (thread && thread->joinable()) thread->join();
	}

	static const int32_t Init()
	{
		return setupESCAPI();
	}

	static uint32_t FindDevice(std::string friendlyname)
	{
		const uint32_t total = countCaptureDevices();
		char name[64];
		for (uint32_t d = 0; d < total; d++)
		{
			getCaptureDeviceName(d, name, sizeof(name));
			if (std::string(name) == friendlyname)
			{
				return d;
			}
		}
		throw CameraException(CameraException::Error_DeviceNotFound, "Device not found");
		return -1;
	}

	const void SetCallback(std::function<void(EventType)> func)
	{
		callback = func;
	}

	const void SetTimeout(const uint32_t value)
	{
		timeout = value;
	}

	const uint32_t& GetTimeout() const
	{
		return timeout;
	}

	const void Connect(const uint32_t cam,const sf::Vector2i resolution)
	{
		device = cam;
		if (!callback)
		{
			throw CameraException(CameraException::Error_CallbackNotSet, "Callback not set!");
		}

		if (connected)
		{
			throw CameraException(CameraException::Error_AlreadyConnected, "Already connected!");
		}

		int devices = countCaptureDevices();

		if (devices == 0 || device > devices)
		{
			throw CameraException(CameraException::Error_NoDeviceFound, "No capture device found!");
		}

		capture.mWidth = resolution.x;
		capture.mHeight = resolution.y;
		capture.mTargetBuf = new int[800 * 600];
				
		if (!initCapture(device, &capture))
		{
			throw CameraException(CameraException::Error_AlreadyInUse, "Device is already in use!");
		}

		char name[64];
		
		getCaptureDeviceName(device, name, sizeof(name));
		friendlyname = name;

		connected = true;
		if (thread) thread->detach();
		thread = std::make_shared<std::thread>(&Camera::CameraThread, this);
		callback(Event_Connected);
	}

	const sf::Vector2i GetResolution() const
	{
		return sf::Vector2i(capture.mWidth,capture.mHeight);
	}

	const void Disconnect()
	{
		if (!connected)
		{
			throw CameraException(CameraException::Error_AlreadyDisconnected, "Already disconnected!");
		}
		connected = false;
		deinitCapture(device);

		if (thread && thread->joinable()) thread->join();
	}

	const uint8_t* GetFrame() const
	{
		return (uint8_t*)capture.mTargetBuf;
	}

	const bool CheckFriendlyName()
	{
		static char name[64];
		getCaptureDeviceName(device, name, sizeof(name));
		return name == friendlyname;
	}

	static const void CameraThread(Camera* t)
	{
		sf::Clock lastFrame;

		doCapture(t->device);
		while (lastFrame.getElapsedTime().asMilliseconds() < 5000 && !isCaptureDone(t->device))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		while (t->connected)
		{
			if (!t->CheckFriendlyName())
			{
				t->connected = false;
				deinitCapture(t->device);
				t->callback(Event_Disconnected);
				break;
			}
			lastFrame.restart();

			doCapture(t->device);
			while (!isCaptureDone(t->device)  && t->connected)
			{
				if (lastFrame.getElapsedTime().asMilliseconds() > t->timeout)
				{
					t->connected = false;
					deinitCapture(t->device);
					t->callback(Event_Disconnected);
				}
				else std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			if (t->connected) 
				t->callback(Event_Frame);
		}
	}

	const bool& IsConnected()
	{
		return connected;
	}


};