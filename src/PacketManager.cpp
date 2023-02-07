#include <Windows.h>
#include <vector>
#include <time.h>

#include "PacketManager.h"
#include "ServerPackets.h"
#include "Logger.h"
#include "GameWorld.h"
#include "Bot.h"
#include "IDInput8.h"

clock_t PacketManager::lastPacketReceived;

float PacketManager::timeSinceLastReceived() {
	return ((float)(clock() - lastPacketReceived)) / CLOCKS_PER_SEC;
}

bool PacketManager::handleInboundPacket(uint8 * packet, int len)
{
	lastPacketReceived = clock();

	switch (packet[0]) {
	case 0x00:
		log(I, "Server is online");
		Bot::getInstance()->gamestatus->server_online = true;
		break;
	case 0x0F:
		log(I, "Client logged in");
		break;
	case 0x1F:
		log(I, "Character enters world");
		handleLoginData(*(InitialLoginData*)&packet[2]);
		Bot::getInstance()->gamestatus->character_spawned = true;
		break;
	case 0x03:
		handleContextUpdate(packet, len);
		break;
	case 0x05:
		Bot::getInstance()->flags->inventory_is_full = true;
		break;
	case 0x19:
		Bot::getInstance()->flags->waiting_loot = false;
		break;
	case 0x12:
		Bot::getInstance()->flags->inventory_open = true;
		break;
	case 0x0c:	// Inventory update : -/+ item | skill used ?
		if ((int)packet[2] == 0x20) {
			Bot::getInstance()->flags->item_acquired = true;
		}
	case 0x16:	// Inventory update : -/+ item quantity ? 
		break;
	case 0x17:	// Target's inventory list (seen in merchant)
		break;
	default:
		break;
	}

	return true;
}

uint16 val;
bool PacketManager::handleOutboundPacket(uint8 opcode, uint8* packet, int len)
{
	
	
	
	std::cout << std::to_string(opcode) << std::endl;
	std::cout << std::to_string(packet[0]) + ' '
		+ std::to_string((int)packet[1]) + ' '
		+ std::to_string((int)packet[2]) + ' '
		+ std::to_string((int)packet[3]) + ' '
		+ std::to_string((int)packet[4]) + ' '
		+ std::to_string((int)packet[5]) + ' '
		+ std::to_string((int)packet[6]) + ' '
		+ std::to_string((int)packet[7]) + ' '
		+ std::to_string((int)packet[8]) + ' '
		+ std::to_string((int)packet[9]) + ' '
		+ std::to_string((int)packet[10]) + ' '
		+ std::to_string((int)packet[11]) + ' '
		+ std::to_string((int)packet[12]) + ' '
		+ std::to_string((int)packet[13]) + ' '
		+ std::to_string((int)packet[14]) + ' '
		+ std::to_string((int)packet[15]) + ' '
		+ std::to_string((int)packet[16]) + ' ' << std::endl;
	

	switch (opcode) {
	case 0x00:
		log(I, "Game is ready");
		Bot::getInstance()->gamestatus->game_ready = true;
		break;
	case 0x01:
		Bot::getInstance()->onMoveOrder(opcode, (uint16*)packet, len);
		break;
	case 0x0f:
		Bot::getInstance()->onPasswordSent(packet);
		break;
	case 0x06:
		Bot::getInstance()->flags->item_sold = true;
		break;
	case 0x07:
		Bot::getInstance()->flags->item_used = true;
		break;
	case 0x08:
		//std::cout << std::to_string(opcode) << std::endl;
		//std::cout << std::to_string(packet[3]) + ' ' << std::endl;
		//Bot::getInstance()->spellPacketCounter = packet[3];
		
		if ((int)packet[6] == 48)
		{
			val = *((uint16*)packet + 4);
			std::cout << "locked to : " + std::to_string((int)val) << std::endl;
			Bot::getInstance()->enemyToAttack1 = (int)val;
		}
		/*//
		std::cout << std::to_string(opcode) << std::endl;
		std::cout << std::to_string(packet[0]) + ' '
			+ std::to_string((int)packet[1]) + ' '
			+ std::to_string((int)packet[2]) + ' '
			+ std::to_string((int)packet[3]) + ' '
			+ std::to_string((int)packet[4]) + ' '
			+ std::to_string((int)packet[5]) + ' '
			+ std::to_string((int)packet[6]) + ' '
			+ std::to_string((int)packet[7]) + ' '
			+ std::to_string((int)packet[8]) + ' '
			+ std::to_string((int)packet[9]) + ' '
			+ std::to_string((int)packet[10]) + ' '
			+ std::to_string((int)packet[11]) + ' '
			+ std::to_string((int)packet[12]) + ' '
			+ std::to_string((int)packet[13]) + ' '
			+ std::to_string((int)packet[14]) + ' '
			+ std::to_string((int)packet[15]) + ' '
			+ std::to_string((int)packet[16]) + ' ' << std::endl;
			*/
		Bot::getInstance()->flags->waiting_skillcast = false;
		break;
	default:
		break;
	}
	return true;
}


// Opcode = 0x1f
void PacketManager::handleLoginData(InitialLoginData data) {
	Bot::getInstance()->setId(data.serverId);
	log(I, std::string("Received player id : ").append(std::to_string(data.serverId)));
}


///////////////////////////////////////////////////////////////////////////////
// Attempt to fix potential data-race crashes, while using C++ objects :
// The array is used in write access by the game thread, and read access by the
// bot's thread.
POINT	lootableBuff[500];
int		lootableNumber = 0;
bool	lootablesUpdated = true;	// Access to boolean is atomic ? So np

std::vector<POINT> PacketManager::getLootableBuffer() {
	std::vector<POINT> res(lootableBuff, lootableBuff+lootableNumber);
	lootablesUpdated = false;
	return res;
}

bool PacketManager::isLootableBuffUpdated() {
	return lootablesUpdated;
}
///////////////////////////////////////////////////////////////////////////////
// Opcode = 0x03
void PacketManager::handleContextUpdate(uint8* packet, int len) {

	SPacket_SelfData update;
	int offset = 2, inc = 0;

	update = *(SPacket_SelfData*)&packet[offset];
	Bot::getInstance()->updateSelf(update.positionX, update.positionY, update.currentHPPct, update.currentMPPct, update.currentExpPct);

	offset += sizeof(SPacket_SelfData);

	lootableNumber = 0;
	lootablesUpdated = true;
	for (int i = 0; i < update.numInRangeDynamicObjects; i++)
	{
		if (packet[offset] == 0x01) {
			inc = 8;
			// Unique write access to lootableBuff
			if (lootableNumber <
				(sizeof(lootableBuff) / sizeof(lootableBuff[0]))
				) {
				lootableBuff[lootableNumber] = POINT{
					((uint16*)(packet + offset + 2))[0] * 20 + 10,
					((uint16*)(packet + offset + 4))[0] * 20 + 10
				};
				lootableNumber++;
			}
		}
		else if (packet[offset] == 0x02) {  // lootable	| see 'struct LootObject'
			inc = 12;
			// Unique write access to lootableBuff
			if (lootableNumber < 
				(sizeof(lootableBuff)/sizeof(lootableBuff[0]))
					) {
				lootableBuff[lootableNumber] = POINT{
					((uint16*)(packet + offset + 2))[0] * 20 + 10,
					((uint16*)(packet + offset + 4))[0] * 20 + 10
				};
				lootableNumber++;
			}
		}
		else if (packet[offset] == 0x03) {
			//std::cout << "0x03: \n";// <<
			inc = 6;
		}
		offset += inc;
	}

	// Unused currently
	for (int x = 0; x < update.numInRangeSpellEffects; x++)
	{
		if (packet[offset] >= 240)
			offset += 12;
		else 
			offset += 8;
	}

	for (int i = 0; i < update.numInRangeUnits; i++)
	{
		uint16 id = (packet[offset + 1] << 8) + packet[offset];
		uint16 trueid = id & 0x7FFF;
		offset += 2;

		if (id & 0x8000)
			continue;
		else
		{
			uint8 updateflag = packet[offset];
			offset += 1;

			if (updateflag & 0x01)
			{
				GameWorld::getInstance()->update(trueid, *(SPacket_UnitMovement*)&packet[offset]);
				offset += 5;
			}
			if (updateflag & 0x02)
			{
				offset += 3;
			}
			if (updateflag & 0x04)
			{
				GameWorld::getInstance()->update(trueid, *(SPacket_UnitModel*)&packet[offset]);
				offset += 8;
			}
			if (updateflag & 0x08)
			{
				GameWorld::getInstance()->update(trueid, *(UpdatePacketUnitAnim*)&packet[offset]);
				offset += 1;
			}
			if (updateflag & 0x10)
			{
				offset += 1;
			}
			if (updateflag & 0x20)
			{
				offset += 1 + packet[offset];
			}

		}
	}
}


uint16 PacketManager::genMoveChecksum(uint16 id, uint16 x, uint16 y, uint8 type)
{
	uint16 checksum = 0;
	checksum += id;
	checksum += x;
	checksum += y;
	checksum += type;
	checksum = type | (checksum << 8);
	return checksum;
}