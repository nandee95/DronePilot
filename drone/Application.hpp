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
#include "Hud_Canvas.hpp"
#include "Hud_TopBar.hpp"

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
	ScopedPtr<Hud_Canvas> canvas;
	ScopedPtr<Hud_TopBar> hud_topbar;

	unsigned char throttle=0;

	bool running = true;

	//Config
	CfgFile cfg;
	sf::Vector2i resolution = sf::Vector2i(800, 600);
	sf::Vector2i camResolution = sf::Vector2i(800, 600);
	float hud_scale = 1.0;

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
			cfg.LoadFromFile("config/drone.ini", {
				{ "NRF24_USB",{
					{ "Handshake",{ CfgFile::RegexValidator("[A-F0-9]{32}"),"00000000000000000000000000000000" } },
					{ "BaudRate",{ CfgFile::IntListValidator({ 110, 150, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600 }),"115200" } },
					{ "FriendlyName",{ CfgFile::AnyValidator, "???" } }
				} },
				{ "Camera",{
					{ "Enabled",{ CfgFile::BoolValidator ,"True" } },
					{ "FriendlyName",{ CfgFile::AnyValidator, "???" } },
					{ "Resolution",{ CfgFile::ResolutionValidator, "800x600" } },
				} },
				{ "Log",{
					{ "Console",{ log_validator, "all" } },
					{ "Hud",{ log_validator, "all" } },
					{ "File",{ log_validator, "all" } }
				} },
				{ "Window",{
					{ "Title",{ CfgFile::AnyValidator, "Drone controller" } },
					{ "Resolution",{ CfgFile::ResolutionValidator, "800x600" } },
					{ "Mode",{ CfgFile::IntRangeValidator(1,3), "1" } }
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
			std::cout << e.Message() << std::endl;
			std::cout << "Generating new config file..." << std::endl;
			cfg.SaveToFile("config/drone.ini");
		}

		camenabled = cfg.GetValue("Camera", "Enabled").ToBool();
		resolution = cfg.GetValue("Window", "Resolution").ToResolution();
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
		long ping_id = 0;




		while (t->running && t->sp.IsConnected())
		{
			sf::Clock clk;

			sf::Packet packet;
			packet << (uint8_t)Protocol_Controls;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))	packet << 1.f;
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) packet << -1.f;
			else packet << 0.f;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))	packet << 1.f;
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) packet << -1.f;
			else packet << 0.f;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))	packet << 1.f;
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) packet << -1.f;
			else packet << 0.f;

			uint8_t binary=0;
			binary |= sf::Keyboard::isKeyPressed(sf::Keyboard::E);

			//t->sp.SendPacket(packet);

			if (t->lastPing.getElapsedTime().asMilliseconds() > 1000)
			{
				t->lastPing.restart();

				sf::Packet ping;
				ping << (uint8_t)Protocol_Ping << ping_id++;
				t->pingTimer.restart();
				t->sp.SendPacket(ping);
				std::cout << "Pinging " << (ping_id-1) << std::endl;
			}

			std::this_thread::sleep_for(std::chrono::microseconds(33333));
		}
	}

	const void InitSerial()
	{
		sp.SetCallback([&](SerialPort::EventType e) {
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
				sf::Packet packet = sp.GetPacket();

				uint8_t protocol=0;
				packet >> protocol;

				switch (protocol)
				{
				case Protocol::Protocol_Ping:
				{
					uint64_t ping_id;
					packet >> ping_id;
					hud_topbar->SetPing(pingTimer.restart().asMilliseconds());
				} break;
				}
				
			} break;
			}
		});

		reconnect_serial = std::make_shared<std::thread>(Application::TryConnectSerial, this);
	}

	Application()
	{
		Log::Init();
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
					t->sp.Connect(p,(SerialPort::BaudRate)t->cfg.GetValue("NRF24_USB","BaudRate").ToInt());
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
		uint8_t mode = cfg.GetValue("Window", "Mode").ToInt(); // 1 - windowed, 2 - borderless, 3 - fullscreen
		if (mode == 2)
		{
			const sf::VideoMode vmode = sf::VideoMode().getDesktopMode();
			resolution = sf::Vector2i(vmode.width, vmode.height);
		}

		canvas = new Hud_Canvas(resolution, camResolution);
		hud_altitude = new Hud_Altitude(resolution, hud_scale, cfg.GetValue("Hud", "AltMax").ToFloat(), cfg.GetValue("Hud", "AltStep").ToFloat());
		hud_compass = new Hud_Compass(resolution,hud_scale);
		hud_roll = new Hud_Roll(resolution,hud_scale);
		hud_topbar = new Hud_TopBar(resolution, hud_scale);

	}

	const int Run()
	{
		sf::RenderWindow window;
		{
			uint8_t mode = cfg.GetValue("Window", "Mode").ToInt(); // 1 - windowed, 2 - borderless, 3 - fullscreen

			sf::ContextSettings settings;
			settings.antialiasingLevel = 8;
			settings.majorVersion = 1;
			settings.minorVersion = 2;

			sf::Uint32 style;
			switch (mode)
			{
			case 1: //Windowed
				style = sf::Style::Titlebar | sf::Style::Close;
				break;
			case 2: //Borderless
			{
				style = sf::Style::None;
				window.setMouseCursorVisible(false);
			}	break;
			case 3: //Fullscreen
				style = sf::Style::Fullscreen;
				window.setMouseCursorVisible(false);
				break;
			}

			window.create(sf::VideoMode(resolution.x, resolution.y),cfg.GetValue("Window","Title").ToString(),style, settings);
		}
		Hud_InverseShader inverse(resolution,camResolution);
		inverse.states.texture = &canvas->GetTexture();

		s_rf.setPosition(20, 20);
		s_cam.setPosition(20, 40);
		//alt.setPosition(20, 580);
		sf::Event e;
		sf::Clock test;
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
				case sf::Event::KeyPressed:
				{
					if (e.key.code == sf::Keyboard::W && throttle < 100) throttle += 10;
					if (e.key.code == sf::Keyboard::S && throttle > 0) throttle -= 10;
					if (sp.IsConnected() && (e.key.code == sf::Keyboard::W || e.key.code == sf::Keyboard::S))
					{
						sf::Packet p;
						p << (unsigned char)2 << throttle;
						sp.SendPacket(p);
						std::cout << "Throttle:" << throttle << std::endl;
					}
				}
				}
			}

			inverse.shader.setUniform("camera", cam.IsConnected());

			hud_roll->Update();
			hud_compass->Update();
			hud_altitude->Update();
			hud_compass->Set(180.0*(float)test.getElapsedTime().asMilliseconds() / 1000.0);
			hud_roll->Set(180.0*(float)test.getElapsedTime().asMilliseconds() / 1000.0);
			hud_altitude->Set(500.0*std::fmod((float)test.getElapsedTime().asMilliseconds() / 1000.0,1.0));
			
			window.clear(sf::Color::Black);
			if(camenabled) window.draw(*canvas);

			window.draw(s_rf);
			window.draw(s_cam);
			window.draw(*hud_compass, inverse.states);
			window.draw(*hud_roll, inverse.states);
			
			window.draw(*hud_altitude, inverse.states);
			window.draw(*hud_topbar);

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