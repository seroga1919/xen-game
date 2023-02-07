#pragma once
#include <Windows.h>
#include <vector>
#include <time.h>
#include "defines.h"
#include "ServerPackets.h"

class PacketManager {

public:
	static float timeSinceLastReceived();
	static bool isLootableBuffUpdated();
	static std::vector<POINT> getLootableBuffer();

	static bool handleInboundPacket(uint8* packet, int len);
	static bool handleOutboundPacket(uint8 opcode, uint8* packet, int len);

	static uint16 genMoveChecksum(uint16 id, uint16 x, uint16 y, uint8 type);
protected:
	static void handleLoginData(InitialLoginData data);
	static void handleContextUpdate(uint8* packet, int len);
private:
	static clock_t lastPacketReceived;
};