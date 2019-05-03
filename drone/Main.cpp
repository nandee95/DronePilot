#include "SerialPort.hpp"
#include <iostream>
#include <stdio.h>
#include "Application.hpp"

#ifdef _DEBUG
	#pragma comment(lib,"sfml-system-d.lib")
	#pragma comment(lib,"sfml-window-d.lib")
	#pragma comment(lib,"sfml-graphics-d.lib")
	#pragma comment(lib,"sfml-network-d.lib")
#else
	#pragma comment(lib,"sfml-system.lib")
	#pragma comment(lib,"sfml-window.lib")
	#pragma comment(lib,"sfml-graphics.lib")
	#pragma comment(lib,"sfml-network.lib")
#endif
#pragma comment(lib,"freetype.lib")
#pragma comment(lib,"opengl32.lib")

int main()
{
	Application app;
	return app.Run();
}