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

#ifndef WOWSERVER_CREATURE_H
#define WOWSERVER_CREATURE_H

class CreatureTemplate;
class GossipScript;

#define MAX_CREATURE_ITEMS 128
#define MAX_CREATURE_LOOT 8
#define MAX_PET_SPELL 4
#define VENDOR_ITEMS_UPDATE_TIME 3600000

struct CreatureItem
{
	uint32 itemid;
	uint32 amount; //!!!!! stack amount.
	uint32 available_amount;
	uint32 max_amount;
	uint32 incrtime;
	uint32 vendormask;
	ItemExtendedCostEntry *extended_cost;
	bool IsDependent;
};

SERVER_DECL bool Rand(float chance);
SERVER_DECL bool Rand(uint32 chance);
SERVER_DECL bool Rand(int32 chance);
#pragma pack(push,1)
struct CreatureInfo
{
	uint32 Id;
	char * Name;
	char * SubName;
	char * info_str;
	uint32 Flags1;
	uint32 Type;
	uint32 TypeFlags;
	uint32 Family;
	uint32 Rank;
	uint32 Unknown1;
	uint32 SpellDataID;
	uint32 Male_DisplayID;
	uint32 Female_DisplayID;
	uint32 Male_DisplayID2;
	uint32 Female_DisplayID2;
	float unkfloat1; // Something to do with elites.
	float unkfloat2; // Something to do with unkfloat1.
	uint8  Civilian;
	uint8  Leader;

	std::string lowercase_name;

	uint32 GenerateModelId(uint32 * dest)
	{
		uint32 models[] = { Male_DisplayID, Male_DisplayID2, Female_DisplayID, Female_DisplayID2 };
		if(!models[0] && !models[1] && !models[2] && !models[3])
		{
			// All models are invalid.
			Log.Warning("CreatureSpawn", "Creature %u has no model_id", Id);
			return 0;
		}

		uint32 modelchoices[4];
		uint32 max = 0;

		if( Male_DisplayID )
		{
			modelchoices[max] = Male_DisplayID;
			++max;
		}

		if( Male_DisplayID2 )
		{
			modelchoices[max] = Male_DisplayID2;
			++max;
		}

		if( Female_DisplayID )
		{
			modelchoices[max] = Female_DisplayID;
			++max;
		}

		if( Female_DisplayID2 )
		{
			modelchoices[max] = Female_DisplayID2;
			++max;
		}

		uint32 r = RandomUInt(max ? max - 1 : 0);
		*dest = modelchoices[r];

		if( *dest == Male_DisplayID || *dest == Male_DisplayID2 )
			return 0;

		return 1;
	}
};

struct TeleportInfo
{
//	uint32 entry;
	uint8 intid;
	uint8 iconid;
	bool teleport;
	string textinfo;
	uint32 teleportmapid;
	float teleportx;
	float teleporty;
	float teleportz;
	float teleporto;
	uint32 castspellid;
	uint32 removetargetaura;
	bool sndchtmessage;
	uint8 messagetype;
	string Message;
};

struct CreatureProtoMode
{
//	uint32 entry; // Woot, not needed inside core.
	uint32 Minlevel;
	uint32 Maxlevel;
	uint32 Minhealth;
	uint32 Maxhealth;
	float Mindmg;
	float Maxdmg;
	uint32 Power;
	uint32 Resistances[7];
//	char * aura_string; // We just convert directly.
	uint32 auraimmune_flag;

	set<uint32> start_auras;
};

struct CreatureProto
{
	uint32 Id;
	uint32 MinLevel;
	uint32 MaxLevel;
	uint32 Faction;
	uint32 MinHealth;
	uint32 MaxHealth;
	uint8 Powertype;
	uint32 Power;
	float  Scale;
	uint32	NPCFLags;
	uint32 AttackTime;
	uint32 AttackType;
	float MinDamage;
	float MaxDamage;
	uint32 RangedAttackTime;
	float RangedMinDamage;
	float RangedMaxDamage;
	uint32 Item1;
	uint32 Item2;
	uint32 Item3;
	uint32 RespawnTime;
	uint32 Resistances[7];
	float CombatReach;
	float BoundingRadius;
	char* aura_string;
	uint32 boss;
	int32 money;
	uint32 invisibility_type;
	uint32 death_state;
	float walk_speed;//base movement
	float run_speed;//most of the time mobs use this
	float fly_speed;
	uint32 extra_a9_flags;
	uint32 auraimmune_flag;
	int32 vehicle_entry;
	uint32 BattleMasterType;
	uint32 SpellClickid;
	uint32 CanMove;

	set<uint32> start_auras;
	list<AI_Spell*> spells[4];
	HM_NAMESPACE::hash_map<uint8, CreatureProtoMode*> ModeProto;
	std::set<TeleportInfo*> TeleportInfoList;
};

struct SeatInfo
{
	uint32 accessoryentry;
	bool ejectfromvehicleondeath;
	bool unselectableaccessory;
};

struct CreatureProtoVehicle
{
	uint32 vehicle_creature_entry; // Entry.
	bool healthfromdriver; // Effects only driver.
	uint32 healthunitfromitemlev;
	uint32 VehicleSpells[6]; // Vehicle spells.
	uint32 MovementFlags;
	SeatInfo seats[8]; // Accessories.
};

struct CreatureInfoExtra
{
	uint32 entry;
	uint32 default_emote_state;
	uint32 default_flags;
	uint16 default_stand_state;
	uint32 default_MountedDisplayID;
	bool   m_canRangedAttack;
	bool   m_canCallForHelp;
	float  m_callForHelpHP;
	bool   m_canFlee;
	float  m_fleeHealth;
	uint32 m_fleeDuration;
	int    sendflee_message;
	string flee_message;
	bool   no_skill_up;
	bool   no_xp;
};

#pragma pack(pop)

struct Formation{
	uint32 fol;
	float ang;
	float dist;
};

enum UNIT_TYPE
{
	NOUNITTYPE		= 0,
	BEAST			= 1,
	DRAGONSKIN		= 2,
	DEMON			= 3,
	ELEMENTAL		= 4,
	GIANT			= 5,
	UNDEAD			= 6,
	HUMANOID		= 7,
	CRITTER			= 8,
	MECHANICAL		= 9,
	UNIT_TYPE_MISC	= 10,
	UNIT_TYPE_TOTEM = 11,
	UNIT_TYPE_NONCOMBAT_PET = 12,
	UNIT_TYPE_GAS_CLOUD = 13,
};

enum FAMILY
{
	FAMILY_WOLF				= 1,
	FAMILY_CAT,
	FAMILY_SPIDER,
	FAMILY_BEAR,
	FAMILY_BOAR,
	FAMILY_CROCILISK,
	FAMILY_CARRION_BIRD,
	FAMILY_CRAB,
	FAMILY_GORILLA,
	FAMILY_RAPTOR			= 11,
	FAMILY_TALLSTRIDER ,
	FAMILY_FELHUNTER		= 15,
	FAMILY_VOIDWALKER,
	FAMILY_SUCCUBUS,
	FAMILY_DOOMGUARD		= 19,
	FAMILY_SCORPID,
	FAMILY_TURTLE,
	FAMILY_IMP				= 23,
	FAMILY_BAT,
	FAMILY_HYENA,
	FAMILY_BIRD_OF_PREY,
	FAMILY_WIND_SERPENT,
	FAMILY_REMOTE_CONTROL,
	FAMILY_FELGUARD,
	FAMILY_DRAGONHAWK,
	FAMILY_RAVAGER,
	FAMILY_WARP_STALKER,
	FAMILY_SPOREBAT,
	FAMILY_NETHER_RAY,
	FAMILY_SERPENT,
	FAMILY_MOTH				= 37,
	FAMILY_CHIMAERA,
	FAMILY_DEVILSAUR,
	FAMILY_GHOUL,
	FAMILY_SILITHID,
	FAMILY_WORM,
	FAMILY_RHINO,
	FAMILY_WASP,
	FAMILY_CORE_HOUND,
	FAMILY_SPIRIT_BEAST,
	FAMILY_FAKE_IMP			= 416,
	FAMILY_FAKE_FELHUNTER,
	FAMILY_FAKE_VOIDWALKER	= 1860,
	FAMILY_FAKE_SUCCUBUS	= 1863,
	FAMILY_FAKE_FELGUARD	= 17252
};

enum CreatureFlags1
{
	CREATURE_FLAGS1_TAMEABLE   = 0x0001,
	CREATURE_FLAGS1_BOSS	   = 0x6C,
	CREATURE_FLAGS1_HERBLOOT   = 0x0100,
	CREATURE_FLAGS1_MININGLOOT = 0x0200,
	CREATURE_FLAGS1_ENGINEERLOOT = 0x08000,
};


enum ELITE
{
	ELITE_NORMAL = 0,
	ELITE_ELITE,
	ELITE_RAREELITE,
	ELITE_WORLDBOSS,
	ELITE_RARE
};

enum TIME_REMOVE_CORPSE
{
	TIME_CREATURE_REMOVE_CORPSE = 180000,
	TIME_CREATURE_REMOVE_RARECORPSE = 180000*3,
	TIME_CREATURE_REMOVE_BOSSCORPSE = 180000*5,
};

struct PetSpellCooldown
{
	uint32 spellId;
	int32 cooldown;
};

class CreatureAIScript;
class GossipScript;
class AuctionHouse;
struct Trainer;
#define CALL_SCRIPT_EVENT(obj, func) if(obj->GetTypeId() == TYPEID_UNIT && TO_CREATURE(obj)->GetScript() != NULL) TO_CREATURE(obj)->GetScript()->func

///////////////////
/// Creature object

class SERVER_DECL Creature : public Unit
{
public:
	friend class Player;

	Creature(uint64 guid);
	virtual ~Creature();
	virtual void Init();
	virtual void Destruct();

	bool Load(CreatureSpawn *spawn, uint32 mode, MapInfo *info);
	void Load(CreatureProto * proto_, uint32 mode, float x, float y, float z, float o = 0.0f);

	void AddToWorld();
	void AddToWorld(MapMgr* pMapMgr);
	void RemoveFromWorld(bool addrespawnevent, bool free_guid);

	/// Creation
	void Create ( const char* creature_name, uint32 mapid, float x, float y, float z, float ang);
	void CreateWayPoint ( uint32 WayPointID, uint32 mapid, float x, float y, float z, float ang);

	/// Arena organizers
	HEARTHSTONE_INLINE bool ArenaOrganizersFlags() const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TABARDCHANGER ); }

	/// Updates
	virtual void Update( uint32 time );

	HEARTHSTONE_INLINE uint32 GetSQL_id() { return spawnid; };

	/// Creature inventory
	HEARTHSTONE_INLINE uint32 GetItemIdBySlot(uint32 slot) { return m_SellItems->at(slot).itemid; }
	HEARTHSTONE_INLINE uint32 GetItemAmountBySlot(uint32 slot) { return m_SellItems->at(slot).amount; }

	HEARTHSTONE_INLINE bool HasItems() { return ((m_SellItems != NULL) ? true : false); }

	int32 GetSlotByItemId(uint32 itemid)
	{
		uint32 slot = 0;
		for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); itr++)
		{
			if(itr->itemid == itemid)
				return slot;
			else
				++slot;
		}
		return -1;
	}

	uint32 GetItemAmountByItemId(uint32 itemid)
	{
		for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); itr++)
		{
			if(itr->itemid == itemid)
				return ((itr->amount < 1) ? 1 : itr->amount);
		}
		return 0;
	}

	HEARTHSTONE_INLINE void GetSellItemBySlot(uint32 slot, CreatureItem &ci)
	{
		ci = m_SellItems->at(slot);
	}

	void GetSellItemByItemId(uint32 itemid, CreatureItem &ci)
	{
		for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); itr++)
		{
			if(itr->itemid == itemid)
			{
				ci = (*itr);
				return;
			}
		}
		ci.amount = 0;
		ci.max_amount=0;
		ci.available_amount =0;
		ci.incrtime=0;
		ci.itemid = 0;
	}

	HEARTHSTONE_INLINE std::vector<CreatureItem>::iterator GetSellItemBegin() { return m_SellItems->begin(); }
	HEARTHSTONE_INLINE std::vector<CreatureItem>::iterator GetSellItemEnd()   { return m_SellItems->end(); }
	HEARTHSTONE_INLINE size_t GetSellItemCount() { return m_SellItems->size(); }
	void RemoveVendorItem(uint32 itemid)
	{
		for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); itr++)
		{
			if(itr->itemid == itemid)
			{
				m_SellItems->erase(itr);
				return;
			}
		}
	}

	void AddVendorItem(uint32 itemid, uint32 amount, uint32 ec = 0);
	void ModAvItemAmount(uint32 itemid, uint32 value);
	void UpdateItemAmount(uint32 itemid);

	/// Quests
	void _LoadQuests();
	bool HasQuests() { return m_quests != NULL; };
	bool HasQuest(uint32 id, uint32 type)
	{
		if(!m_quests) return false;
		for(std::list<QuestRelation*>::iterator itr = m_quests->begin(); itr != m_quests->end(); itr++)
		{
			if((*itr)->qst->id == id && (*itr)->type & type)
				return true;
		}
		return false;
	}

	void AddQuest(QuestRelation *Q);
	void DeleteQuest(QuestRelation *Q);
	Quest *FindQuest(uint32 quest_id, uint8 quest_relation);
	uint16 GetQuestRelation(uint32 quest_id);
	uint32 NumOfQuests();
	list<QuestRelation *>::iterator QuestsBegin() { return m_quests->begin(); };
	list<QuestRelation *>::iterator QuestsEnd() { return m_quests->end(); };
	void SetQuestList(std::list<QuestRelation *>* qst_lst) { m_quests = qst_lst; };

	HEARTHSTONE_INLINE void SetSheatheForAttackType(uint8 type) { SetByte(UNIT_FIELD_BYTES_2, 0, type); }
	HEARTHSTONE_INLINE bool isQuestGiver() { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER ); };

	void CalcResistance(uint32 type);
	void CalcStat(uint32 type);
	void RegenerateHealth(bool isinterrupted);
	void RegenerateMana(bool isinterrupted);

	bool CanSee(Unit* obj) // * Invisibility & Stealth Detection - Partha *
	{
		if(!obj)
			return false;

		if(obj->m_invisible) // Invisibility - Detection of Players and Units
		{
			if(obj->getDeathState() == CORPSE) // can't see dead players' spirits
				return false;

			if(m_invisDetect[obj->m_invisFlag] < 1) // can't see invisible without proper detection
				return false;
		}

		if(obj->InStealth()) // Stealth Detection (  I Hate Rogues :P  )
		{
			if(isInFront(obj)) // stealthed player is in front of creature
			{
				int32 hide_level = (getLevel() * 5 + GetStealthDetectBonus()) - obj->GetStealthLevel();
				detectRange = (hide_level * 0.15f) + 5;

				if(detectRange < 1.0f)
					detectRange = 1.0f; // Minimum Detection Range = 1yd
			}
			else // stealthed player is behind creature
			{
				if(GetStealthDetectBonus() > 500) return true; // immune to stealth
				else detectRange = 0.0f;
			}

			detectRange += GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS); // adjust range for size of creature
			detectRange += obj->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS); // adjust range for size of stealthed player

			if(GetDistance2dSq(obj) > detectRange * detectRange)
				return false;
		}

		return true;
	}

	//Make this unit face another unit
	bool setInFront(Unit* target);

	bool Skinned;

	/// Misc
	HEARTHSTONE_INLINE void setEmoteState(uint8 emote) { m_emoteState = emote; };

	void CreatureSetDeathState(DeathState s);

	uint32 GetOldEmote() { return m_oldEmote; }

	// Serialization
	void SaveToDB(bool saveposition = false);
	void LoadAIAgents(CreatureTemplate * t);
	void LoadAIAgents();
	void DeleteFromDB();

	void OnJustDied();
	void OnRemoveCorpse();
	void OnRespawn(MapMgr* m);
	void SafeDelete();

	// In Range
	void AddInRangeObject(Object* pObj);
	void OnRemoveInRangeObject(Object* pObj);
	void ClearInRangeSet();

	// Demon
	void EnslaveExpire();

	// Pet
	void UpdatePet();
	uint32 GetEnslaveCount() { return m_enslaveCount; }
	void SetEnslaveCount(uint32 count) { m_enslaveCount = count; }
	uint32 GetEnslaveSpell() { return m_enslaveSpell; }
	void SetEnslaveSpell(uint32 spellId) { m_enslaveSpell = spellId; }
	bool RemoveEnslave();

	//General vars for summoned creatures
	Unit* GetSummonOwner();
	HEARTHSTONE_INLINE int32 GetSummonSlot() { return SummonSlot; }
	HEARTHSTONE_INLINE bool IsSummon() { return SummonOwner && SummonSlot > -1; }
	HEARTHSTONE_INLINE void SetSummonOwnerSlot(uint64 ownerguid, int8 slot) { SummonOwner = ownerguid; SummonSlot = slot;}

	//Special summons, Totems
	HEARTHSTONE_INLINE bool IsTotem() { return Totem;}
	HEARTHSTONE_INLINE void SetTotem(bool totem) { Totem = totem;}

	HEARTHSTONE_INLINE bool IsPickPocketed() { return m_PickPocketed; }
	HEARTHSTONE_INLINE void SetPickPocketed(bool val = true) { m_PickPocketed = val; }

	HEARTHSTONE_INLINE CreatureAIScript * GetScript() { return _myScriptClass; }
	void LoadScript();

	void CallScriptUpdate();

	uint32 m_TaxiNode;
	CreatureInfo *creature_info;
	const char* GetName()
	{
		return creature_info->Name;
	}
	// left this function for backwards compatibility with scripts
	// please use GetCreatureInfo()
	HEARTHSTONE_INLINE CreatureInfo *GetCreatureInfo()
	{
		return creature_info;
	}
	HEARTHSTONE_INLINE void SetCreatureName(CreatureInfo *ci) { creature_info = ci; }
	HEARTHSTONE_INLINE Trainer* GetTrainer() { return mTrainer; }
	void RegenerateFocus();
	void RegenerateEnergy();

	CreatureFamilyEntry * myFamily;
	void FormationLinkUp(uint32 SqlId);
	void ChannelLinkUpGO(uint32 SqlId);
	void ChannelLinkUpCreature(uint32 SqlId);
	WayPoint * CreateWaypointStruct();
	CreatureProto * proto;
	HEARTHSTONE_INLINE CreatureProto *GetProto() { return proto; }
	CreatureProtoMode* LoadedProto;
	HEARTHSTONE_INLINE CreatureProtoMode *GetLoadedProto() { return LoadedProto; }
	CreatureInfoExtra * ExtraInfo;
	HEARTHSTONE_INLINE CreatureInfoExtra *GetExtraInfo() { return ExtraInfo; }
	uint32 GetRespawnTime() { if(proto != NULL) return proto->RespawnTime; else return 0; }
	CreatureSpawn * m_spawn;
	EventIdInfo * m_event;
	void OnPushToWorld();
	void Despawn(uint32 delay, uint32 respawntime);
	void TriggerScriptEvent(int);

	AuctionHouse * auctionHouse;

	void DeleteMe();
	bool CanAddToWorld();

	WayPointMap * m_custom_waypoint_map;
	Player* m_escorter;
	void DestroyCustomWaypointMap();
	bool IsInLimboState() { return m_limbostate; }
	uint32 GetLineByFamily(CreatureFamilyEntry * family){return family->skilline ? family->skilline : 0;};
	void RemoveLimboState(Unit* healer);
	void SetGuardWaypoints();
	MapCell * m_respawnCell;
	uint32 GetCanMove()
	{
		if(m_spawn)
			return m_spawn->CanMove;
		else if(proto)
			return proto->CanMove;

		return 1;
	};

	bool HasNpcFlag(uint32 Flag)
	{
		if(GetUInt32Value(UNIT_NPC_FLAGS) & Flag)
			return true;
		return false;
	}

protected:
	CreatureAIScript *_myScriptClass;
	bool m_limbostate;
	Trainer* mTrainer;

	void _LoadGoods();
	void _LoadGoods(std::list<CreatureItem*>* lst);
	void _LoadMovement();

	/// Vendor data
	std::vector<CreatureItem>* m_SellItems;

	/// Taxi data
	uint32 mTaxiNode;

	/// Quest data
	std::list<QuestRelation *>* m_quests;

	/// Pet
	uint32 m_enslaveCount;
	uint32 m_enslaveSpell;

	uint32 SummonOwner;
	int32 SummonSlot;
	bool Totem;

	bool m_PickPocketed;
	uint32 _fields[UNIT_END];
public:
	//	custom functions for scripting
	HEARTHSTONE_INLINE uint32 GetProtoItemDisplayId(uint8 i) { return GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + i); }

	// loooooot
	void GenerateLoot();
	uint32 GetRequiredLootSkill();

	// updates the loot state, whether it is tagged or lootable, or no longer has items
	void UpdateLootAnimation(Player* Looter);

	// clears tag, clears "tagged" visual grey
	void ClearTag();

	// tags the object by a certain player.
	void Tag(Player* plr);

	// used by bgs
	bool IsLightwell(uint32 entry) { if(GetEntry() == 31883 || GetEntry() == 31893 || GetEntry() == 31894 || GetEntry() == 31895 || GetEntry() == 31896 || GetEntry() == 31897) return true; return false; }

	ItemPrototype* IP_shield;

public: // values
	bool b_has_shield;
	bool m_corpseEvent;
	bool m_noRespawn;
	bool has_waypoint_text;
	bool has_combat_text;
	bool haslinkupevent;
	bool m_canRegenerateHP;
	bool m_noDeleteAfterDespawn;
	uint32 m_taggingGroup;
	uint32 m_taggingPlayer;
	uint32 spawnid;
	uint32 original_emotestate;
	uint32 original_MountedDisplayID;
	int8 BaseAttackType;
	int8 m_lootMethod;
	int32 FlatResistanceMod[7];
	int32 BaseResistanceModPct[7];
	int32 ResistanceModPct[7];
	int32 FlatStatMod[5];
	int32 StatModPct[5];
	int32 TotalStatModPct[5];
};

#endif
