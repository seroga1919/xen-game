#include <vector>
#include <math.h>
#include <mutex>
#include <unordered_map>

#include "defines.h"
#include "Utils.h"
#include "GameWorld.h"
#include "Logger.h"
#include "PacketManager.h"


GameWorld * GameWorld::instance;
std::mutex units_lock;
std::mutex lootables_lock;
//wtf
GameWorld::GameWorld(void)
{
	merchant = new Unit(0);
	instance =	this;
	units =		UNIT_MAP();
	lootables = UNIT_MAP();
}
int locX = 0;
int locY = 0;
// Updates the coordinates of an already existing Unit identified by 'true_id'.
// If the Unit doesn't exist in the collection, creates a new one.
void GameWorld::update(uint16 true_id, SPacket_UnitMovement mov)
{
	units_lock.lock();
	if (true_id != 0 && true_id == merchant->getId()) {
		//log(I, "Received position for merchant's true id " + std::to_string(merchant->getId()));
		merchant->set(mov);
		Bot::getInstance()->flags->merchant_lost = false;
	}

	std::unordered_map<int, Unit>::iterator unit =
		(std::unordered_map<int, Unit>::iterator)units.find(true_id);

	if (unit != units.end()) {
		unit->second.set(mov);
	}
	else
	{
		Unit u = Unit(true_id);
		u.set(mov);
		units.insert_or_assign(true_id, u);
	}
	units_lock.unlock();
	cleanUnitMap();
}

// Update the model data of an already existing Unit identified by 'true_id'.
// If the Unit doesn't exist in the collection, creates a new one.
void GameWorld::update(uint16 true_id, SPacket_UnitModel model)
{
	units_lock.lock();
	if (model.model == Bot::getInstance()->getCurrentPath()->getMerchantModelId()) {
		//log(I, "Received model for merchant's true id " + std::to_string(merchant->getId()));
		merchant->setId(true_id);
		merchant->set(model);
		//Bot::getInstance()->flags->merchant_lost = false;	 // No position yet
	}

	std::unordered_map<int, Unit>::iterator unit =
		(std::unordered_map<int, Unit>::iterator)units.find(true_id);

	if (unit != units.end()) {
		if (unit->second.isAlive())
			unit->second.set(model);
	}
	else {
		Unit u = Unit(true_id);
		u.set(model);
		units.insert_or_assign(true_id, u);
	}
	units_lock.unlock();
	cleanUnitMap();
}

// Update the anim data of an already existing Unit identified by 'true_id'.
// If the Unit doesn't exist in the collection, creates a new one.
void GameWorld::update(uint16 true_id, UpdatePacketUnitAnim anim) {

	if (anim.anim == 0)
		return;

	units_lock.lock();
	std::unordered_map<int, Unit>::iterator unit = (std::unordered_map<int, Unit>::iterator)units.find(true_id);
	if (unit != units.end() && unit->second.isAlive()) {
		unit->second.set(anim);
	}
	else {
		Unit u = Unit(true_id);
		u.set(anim);
		units.insert_or_assign(true_id, u);
	}
	units_lock.unlock();
}

Unit * GameWorld::getUnit(int id) {

	Unit * res = NULL;
	units_lock.lock();
	std::unordered_map<int, Unit>::const_iterator got = units.find(id);
	if (got != units.end()) {
		res = (Unit*)&got->second;
	}
	units_lock.unlock();
	return res;
}


int GameWorld::getLocX() {
	return locX;
}
int GameWorld::getLocY() {
	return locY;
}

Unit* GameWorld::getClosestUnitFromRealId(int realId) {
	Unit* res = NULL;
	units_lock.lock();
	for (auto unit : units) {
		if ((int)unit.second.getId() == realId
			&& (res == NULL
				|| unit.second.distanceFrom(playerPos.x, playerPos.y)
				< res->distanceFrom(playerPos.x, playerPos.y)
				)
			) {
			res = new Unit(unit.second.getId());
			res->set(unit.second.getUnitMovement());
			res->set(unit.second.getUpdatePacketUnitAnim());
			res->set(unit.second.getUnitModel());
		}
	}
	units_lock.unlock();
	return res;
}


Unit * GameWorld::getClosestUnitFromModel(int modelId) {
	Unit * res = NULL;
	units_lock.lock();
	for (auto unit : units) {
		if (unit.second.getUnitModel().model == modelId
			&& (res == NULL
				|| unit.second.distanceFrom(playerPos.x, playerPos.y)
				< res->distanceFrom(playerPos.x, playerPos.y)
				)
			) {
			res = new Unit(unit.second.getId());
			res->set(unit.second.getUnitMovement());
			res->set(unit.second.getUpdatePacketUnitAnim());
			res->set(unit.second.getUnitModel());
		}
	}
	units_lock.unlock();
	return res;
}

UNIT_MAP GameWorld::getUnitsInRange(int range, bool filterOutIDleUnits) {
	return getUnitsInRange(playerPos.x, playerPos.y, range, filterOutIDleUnits);
}

UNIT_MAP GameWorld::getUnitsInRange(int x, int y, int range, bool filterOutIDleUnits) {
	locX = 0;
	locY = 0;
	int Dis = 999;

	UNIT_MAP res;
	int d;
	units_lock.lock();
	for (auto elem : units) {

		d = distanceBetween(
			elem.second.getUnitMovement().positionX,
			elem.second.getUnitMovement().positionY,
			x,
			y
		);
		if (d < Dis)
		{
			Dis = d;
			locX = elem.second.getUnitMovement().positionX;
			locY = elem.second.getUnitMovement().positionY;
		}
		if (d <= range
			&& elem.second.getUpdatePacketUnitAnim().anim != 0
			&& elem.first != 0
			&& elem.second.getId() != 0
			&& elem.first != 3111
			&& elem.second.getId() != 11080
			//&& elem.second.getUnitModel().model != 0
			//&& elem.second.getUnitModel().model < 9000
			&& (!filterOutIDleUnits || elem.second.timeSinceLastUpdate() < IDLE_TIME)
			)
			res.insert_or_assign(elem.first, elem.second);
	}
	units_lock.unlock();
	return res;
}

int GameWorld::getEnemyNumberInRange(int range) {
	return getUnitsInRange(range).size();
}

// Removes the units out of range
void GameWorld::cleanUnitMap() {

	std::unordered_map<int, Unit> tmp;
	units_lock.lock();

	for (auto elem : units) {
		if (elem.second.isAlive()
			&& elem.second.distanceFrom(playerPos.x, playerPos.y) < detectionRange
			) {
			tmp.insert(elem);
		}
	}

	units = tmp;
	if (merchant->getId() != 0)
		units.insert_or_assign(merchant->getId(), *merchant);
	units_lock.unlock();
}

void GameWorld::purgeUnits() {
	units_lock.lock();
	units.clear();

	if (merchant->getId() != 0)
		units.insert_or_assign(merchant->getId(), *merchant);

	units_lock.unlock();
}

std::vector<std::string> GameWorld::units_toStrings(int range, bool filterOutIDleUnits) {
	std::vector<std::string> res;
	std::string str = "";
	for (auto elem : getUnitsInRange(range, filterOutIDleUnits)) {
		str = add_padding(std::to_string(elem.second.getUnitModel().model), 10)
			+ "< " + add_padding(std::to_string(elem.second.getUnitMovement().positionX), 10)
			+ "; "
			+ add_padding(std::to_string(elem.second.getUnitMovement().positionY), 10)
			+ ">  -  Distance : " + add_padding(std::to_string(elem.second.distanceFrom(playerPos.x, playerPos.y)), 10)
			+ "animId: " + std::to_string(elem.second.getUpdatePacketUnitAnim().anim)
			+ "    (uId: " + std::to_string(elem.first) + ")"
			+ "    (lastUpdated: " + std::to_string(elem.second.timeSinceLastUpdate()) + ")"
			+ "\n";
		res.push_back(str);
		str.clear();
	}

	return res;
}

void GameWorld::pushLootable(int x, int y) {

	if ( x != 0 && y != 0
		) {
		Unit u = Unit(lootables.size() + 1);
		SPacket_UnitMovement m;
		m.positionX = x;
		m.positionY = y;
		u.set(m);
		lootables.insert_or_assign(u.getId(), u);
	}
}

void GameWorld::updateLootables() {
	std::vector<POINT> points = PacketManager::getLootableBuffer();
	clearLootables();
	for (POINT p : points) {
		pushLootable(p.x, p.y);
	}
}

Unit * GameWorld::getNextLootable(bool canUseMoveSkill) {

	Unit * res = NULL;

	if (PacketManager::isLootableBuffUpdated()) {
		updateLootables();
	}

	if (lootables.size() == 0) {
		return NULL;
	}
	//lootables_lock.lock();
	int key_min = 0;
	int enemies_around, d, min_enemies_around = 0xFFFF, min_d = MAX_MOVE_DISTANCE;

	for (auto elem : lootables) {
		
		if (isBlacklisted(elem.second))
			continue;

		d = elem.second.distanceFrom(playerPos.x, playerPos.y);

		enemies_around = getUnitsInRange(
			elem.second.getUnitMovement().positionX,
			elem.second.getUnitMovement().positionY,
			40).size();

		// Find the closest loot with the least enemies around
		if (d < min_d-20		// If skill move disabled : only closest
			&& (!canUseMoveSkill
				|| enemies_around <= min_enemies_around)
			) {
			key_min = elem.first;
			min_d = d;
			min_enemies_around = enemies_around;
		}
	}
	
	
	if (key_min != 0)	// on rare occasions this happens
	{
		res = new Unit(lootables.at(key_min).getId());
		res->set(lootables.at(key_min).getUnitMovement());
		res->set(lootables.at(key_min).getUpdatePacketUnitAnim());
		res->set(lootables.at(key_min).getUnitModel());

		/*log(I, "Selected at "
			+ std::to_string(res->getUnitMovement().positionX) + " - "
			+ std::to_string(res->getUnitMovement().positionY)
		);*/
	}
	//lootables_lock.unlock();

	return res;
}

void GameWorld::clearLootables() {
	lootables.clear();
}

void GameWorld::removeLoot(int unitId) {
	//lootables_lock.lock();
	lootables.erase(unitId);
	//lootables_lock.unlock();
}

void GameWorld::blacklistLootPosition(int x, int y) {
	blacklisted.push_back(POINT{ x,y });
}

bool GameWorld::isBlacklisted(Unit loot) {
	int x = loot.getUnitMovement().positionX;
	int y = loot.getUnitMovement().positionY;
	bool res = false;

	for (auto elem : blacklisted) {
		if (x == elem.x && y == elem.y) {
			res = true;
			break;
		}
	}

	return res;
}

void GameWorld::clearBlacklist() {
	blacklisted.clear();
}
