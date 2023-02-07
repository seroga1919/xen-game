#pragma once
#include <time.h>
#include <iostream>

class Unit
{
private:
	uint16 m_id;
	clock_t lastUpdated;
	SPacket_UnitMovement m_movement;
	SPacket_UnitModel m_model;
	UpdatePacketUnitAnim m_anim;

public:
	Unit(uint16 id);
	Unit(uint16 id, SPacket_UnitMovement mov, SPacket_UnitModel model, UpdatePacketUnitAnim anim);

	bool isAlive();
	int  distanceFrom(int x, int y);
	float timeSinceLastUpdate();

	void setId(uint16 id) { m_id = id; }
	void set(SPacket_UnitMovement mov);
	void set(SPacket_UnitModel model);
	void set(UpdatePacketUnitAnim anim);
	void touch();
	
	uint16					getId() { return m_id; }
	SPacket_UnitMovement	getUnitMovement() { return m_movement; }
	SPacket_UnitModel		getUnitModel() { return m_model; }
	UpdatePacketUnitAnim	getUpdatePacketUnitAnim() { return m_anim; }
};