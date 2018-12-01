#include "SerialPort.hpp"
#include <iostream>
#include <stdio.h>
#include "Application.hpp"

#pragma comment(lib,"sfml-system-d.lib")
#pragma comment(lib,"sfml-window-d.lib")
#pragma comment(lib,"sfml-graphics-d.lib")
#pragma comment(lib,"sfml-network-d.lib")
#pragma comment(lib,"freetype.lib")
#pragma comment(lib,"opengl32.lib")

int main()
{
	Application app;
	return app.Run();
}