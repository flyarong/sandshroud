/*
* Aspire Hearthstone
* Copyright (C) 2008 - 2010 AspireDev <http://www.aspiredev.net/>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef _VEHICLE_H
#define _VEHICLE_H

class SERVER_DECL Vehicle : public Creature
{
public:
	Vehicle(uint64 guid);
	~Vehicle();
	virtual void Destructor();

	void Init();
	void InitSeats(uint32 vehicleEntry, Player* pRider = NULLPLR);
	virtual void Update(uint32 time);
	bool Load(CreatureSpawn *spawn, uint32 mode, MapInfo *info);
	void Load(CreatureProto * proto_, float x, float y, float z, float o = 0.0f);
	void Despawn(uint32 delay, uint32 respawntime);
	void DeleteMe();
	void SafeDelete();
	void MoveVehicle(float x, float y, float z, float o);
	void AddPassenger(Unit* pPassenger);
	void RemovePassenger(Unit* pPassenger);
	bool HasPassenger(Unit* pPassenger);
	void SendSpells(uint32 entry, Player* plr);
	void setDeathState(DeathState s);
	void SetSpeed(uint8 SpeedType, float value);

	//---------------------------------------
	// Accessors
	//---------------------------------------
	uint32 GetMaxPassengerCount() { return m_maxPassengers; }
	uint32 GetPassengerCount() { return m_passengerCount; }

	uint32 GetVehicleEntry() { return m_vehicleEntry; }
	void SetVehicleEntry(uint32 entry) { m_vehicleEntry = entry; }

	Unit* GetControllingUnit() { return m_passengers[0]; }
	void SetControllingUnit(Unit* pUnit) { m_controllingUnit = pUnit; }

	uint8 GetPassengerSlot(Unit* pPassenger);
	//---------------------------------------
	// End accessors
	//---------------------------------------

	bool IsFull() { return m_passengerCount == m_maxPassengers; }

	VehicleSeatEntry * m_vehicleSeats[8];
	bool Initialised;
	bool m_CreatedFromSpell;
	uint32 m_mountSpell;

private:
	void _AddToSlot(Unit* pPassenger, uint8 slot);

protected:
	Unit* m_controllingUnit;

	Unit* m_passengers[8];

	uint8 m_passengerCount;
	uint8 m_maxPassengers;
	uint32 m_vehicleEntry;
};

#endif
