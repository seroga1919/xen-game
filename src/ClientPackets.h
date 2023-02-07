#pragma once
#include "defines.h"
// Sent by game client
struct CPacket_Move
{
	uint16 charId;
	uint16 x;
	uint16 y;
	uint16 checksum;
};