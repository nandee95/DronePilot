#pragma once

//Types
typedef float fdp_float;
typedef uint8_t fdp_8;
typedef uint16_t fdp_16;
typedef char fdp_char;

typedef fdp_8 fdp_protocol;

template <typename T>
struct fdp_vec3
{
	T X, Y, Z;
};

typedef fdp_vec3<fdp_float> fdp_vec3f;

struct fdp_pid
{
	fdp_float P, I, D;
};

//Flags
#define FDP_SEN_ACC 1<<0
#define FDP_SEN_GYRO 1<<1
#define FDP_SEN_MAG 1<<2
#define FDP_SEN_ALT 1<<3

//Protocol
enum Protocol : fdp_8
{
	UnknownProtocol,
	RequestConnect,
	WhoAmI,
	SensorReport,
	Controls,
	Calibrate,
	PID
};

//Structures
struct Protocol_RequestConnect
{
	fdp_protocol id = Protocol::RequestConnect;
};

struct Protocol_WhoAmI
{
	fdp_protocol id = Protocol::WhoAmI;
	fdp_16 version;
	fdp_16 sensors;
	fdp_char name[32];
};

struct Protocol_SensorReport
{
	fdp_protocol id = Protocol::SensorReport;
	fdp_vec3f gyro;// , acc, mag;
	//fdp_float alt;
	//fdp_float cell_volts[6];
};

struct Protocol_Controls
{
	fdp_protocol id = Protocol::Controls;
	float rearfront, leftright, downup;
	float rotation;
};

struct Protocol_Calibrate
{
	fdp_protocol id = Protocol::Calibrate;
	fdp_vec3f diff;
};

struct Protocol_PID
{
	fdp_protocol id = Protocol::PID;
	fdp_vec3<fdp_pid> pid;
};