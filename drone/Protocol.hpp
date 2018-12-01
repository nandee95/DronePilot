#pragma once

enum Protocol
{
	Protocol_Unknown,	//???
	Protocol_HandShake,	//[DC] byte[64] token
	Protocol_Ping,		//[DC] none
	Protocol_Message,
	Protocol_Controls,	//[D]  float bwfw, float ltrt, float uwdw,int8_t (horn)
	Protocol_Sensors    //[C]
};