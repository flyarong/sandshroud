/*
 * Sandshroud Hearthstone
 * Copyright (C) 2010 - 2011 Sandshroud <http://www.sandshroud.org/>
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

#include "StdAfx.h"

/// Where we check if we object A can attack object B. This is used in many feature's
/// Including the spell class, the player class, and the AI interface class.
int intisAttackable(Object* objA, Object* objB, bool CheckStealth)// A can attack B?
{
	// can't attack self.. this causes problems with buffs if we don't have it :p
	if( !objA || !objB || objA == objB )
		return 0;

	// can't attack corpses neither...
	if( objA->GetTypeId() == TYPEID_CORPSE || objB->GetTypeId() == TYPEID_CORPSE )
		return 0;

	// Dead people can't attack anything.
	if( (objA->IsUnit() && !TO_UNIT(objA)->isAlive()) || (objB->IsUnit() && !TO_UNIT(objB)->isAlive()) )
		return 0;

	// Checks for untouchable, unattackable
	if( objA->IsUnit() && (objA->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_9) ||
		objA->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNTED_TAXI) || objA->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE)))
		return 0;

	if( objB->IsUnit() && (objB->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_9) ||
		objB->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNTED_TAXI) || objB->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE)))
		return 0;

	if(!objA->PhasedCanInteract(objB))
		return 0;

	// we cannot attack sheathed units. Maybe checked in other places too ?
	// !! warning, this presumes that objA is attacking ObjB
	if( CheckStealth && objB->IsUnit() && TO_UNIT(objB)->InStealth() )
		return 0;

	// Get players (or owners of pets/totems)
	Player* player_objA = GetPlayerFromObject(objA);
	Player* player_objB = GetPlayerFromObject(objB);
	if(objA->IsUnit() && objB->IsVehicle())
		if(TO_VEHICLE(objB)->GetPassengerSlot(TO_UNIT(objA)) != -1)
			return 0;
	else if(objB->IsUnit() && objA->IsVehicle())
		if(TO_VEHICLE(objA)->GetPassengerSlot(TO_UNIT(objB)) != -1)
			return 0;

	// Allow GM's to attack any creatures, but players are a no.
	if(player_objA && player_objB && player_objA->bGMTagOn)
		return -1;

	// Don't allow players to attack GMs
	if(player_objA && player_objB && player_objB->bGMTagOn)
		return -1;

	// Creatures cannot attack a GM with tag on.
	if(!player_objA && player_objB && player_objB->bGMTagOn)
		return 0;

	if(objA->IsCreature() && isTargetDummy(objA->GetEntry()))
		return 0; // Bwahahaha

	if( player_objA && player_objB )
	{
		if(player_objA->DuelingWith == player_objB && player_objA->GetDuelState() == DUEL_STATE_STARTED )
			return 1;
	}
	else if(player_objA)
	{
		if(objB->IsPet() && TO_PET(objB)->GetOwner() == player_objA && player_objA->DuelingWith == player_objB)
			return 1;
	}
	else if(player_objB)
	{
		if(objA->IsPet() && TO_PET(objA)->GetOwner() == player_objB && player_objB->DuelingWith == player_objA)
			return 1;
	}
	else if(player_objA == NULL && player_objB == NULL) // Ignore players, we have critters in sanctuaries
	{
		// Do not let units attack each other in sanctuary
		// We know they aren't dueling
		AreaTable *atA = dbcArea.LookupEntry(objA->GetAreaID());
		AreaTable *atB = dbcArea.LookupEntry(objB->GetAreaID());
		if( atA && atB && (atA->AreaFlags & AREA_SANCTUARY || atB->AreaFlags & AREA_SANCTUARY) )
			return 0;

		if(sWorld.FunServerMall != -1 && (objA->GetAreaID() == (uint32)sWorld.FunServerMall
			|| objB->GetAreaID() == (uint32)sWorld.FunServerMall))
			return 0;
	}

	if(objA->IsCreature())
	{
		if(objA->IsPet())
		{
			if(player_objB)
			{
				if(TO_PET(objA)->GetOwner())
				{
					if(TO_PET(objA)->GetOwner()->IsPvPFlagged() && !player_objB->IsPvPFlagged())
						return 0;
					if(!TO_PET(objA)->GetOwner()->IsPvPFlagged() && !player_objB->IsPvPFlagged())
						return 0;
					// the target is PvP, its okay.
				}
				else
					return 0;
			}
		}
		else if(TO_CREATURE(objA)->IsTotem())
		{
			if(player_objB)
			{
				if(TO_CREATURE(objA)->GetSummonOwner())
				{
					if(TO_PET(objA)->GetSummonOwner()->IsPvPFlagged() && !player_objB->IsPvPFlagged())
						return 0;
					if(!TO_PET(objA)->GetSummonOwner()->IsPvPFlagged() && !player_objB->IsPvPFlagged())
						return 0;
					// the target is PvP, its okay.
				}
				else
					return 0;
			}
		}
	}

	// We do need all factiondata for this
	if( objB->m_factionDBC == NULL || objA->m_factionDBC == NULL || objB->m_faction == NULL || objA->m_faction == NULL
		|| (((objA->IsPlayer() && !TO_PLAYER(objA)->IsFFAPvPFlagged()) ? true : false)
		&&  ((objB->IsPlayer() && !TO_PLAYER(objB)->IsFFAPvPFlagged()) ? true : false)
		&& (objB->m_factionDBC == objA->m_factionDBC || objB->m_faction == objA->m_faction)))
		return 0;

	if( player_objA && player_objB )
	{
		if(sWorld.FunServerMall != -1 && (player_objA->GetAreaID() == (uint32)sWorld.FunServerMall
			|| player_objB->GetAreaID() == (uint32)sWorld.FunServerMall))
			return 0;

		if(player_objA->IsPvPFlagged() && !player_objB->IsPvPFlagged() && player_objA->DuelingWith != player_objB)
			return 0;
		if(!player_objA->IsPvPFlagged() && !player_objB->IsPvPFlagged() && player_objA->DuelingWith != player_objB)
			return 0;

		//These area's are sanctuaries
		for(uint32 i = 0; i < NUM_SANCTUARIES ; i++)
		{
			if( player_objA->GetPlayerAreaID() == SANCTUARY_AREAS[i] || player_objB->GetPlayerAreaID() == SANCTUARY_AREAS[i])
				return 0;
		}

		// Players with feign death flags can't be attacked
		// But they can be attacked by another players. -- Dvlpr
		// WARNING: This presumes, that objA attacks objb!!!
		if( (objA->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH) || objB->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH)) && !objA->IsPlayer() )
			return 0;

		//Handle BG's
		if( player_objA->m_bg != NULL)
		{
			//Handle ffa_PVP
			if( player_objA->HasFlag(PLAYER_FLAGS,PLAYER_FLAG_FREE_FOR_ALL_PVP) && player_objA->HasFlag(PLAYER_FLAGS,PLAYER_FLAG_FREE_FOR_ALL_PVP))
			{
				if( player_objA->GetGroup() == player_objB->GetGroup() )
					return 0;
				else
					return 1;
			}
			//Handle Arenas
			if( player_objA->GetTeam() != player_objB->GetTeam() )
				return 1;
		}

		if(player_objA->IsFFAPvPFlagged() && player_objB->IsFFAPvPFlagged())
		{
			if( player_objA->GetGroup() && player_objA->GetGroup() == player_objB->GetGroup() )
				return 0;

			if( player_objA == player_objB ) // Totems...
				return 0;

			return 1;		// can hurt each other in FFA pvp
		}

		if( player_objA->GetAreaDBC() != NULL )
		{
			if( player_objA->GetAreaDBC()->AreaFlags & 0x800 )
				return 0;
		}

		return 1; // Skip the rest of this, it's all faction shit.
	}

	// same faction can't kill each other.
	if(objA->m_faction == objB->m_faction)
		return 0;

	// moved this from IsHostile();
	// by doing so we skip a hell of a lot redundant checks, which we already passed in this routine.
	uint32 faction = objB->m_faction->Mask;
	uint32 host = objA->m_faction->HostileMask;
	bool hostile = false;
	if(faction & host)
	{
		hostile = true;
		// check friend/enemy list
		for(uint32 i = 0; i < 4; i++)
		{
			if(objA->m_faction->EnemyFactions[i] == objB->m_faction->Faction)
				hostile = true;

			if(objA->m_faction->FriendlyFactions[i] == objB->m_faction->Faction)
				hostile = false;
		}
	}

	// Reputation System Checks
	if(player_objA)
	{
		if(objB->m_factionDBC->RepListId >= 0)
			hostile = player_objA->IsHostileBasedOnReputation( objB->m_factionDBC );

		if(hostile == false)
		{
			if(objB->m_factionDBC->RepListId == -1 && objB->m_faction->HostileMask == 0 && objB->m_faction->FriendlyMask == 0)
				hostile = true;

			if(player_objA->bGMTagOn)
				hostile = true;
		}
	}
	else if(player_objB)
	{
		if(objA->m_factionDBC->RepListId >= 0)
			hostile = player_objB->IsHostileBasedOnReputation( objA->m_factionDBC );

		if(hostile == false)
			if(objA->m_factionDBC->RepListId == -1 && objA->m_faction->HostileMask == 0 && objA->m_faction->FriendlyMask == 0)
				hostile = true;
	}

	return (hostile ? 1 : 0);
}

bool isAttackable(Object* objA, Object* objB, bool CheckStealth)// A can attack B?
{
	return (intisAttackable(objA, objB, CheckStealth) == 1);
}

bool isHostile(Object* objA, Object* objB)// B is hostile for A?
{
	bool hostile = false;

	// can't attack self.. this causes problems with buffs if we dont have it :p
	if(!objA || !objB || (objA == objB))
		return false;

	// can't attack corpses neither...
	if(objA->GetTypeId() == TYPEID_CORPSE || objB->GetTypeId() == TYPEID_CORPSE)
		return false;

	if( objA->m_faction == NULL || objB->m_faction == NULL || objA->m_factionDBC == NULL || objB->m_factionDBC == NULL )
		return true;

	if(!objA->PhasedCanInteract(objB))
		return false;

	uint32 faction = objB->m_faction->Mask;
	uint32 host = objA->m_faction->HostileMask;

	if(faction & host)
		hostile = true;

	// check friend/enemy list
	for(uint32 i = 0; i < 4; i++)
	{
		if(objA->m_faction->EnemyFactions[i] == objB->m_faction->Faction)
		{
			hostile = true;
			break;
		}
		if(objA->m_faction->FriendlyFactions[i] == objB->m_faction->Faction)
		{
			hostile = false;
			break;
		}
	}

	// PvP Flag System Checks
	// We check this after the normal isHostile test, that way if we're
	// on the opposite team we'll already know :p
	Player* player_objA = GetPlayerFromObject(objA);
	Player* player_objB = GetPlayerFromObject(objB);

	//BG or PVP?
	if( player_objA && player_objB )
	{
		if( player_objA->m_bg != NULL )
		{
			if( player_objA->m_bgTeam != player_objB->m_bgTeam )
				return true;
		}
		if( hostile && player_objA->IsPvPFlagged()&& player_objB->IsPvPFlagged() )
			return true;
		else
			return false;
	}

	// Reputation System Checks
	if(player_objA && !player_objB)		// PvE
	{
		if(objB->m_factionDBC->RepListId >= 0)
			hostile = player_objA->IsHostileBasedOnReputation( objB->m_factionDBC );
	}

	if(player_objB && !player_objA)		// PvE
	{
		if(objA->m_factionDBC->RepListId >= 0)
			hostile = player_objB->IsHostileBasedOnReputation( objA->m_factionDBC );
	}

	if( objA->IsPlayer() && objB->IsPlayer() && TO_PLAYER(objA)->m_bg != NULL )
	{
		if( TO_PLAYER(objA)->m_bgTeam != TO_PLAYER(objB)->m_bgTeam )
			return true;
	}

	return hostile;
}

Player* GetPlayerFromObject(Object* obj)
{
	Player* player_obj = NULLPLR;

	if( obj->IsPlayer() )
	{
		player_obj =  TO_PLAYER( obj );
	}
	else if( obj->IsPet() )
	{
		Pet* pet_obj = TO_PET(obj);
		if( pet_obj )
			player_obj =  pet_obj->GetPetOwner();
	}
	else if( obj->IsUnit() )
	{
		// If it's not a player nor a pet, it can still be a totem.
		Creature* creature_obj = TO_CREATURE(obj);
		if( creature_obj && creature_obj->IsTotem())
			player_obj =  TO_PLAYER(creature_obj->GetSummonOwner());
	}
	return player_obj;
}

bool isCombatSupport(Object* objA, Object* objB)// B combat supports A?
{
	if( !objA || !objB )
		return false;

	// can't support corpses...
	if( objA->GetTypeId() == TYPEID_CORPSE || objB->GetTypeId() == TYPEID_CORPSE )
		return false;

	// We do need all factiondata for this
	if( objB->m_factionDBC == NULL || objA->m_factionDBC == NULL || objB->m_faction == NULL || objA->m_faction == NULL )
		return false;

	if(!objA->PhasedCanInteract(objB))
		return false;

	bool combatSupport = false;

	uint32 fSupport = objB->m_faction->FriendlyMask;
	uint32 myFaction = objA->m_faction->Mask;

	if(myFaction & fSupport)
		combatSupport = true;

	// check friend/enemy list
	for(uint32 i = 0; i < 4; i++)
	{
		if(objB->m_faction->EnemyFactions[i] == objA->m_faction->Faction)
		{
			combatSupport = false;
			break;
		}
		if(objB->m_faction->FriendlyFactions[i] == objA->m_faction->Faction)
		{
			combatSupport = true;
			break;
		}
	}
	return combatSupport;
}


bool isAlliance(Object* objA)// A is alliance?
{
	if(!objA || objA->m_factionDBC == NULL || objA->m_faction == NULL)
		return true;

	//Get stormwind faction frm dbc (11/72)
	FactionTemplateDBC * m_sw_faction = dbcFactionTemplate.LookupEntry(11);
	FactionDBC * m_sw_factionDBC = dbcFaction.LookupEntry(72);

	if(m_sw_faction == objA->m_faction || m_sw_factionDBC == objA->m_factionDBC)
		return true;

	//Is StormWind hostile to ObjectA?
	uint32 faction = m_sw_faction->Faction;
	uint32 hostilemask = objA->m_faction->HostileMask;

	if(faction & hostilemask)
		return false;

	// check friend/enemy list
	for(uint32 i = 0; i < 4; i++)
	{
		if(objA->m_faction->EnemyFactions[i] == faction)
			return false;
	}

	//Is ObjectA hostile to StormWind?
	faction = objA->m_faction->Faction;
	hostilemask = m_sw_faction->HostileMask;

	if(faction & hostilemask)
		return false;

	// check friend/enemy list
	for(uint32 i = 0; i < 4; i++)
	{
		if(objA->m_faction->EnemyFactions[i] == faction)
			return false;
	}

	//We're not hostile towards SW, so we are allied
	return true;
}


