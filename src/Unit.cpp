#include "defines.h"
#include "Utils.h"
#include "ServerPackets.h"
#include "Unit.h"

Unit::Unit(uint16 id) {
	m_id = id;
	m_movement = { 0 };
	m_model = { 0 };
	m_anim = { 0 };
	lastUpdated = clock();
}

Unit::Unit(uint16 id, SPacket_UnitMovement mov, SPacket_UnitModel model, UpdatePacketUnitAnim anim)
{
	m_id = id;
	m_movement = mov;
	m_model = model;
	m_anim = anim;
	lastUpdated = clock();
}


bool Unit::isAlive() {
	return m_anim.anim != 21
		&& m_anim.anim != 33
		&& m_anim.anim != 45;
}


int Unit::distanceFrom(int x, int y) {
	return (int)distanceBetween(m_movement.positionX, m_movement.positionY, x, y);
}


void Unit::touch() {
	lastUpdated = clock();
}


float Unit::timeSinceLastUpdate() {
	return ((float)(clock() - lastUpdated)) / CLOCKS_PER_SEC;
}


void Unit::set(SPacket_UnitMovement mov) {
	m_movement = mov;
	touch();
}


void Unit::set(SPacket_UnitModel model) {
	m_model = model;
	touch();
}


void Unit::set(UpdatePacketUnitAnim anim) {
	m_anim = anim;
	touch();
}