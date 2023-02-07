#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "defines.h"
#include "ServerPackets.h"
#include "Unit.h"

#define UNIT_MAP	std::unordered_map<int, Unit>
#define IDLE_TIME	1.5

class GameWorld
{
private:
	static GameWorld *				instance;
	POINT							playerPos;
	// Range after which the server sends the unit's info again
	const int						detectionRange = 800;
	UNIT_MAP						units;
	UNIT_MAP						lootables;
	std::vector<POINT>				blacklisted;
	static HANDLE					ghMutex;
	Unit * merchant;

public:
						GameWorld(void);
	static GameWorld *	getInstance() { return instance; }

	void				setPlayerPos(POINT p) { playerPos = p; };
	void				update(uint16 true_id, SPacket_UnitMovement mov);
	void				update(uint16 true_id, SPacket_UnitModel model);
	void				update(uint16 true_id, UpdatePacketUnitAnim anim);
	
	Unit *				getUnit(int id);
	int getLocX();
	int getLocY();
	Unit* getClosestUnitFromRealId(int realId);
	Unit *				getClosestUnitFromModel(int modelId);
	UNIT_MAP			getUnitsInRange(int x, int y, int range, bool filterOutIDleUnits = true);
	UNIT_MAP			getUnitsInRange(int range, bool filterOutIDleUnits = true);
	int					getEnemyNumberInRange(int range);
	void				cleanUnitMap();
	void				purgeUnits();

	void				pushLootable(int x, int y);
	void				updateLootables();
	UNIT_MAP *			getLootables() { return &lootables; }
	Unit *				getNextLootable(bool canUseMoveSkill = false);
	void				clearLootables();
	void				removeLoot(int unitId);
	void				blacklistLootPosition(int x, int y);
	bool				isBlacklisted(Unit loot);
	void				clearBlacklist();
	
	std::vector<std::string>	units_toStrings(int range = 50, bool filterOutIDleUnits = true);
};
