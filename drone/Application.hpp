#pragma once

#include "SerialPort.hpp"
#include "Camera.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "Hud_Compass.hpp"
#include "Hud_Status.hpp"
#include "Hud_Altitude.hpp"
#include "Hud_Roll.hpp"
#include "Hud_Messages.hpp"
#include "Hud_Canvas.hpp"
#include "Protocol.hpp"

#include "CfgFile.hpp"
#include "Log.hpp"

#include "ScopedPtr.hpp"


class Application
{
	SerialPort sp;
	ScopedPtr<Hud_Compass> hud_compass;
	Hud_RF s_rf;
	Hud_CAM s_cam;
	ScopedPtr<Hud_Altitude> hud_altitude;
	ScopedPtr<Hud_Roll> hud_roll;
	ScopedPtr<Hud_Messages> hud_messages;
	ScopedPtr<Hud_Canvas> canvas;

	unsigned char throttle=0;

	bool running = true;

	//Config
	CfgFile cfg;
	sf::Vector2i resolution = sf::Vector2i(800, 600);
	sf::Vector2i camResolution = sf::Vector2i(800, 600);
	float hud_scale = 1.0;
	bool fullscreen = false;

	//Serial
	std::shared_ptr<std::thread> reconnect_serial;
	std::shared_ptr<std::thread> comm;

	//Ping
	sf::Clock lastPing;
	sf::Clock pingTimer;
	uint16_t ping=1000;


	//Camera
	bool camenabled = false;
	Camera cam;
	std::shared_ptr<std::thread> reconnect_camera;
public:
	const void LoadConfig()
	{
		Log::Info("Loading config file...");
		try
		{
			const std::function<bool(std::string)> log_validator = [] (std::string value)->bool {
				return value == "all" || value == "none" || value == "warning" || value == "error";
			};
			//Overwrite with file
			cfg.LoadFromFile("config/config.ini", {
				{ "Remote",{
					{ "Handshake",{ CfgFile::RegexValidator("[A-F0-9]{32}"),"00000000000000000000000000000000" } },
					{ "BaudRate",{ CfgFile::IntListValidator({ 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600 }),"115200" } },
					{ "FriendlyName",{ CfgFile::AnyValidator, "???" } }
				} },
				{ "Camera",{
					{ "Enabled",{ CfgFile::BoolValidator ,"False" } },
					{ "FriendlyName",{ CfgFile::AnyValidator, "???" } },
					{ "Resolution",{ CfgFile::ResolutionValidator, "800x600" } },
				} },
				{ "Log",{
					{ "Console",{ log_validator, "all" } },
					{ "Hud",{ log_validator, "all" } },
					{ "File",{ log_validator, "all" } }
				} }, 
				{ "Window",{
					{ "Title",{ CfgFile::AnyValidator, "Drone controller" } }
				} },
				{ "Hud",{
					{ "Scale",{ CfgFile::FloatRangeValidator(0.1,3.0), "1.0" } },
					{ "AltMax",{ CfgFile::FloatRangeValidator(0.0,10000.0), "500.0" } },
					{ "AltStep",{ CfgFile::FloatRangeValidator(0.0,10000.0), "50.0" } },
				} }
				});
		}
		catch (CfgFileException& e)
		{
			Log::Warning("Config file not found! Generating new one...");
			cfg.SaveToFile("config/config.ini");
		}

		camenabled = cfg.GetValue("Camera", "Enabled").ToBool();
		auto desktop = sf::VideoMode::getDesktopMode();
		resolution = { (int)desktop.width,(int)desktop.height };
		camResolution = cfg.GetValue("Camera", "Resolution").ToResolution();
		hud_scale = cfg.GetValue("Hud", "Scale").ToFloat();
		Log::Success("Config loaded successfully!");
	}

	const void InitCamera()
	{
		if (!camenabled)
		{
			Log::Warning("Camera disabled!");
			s_cam.SetStatus(0);
			s_cam.SetString("Disabled");
			return;
		}
		Log::Info("Initializing camera...");


		cam.SetCallback([&](Camera::EventType e) {
			switch (e)
			{
			case Camera::Event_Connected:
			{
				s_cam.SetStatus(1);
				s_cam.SetString("OK");
				Log::Success(std::string("Camera connected to : ")+cfg.GetValue("Camera","FriendlyName").ToString());
			} break;
			case Camera::Event_Disconnected:
			{
				s_cam.SetStatus(0);
				s_cam.SetString("Disconnected");
				canvas->Reset();

				Log::Warning("Camera disconnected!");
				if (reconnect_camera->joinable()) reconnect_camera->join();
				reconnect_camera = std::make_shared<std::thread>(Application::TryConnectCamera, this);
			} break;
			case Camera::Event_Frame:
			{
				canvas->Update(cam.GetFrame());
			} break;
			}
		});

		reconnect_camera = std::make_shared<std::thread>(Application::TryConnectCamera, this);
	}

	static const void Communication(Application* t)
	{
		return;
		sf::Uint64 ping_id = 0;

		while (t->running && t->sp.IsConnected())
		{
			sf::Clock clk;

			std::this_thread::sleep_for(std::chrono::microseconds(33333));
		}
	}

	const void InitSerial()
	{
		sp.SetCallback([&](SerialPort::EventType e,uint8_t* data,size_t len) {
			switch (e)
			{
			case SerialPort::Event_Connected:
			{
				s_rf.SetString("OK");
				s_rf.SetStatus(1);
				if (comm && comm->joinable()) comm->join();
				comm = std::make_shared<std::thread>(Application::Communication, this);
				/*

				sf::Packet handshake;

				uint8_t rand[32];
				const std::string& str = cfg.GetValue("NRF24L01", "Handshake").ToString();


				handshake << (uint8_t)Protocol_HandShake << rand;

				sp.SendPacket(handshake);*/
			} break;
			case SerialPort::Event_Disconnected:
			{
				s_rf.SetString("Disconnected");
				s_rf.SetStatus(0);
				if (reconnect_serial && reconnect_serial->joinable()) reconnect_serial->join();
				reconnect_serial = std::make_shared<std::thread>(Application::TryConnectSerial, this);
			} break;
			case SerialPort::Event_Data:
			{
				uint8_t protocol=data[0];
				std::cout << "Packet received on protocol: " << (int)protocol << std::endl;
				switch (protocol)
				{
				case Protocol::SensorReport:
				{
					Protocol_SensorReport* packet = (Protocol_SensorReport*)data;
					//hud_roll->Set(packet->gyro.X);
					std::cout << "roll X: " << packet->gyro.X << std::endl;
					std::cout << "roll Y: " << packet->gyro.Y << std::endl;
					std::cout << "roll Z: " << packet->gyro.Z << std::endl;
					for (int i = 0; i < len; i++)
					{
						std::cout << (int)data[i] << std::endl;
					}
				} break;
				}
				
			} break;
			}
		});

		reconnect_serial = std::make_shared<std::thread>(Application::TryConnectSerial, this);
	}

	Application()
	{
		Log::Init(hud_messages.ptr);
		Camera::Init();

		LoadConfig();
		InitSerial();
		InitCamera();
		InitHud();
	}

	~Application()
	{
		running = false;
		if (comm && comm->joinable())
			comm->join();
		if (reconnect_serial && reconnect_serial->joinable())
			reconnect_serial->join();
		if (reconnect_camera && reconnect_camera->joinable())
			reconnect_camera->join();
	}

	static const void TryConnectSerial(Application* t)
	{
		while (!t->sp.IsConnected() && t->running)
		{
			t->s_rf.SetStatus(2);
			t->s_rf.SetString("Connecting...");
			
			auto ports = SerialPort::GetAvailablePorts();
			if (ports.size() == 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
				t->s_rf.SetString("Disconnected");
				t->s_rf.SetStatus(0);
				std::this_thread::sleep_for(std::chrono::milliseconds(750));
				continue;
			}
			for (auto p : ports)
			{
				try
				{
					t->sp.Connect(p,(SerialPort::BaudRate)t->cfg.GetValue("Remote","BaudRate").ToInt());
				}
				catch (SerialPortException& e) {}

				if (t->sp.IsConnected()) break;
			}
			if (t->sp.IsConnected()) break;
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
			t->s_rf.SetString("Disconnected");
			t->s_rf.SetStatus(0);
			std::this_thread::sleep_for(std::chrono::milliseconds(750));
		}
	}

	static const void TryConnectCamera(Application* t)
	{
		while (!t->cam.IsConnected() && t->running)
		{
			t->s_cam.SetStatus(2);
			t->s_cam.SetString("Connecting...");

			try
			{
				t->cam.Connect(Camera::FindDevice(t->cfg.GetValue("Camera", "FriendlyName")),t->camResolution);
				break;
			}
			catch (CameraException& e)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
				t->s_cam.SetString(e.Message());
				t->s_cam.SetStatus(0);
				std::this_thread::sleep_for(std::chrono::milliseconds(750));
			}
		}
	}

	const void InitHud()
	{
		canvas = new Hud_Canvas(resolution, camResolution);
		hud_altitude = new Hud_Altitude(resolution, hud_scale, cfg.GetValue("Hud", "AltMax").ToFloat(), cfg.GetValue("Hud", "AltStep").ToFloat());
		hud_compass = new Hud_Compass(resolution,hud_scale);
		hud_roll = new Hud_Roll();
		hud_messages = new Hud_Messages();
	}

	const void SetResolution(sf::RenderWindow& window, sf::Vector2i resolution)
	{
		sf::FloatRect visibleArea(0, 0, resolution.x, resolution.y);
		window.setView(sf::View(visibleArea));

		hud_roll->UpdateResolution(resolution, 1.0);
		hud_messages->UpdateResolution(resolution, 1.0);
	}

	const int Run()
	{
		sf::RenderWindow window;
		{
			sf::ContextSettings settings;
			settings.antialiasingLevel = 8;
			settings.majorVersion = 1;
			settings.minorVersion = 2;

			auto desktop = sf::VideoMode::getDesktopMode();
			window.create(sf::VideoMode(800,600),cfg.GetValue("Window","Title").ToString(), sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize, settings);
		}
		Hud_InverseShader inverse(resolution,camResolution);
		inverse.states.texture = &canvas->GetTexture();

		s_rf.setPosition(20, 20);
		s_cam.setPosition(20, 40);
		//alt.setPosition(20, 580);
		sf::Event e;
		sf::Clock test;
		SetResolution(window, { 800,600 });
		ShowWindow(window.getSystemHandle(), SW_MAXIMIZE);

		while (window.isOpen())
		{
			while (window.pollEvent(e))
			{
				switch (e.type)
				{
				case sf::Event::Closed:
				{
					window.close();
					goto cleanup;
				} break;
				case sf::Event::Resized:
				{
					if (e.size.width < 800 || e.size.height < 600)
					{
						if (e.size.width < 800) e.size.width = 800;
						if (e.size.height < 600) e.size.height = 600;
						window.setSize({ e.size.width, e.size.height });
					}
				
					SetResolution(window, {static_cast<int32_t>(e.size.width),static_cast<int32_t>(e.size.height)});
				} break;
				case sf::Event::KeyPressed:
				{
					if (e.key.code == sf::Keyboard::F11)
					{
						if(!fullscreen)
							window.create({ static_cast<uint32_t>(resolution.x),  static_cast<uint32_t>(resolution.y) },
								cfg.GetValue("Window", "Title").ToString(), sf::Style::None);
						else
						{
							window.create({ static_cast<uint32_t>(resolution.x),  static_cast<uint32_t>(resolution.y) },
								cfg.GetValue("Window", "Title").ToString(), sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
							ShowWindow(window.getSystemHandle(), SW_MAXIMIZE);
						}
						fullscreen = !fullscreen;
					}
					else if (e.key.code == sf::Keyboard::Escape)
					{
						window.close();
					}
					if (e.key.code == sf::Keyboard::Space)
					{
						hud_messages->AddText("Test message " + std::to_string(rand() % 255),sf::Color::Red);
					}
				}
				}
			}

			inverse.shader.setUniform("camera", cam.IsConnected());

			hud_roll->Update();
			hud_compass->Update();
			hud_altitude->Update();
			hud_messages->Update();
			hud_compass->Set(180.0*(float)test.getElapsedTime().asMilliseconds() / 1000.0);
			hud_altitude->Set(500.0*std::fmod((float)test.getElapsedTime().asMilliseconds() / 1000.0,1.0));
			
			window.clear(sf::Color::Black);
			if(camenabled) window.draw(*canvas);

			window.draw(s_rf);
			window.draw(s_cam);
			window.draw(*hud_messages);
			window.draw(*hud_compass, inverse.states);
			window.draw(*hud_roll, inverse.states);
			
			window.draw(*hud_altitude, inverse.states);

			window.display();
		}
		cleanup:
		if (sp.IsConnected())
			sp.Disconnect();

		if (cam.IsConnected())
			cam.Disconnect();

		return EXIT_SUCCESS;
	}
};