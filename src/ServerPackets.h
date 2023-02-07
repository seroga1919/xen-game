#pragma once

// Packets received by the game client (mainly based on previous project)

struct InitialLoginData
{
	int32 positionX;		// 0
	int32 positionY;		// 4
	uint32 mapId;			// 8
	uint16 serverId;		//12
	uint16 unk;				//14 - high bits of serverid, seems to be 32bit, unused
	uint16 unk2;			//16 - seems to be initial login (show tutorial if 1)
	uint16 unk3;			//18
	uint16 serverId2;		//20 - if this != previous id, do not login, don't know why this is here, maybe an old hack by that hollow guy
	uint16 unk4;			//22 - high bits of serverid, unused
	float time_current;		//24 - time as of login
	float time_roc;			//28 - rate of change (every 50 milliseconds passing, rate value changes)
	uint8 unk7;				//32
};


struct SPacket_SelfData
{
	uint16 positionX;				 // 0
	uint16 positionY;				 // 2
	uint16 spellflags;				 // 4
	uint16 colorbits;				 // 6
	uint16 numInRangeDynamicObjects; // 8 - Includes loots on the floor
	uint8 numInRangeUnits;			 //10 - Includes mobs, alive AND dead
	uint8 unklol;					 //11 - Always 0 ?
	uint8 flags;					 //12 - if & 1, expect self say text at end, if & 2, spellflags contain cooldown data, expect no buffs or skills this update if & 4 you're in pvp mode
	uint8 rotation;					 //13
	uint8 animation;				 //14
	uint8 spellEffect;				 //15
	uint8 numInRangeSpellEffects;	 //16
	uint8 unklol2;					 //17 - Light level ?
	uint8 unklol3;					 //18 - 
	uint8 currentHPPct;				 //19
	uint8 currentMPPct;				 //20 - for hp mp and exp, 0 = 0%, 255 = 100%
	uint8 currentExpPct;			 //21
};

struct SPacket_UnitMovement
{
	uint16 positionX;
	uint16 positionY;
	uint8 rotation;
};

struct SPacket_UnitModel
{
	uint16 model;
	uint16 weapon;
	uint8 shield;
	uint8 helmet;
	uint16 colorbits;
};

struct UpdatePacketUnitAnim
{
	uint8 anim;
};

struct LootObject
{
	uint16 x_pos_root;		// = x/20
	uint16 y_pos_root;		// = y/20
	uint8 item_model1;		// Always set
	uint8 padding_zero1;	// Always 0
	uint8 item_model2;		// Unsure after that offset
	uint8 padding_zero2;
	uint8 item_model3;
	uint8 padding_zero3;
};