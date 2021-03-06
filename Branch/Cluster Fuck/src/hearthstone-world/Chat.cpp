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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"

initialiseSingleton( ChatHandler );
initialiseSingleton(CommandTableStorage);

ChatCommand * ChatHandler::getCommandTable()
{
	ASSERT(false);
	return 0;
}

ChatCommand * CommandTableStorage::GetSubCommandTable(const char * name)
{
	if(!stricmp(name, "modify"))
		return _modifyCommandTable;
	else if(!stricmp(name, "debug"))
		return _debugCommandTable;
	else if(!stricmp(name, "waypoint"))
		return _waypointCommandTable;
	else if(!stricmp(name, "ticket"))
		return _GMTicketCommandTable;
	else if(!stricmp(name, "gobject"))
		return _GameObjectCommandTable;
	else if(!stricmp(name, "battleground"))
		return _BattlegroundCommandTable;
	else if(!stricmp(name, "npc"))
		return _NPCCommandTable;
	else if(!stricmp(name, "gm"))
		return _gamemasterCommandTable;
	else if(!stricmp(name, "tracker"))
		return _trackerCommandTable;
	else if(!stricmp(name, "warn"))
		return _warnCommandTable;
	else if(!stricmp(name, "admin"))
		return _administratorCommandTable;
	else if(!stricmp(name, "cheat"))
		return _CheatCommandTable;
	else if(!stricmp(name, "account"))
		return _accountCommandTable;
	else if(!stricmp(name, "pet"))
		return _petCommandTable;
	else if(!stricmp(name, "recall"))
		return _recallCommandTable;
	else if(!stricmp(name, "honor"))
		return _honorCommandTable;
	else if(!stricmp(name, "guild"))
		return _GuildCommandTable;
	else if(!stricmp(name, "title"))
		return _TitleCommandTable;
	else if(!stricmp(name, "quest"))
		return _questCommandTable;
	else if(!stricmp(name, "lookup"))
		return _lookupCommandTable;
	else if(!stricmp(name, "wintergrasp"))
		return _wintergraspCommandTable;
	else if(!stricmp(name, "faction"))
		return _FactionCommandTable;

	// DB command Start
	else if(!stricmp(name, "database"))
		return _DatabaseCommandTable;
	else if(!stricmp(name, "item"))
		return _DBItemCommandTable;
	return 0;
}

#define dupe_command_table(ct, dt) this->dt = (ChatCommand*)allocate_and_copy(sizeof(ct)/* / sizeof(ct[0])*/, ct)
HEARTHSTONE_INLINE void* allocate_and_copy(uint32 len, void * pointer)
{
	void * data = (void*)malloc(len);
	memcpy(data, pointer, len);
	return data;
}

void CommandTableStorage::Load()
{
	QueryResult * result = WorldDatabase.Query("SELECT * FROM command_overrides");
	if(!result) return;

	do
	{
		const char * name = result->Fetch()[0].GetString();
		const char * level = result->Fetch()[1].GetString();
		Override(name, level);
	} while(result->NextRow());
	delete result;
}

void CommandTableStorage::Override(const char * command, const char * level)
{
	ASSERT(level[0] != '\0');
	char * cmd = strdup(command);

	// find the command we're talking about
	char * sp = strchr(cmd, ' ');
	const char * command_name = cmd;
	const char * subcommand_name = 0;

	if(sp != 0)
	{
		// we're dealing with a subcommand.
		*sp = 0;
		subcommand_name = sp + 1;
	}

	size_t len1 = strlen(command_name);
	size_t len2 = subcommand_name ? strlen(subcommand_name) : 0;

	// look for the command.
	ChatCommand * p = &_commandTable[0];
	while(p->Name != 0)
	{
		if(!strnicmp(p->Name, command_name, len1))
		{
			// this is the one we wanna modify
			if(!subcommand_name)
			{
				// no subcommand, we can change it.
				p->CommandGroup = level[0];
				printf("Changing command level of command `%s` to %c.\n", p->Name, level[0]);
			}
			else
			{
				// assume this is a subcommand, loop the second set.
				ChatCommand * p2 = p->ChildCommands;
				if(!p2)
				{
					printf("Invalid command specified for override: %s\n", command_name);
				}
				else
				{
					while(p2->Name != 0)
					{
						if(!strnicmp("*",subcommand_name,1))
						{
								p2->CommandGroup = level[0];
								printf("Changing command level of command (wildcard) `%s`:`%s` to %c.\n", p->Name, p2->Name, level[0]);
						}else{
							if(!strnicmp(p2->Name, subcommand_name, len2))
							{
								// change the level
								p2->CommandGroup = level[0];
								printf("Changing command level of command `%s`:`%s` to %c.\n", p->Name, p2->Name, level[0]);
								break;
							}
						}
						p2++;
					}
					if(p2->Name == 0)
					{
						if(strnicmp("*",subcommand_name,1)) //Hacky.. meh.. -DGM
						{
							printf("Invalid subcommand referenced: `%s` under `%s`.\n", subcommand_name, p->Name);
						}
						break;
					}
				}
			}
			break;
		}
		++p;
	}

	if(p->Name == 0)
	{
		printf("Invalid command referenced: `%s`\n", command_name);
	}

	free(cmd);
}

void CommandTableStorage::Dealloc()
{
	free( _modifyCommandTable );
	free( _debugCommandTable );
	free( _DBItemCommandTable );
	free( _DatabaseCommandTable );
	free( _waypointCommandTable );
	free( _GMTicketCommandTable );
	free( _GameObjectCommandTable );
	free( _BattlegroundCommandTable );
	free( _NPCCommandTable );
	free( _gamemasterCommandTable );
	free( _trackerCommandTable );
	free( _warnCommandTable );
	free( _administratorCommandTable );
	free( _CheatCommandTable );
	free( _accountCommandTable );
	free( _petCommandTable );
	free( _recallCommandTable );
	free( _honorCommandTable );
	free( _GuildCommandTable);
	free( _TitleCommandTable);
	free( _questCommandTable );
	free( _lookupCommandTable );
	free(_wintergraspCommandTable);
	free( _commandTable );
}

void CommandTableStorage::Init()
{
	static ChatCommand modifyCommandTable[] =
	{
		{ "hp",					'm', NULL,											"Health Points/HP",			NULL, UNIT_FIELD_HEALTH,			UNIT_FIELD_MAXHEALTH,	1 },
		{ "gender",				'm', &ChatHandler::HandleGenderChanger,				"Changes gender",			NULL, 0,							0,						0 },
		{ "mana",				'm', NULL,											"Mana Points/MP",			NULL, UNIT_FIELD_POWER1,			UNIT_FIELD_MAXPOWER1,	1 },
		{ "rage",				'm', NULL,											"Rage Points",				NULL, UNIT_FIELD_POWER2,			UNIT_FIELD_MAXPOWER2,	1 },
		{ "runicpower",			'm', NULL,											"Runic Power",				NULL, UNIT_FIELD_POWER7,			UNIT_FIELD_MAXPOWER7,	1 },
		{ "energy",				'm', NULL,											"Energy Points",			NULL, UNIT_FIELD_POWER4,			UNIT_FIELD_MAXPOWER4,	1 },
		{ "level",				'm', &ChatHandler::HandleModifyLevelCommand,		"Level",					NULL, 0,							0,						0 },
		{ "armor",				'm', NULL,											"Armor",					NULL, UNIT_FIELD_STAT1,				0,						1 },
		{ "holy",				'm', NULL,											"Holy Resistance",			NULL, UNIT_FIELD_RESISTANCES+1,		0,						1 },
		{ "fire",				'm', NULL,											"Fire Resistance",			NULL, UNIT_FIELD_RESISTANCES+2,		0,						1 },
		{ "nature",				'm', NULL,											"Nature Resistance",		NULL, UNIT_FIELD_RESISTANCES+3,		0,						1 },
		{ "frost",				'm', NULL,											"Frost Resistance",			NULL, UNIT_FIELD_RESISTANCES+4,		0,						1 },
		{ "shadow",				'm', NULL,											"Shadow Resistance",		NULL, UNIT_FIELD_RESISTANCES+5,		0,						1 },
		{ "arcane",				'm', NULL,											"Arcane Resistance",		NULL, UNIT_FIELD_RESISTANCES+6,		0,						1 },
		{ "damage",				'm', NULL,											"Unit Damage Min/Max",		NULL, UNIT_FIELD_MINDAMAGE,			UNIT_FIELD_MAXDAMAGE,	2 },
		{ "scale",				'm', &ChatHandler::HandleModifyScaleCommand,		"Size/Scale",				NULL, 0,							0,						2 },
		{ "gold",				'm', &ChatHandler::HandleModifyGoldCommand,			"Gold/Money/Copper",		NULL, 0,							0,						0 },
		{ "speed",				'm', &ChatHandler::HandleModifySpeedCommand,		"Movement Speed",			NULL, 0,							0,						0 },
		{ "nativedisplayid",	'm', NULL,											"Native Display ID",		NULL, UNIT_FIELD_NATIVEDISPLAYID,	0,						1 },
		{ "displayid",			'm', NULL,											"Display ID",				NULL, UNIT_FIELD_DISPLAYID,			0,						1 },
		{ "flags",				'm', NULL,											"Unit Flags",				NULL, UNIT_FIELD_FLAGS,				0,						1 },
		{ "faction",			'm', &ChatHandler::HandleModifyFactionCommand,		"Faction Template",			NULL, 0,							0,						1 },
		{ "dynamicflags",		'm', NULL,											"Dynamic Flags",			NULL, UNIT_DYNAMIC_FLAGS,			0,						1 },
		{ "talentpoints",		'm', &ChatHandler::HandleModifyTPsCommand,			"Talent points",			NULL, 0,							0,						0 },
		{ "happiness",			'm', NULL,											"Happiness",				NULL, UNIT_FIELD_POWER5,			UNIT_FIELD_MAXPOWER5,	1 },
		{ "spirit",				'm', NULL,											"Spirit",					NULL, UNIT_FIELD_STAT0,				0,						1 },
		{ "boundingraidius",	'm', NULL,											"Bounding Radius",			NULL, UNIT_FIELD_BOUNDINGRADIUS,	0,						2 },
		{ "combatreach",		'm', NULL,											"Combat Reach",				NULL, UNIT_FIELD_COMBATREACH,		0,						2 },
		{ "bytes",				'm', NULL,											"Bytes",					NULL, UNIT_FIELD_BYTES_0,			0,						1 },
		{ "playerflags",		'm', &ChatHandler::HandleModifyPlayerFlagsCommand,	"modify a player's flags",	NULL, 0,							0,						0 },
		{ "aurastate",			'm', &ChatHandler::HandleModifyAuraStateCommand,	"mods player's aurastate",	NULL, 0,							0,						0 },
		{ NULL,					'0', NULL,											"",							NULL, 0,							0,						0 }
	};
	dupe_command_table(modifyCommandTable, _modifyCommandTable);

	static ChatCommand warnCommandTable[] =
	{
		{ "add",				'a', &ChatHandler::HandleWarnPlayerCommand,		"Warns a player, Syntax: !warn add <playername> <reason>",						NULL, 0, 0, 0 },
		{ "list",				'0', &ChatHandler::HandleWarnListCommand,		"Warns a player, Syntax: !warn list <playername>",								NULL, 0, 0, 0 },
		{ "clearall",			'z', &ChatHandler::HandleWarnClearCommand,		"Clears warns from a player, Syntax: !warn clear <playername>",					NULL, 0, 0, 0 },
		{ "delete",				'a', &ChatHandler::HandleWarnSingleDelCommand,	"Deletes a warn from a player, Syntax: !warn delete <playername> <WarnID>",		NULL, 0, 0, 0 },
		{ NULL,					'0', NULL,										"",																				NULL, 0, 0, 0 }
	};
	dupe_command_table(warnCommandTable, _warnCommandTable);

	static ChatCommand debugCommandTable[] =
	{
		{ "retroactivequest",		'd', &ChatHandler::HandleDebugRetroactiveQuestAchievements,		"",																														NULL, 0, 0, 0 },
		{ "setphase",				'd', &ChatHandler::HandleDebugSetPhase,							"",																														NULL, 0, 0, 0 },
		{ "infront",				'd', &ChatHandler::HandleDebugInFrontCommand,					"",																														NULL, 0, 0, 0 },
		{ "showreact",				'd', &ChatHandler::HandleShowReactionCommand,					"",																														NULL, 0, 0, 0 },
		{ "aimove",					'd', &ChatHandler::HandleAIMoveCommand,							"",																														NULL, 0, 0, 0 },
		{ "dist",					'd', &ChatHandler::HandleDistanceCommand,						"",																														NULL, 0, 0, 0 },
		{ "face",					'd', &ChatHandler::HandleFaceCommand,							"",																														NULL, 0, 0, 0 },
		{ "moveinfo",				'd', &ChatHandler::HandleMoveInfoCommand,						"",																														NULL, 0, 0, 0 },
		{ "setbytes",				'd', &ChatHandler::HandleSetBytesCommand,						"",																														NULL, 0, 0, 0 },
		{ "getbytes",				'd', &ChatHandler::HandleGetBytesCommand,						"",																														NULL, 0, 0, 0 },
		{ "unroot",					'd', &ChatHandler::HandleDebugUnroot,							"",																														NULL, 0, 0, 0 },
		{ "root",					'd', &ChatHandler::HandleDebugRoot,								"",																														NULL, 0, 0, 0 },
		{ "landwalk",				'd', &ChatHandler::HandleDebugLandWalk,							"",																														NULL, 0, 0, 0 },
		{ "waterwalk",				'd', &ChatHandler::HandleDebugWaterWalk,						"",																														NULL, 0, 0, 0 },
		{ "castspellne",			'd', &ChatHandler::HandleCastSpellNECommand,					".castspellne <spellid> - Casts spell on target (only plays animations, doesnt handle effects or range/facing/etc.",	NULL, 0, 0, 0 },
		{ "aggrorange",				'd', &ChatHandler::HandleAggroRangeCommand,						".aggrorange - Shows aggro Range of the selected Creature.",															NULL, 0, 0, 0 },
		{ "knockback ",				'd', &ChatHandler::HandleKnockBackCommand,						".knockback <hspeed> <vspeed> - Knocks selected player back.",															NULL, 0, 0, 0 },
		{ "fade ",					'd', &ChatHandler::HandleFadeCommand,							".fade <value> - calls ModThreatModifyer().",																			NULL, 0, 0, 0 },
		{ "threatMod ",				'd', &ChatHandler::HandleThreatModCommand,						".threatMod <value> - calls ModGeneratedThreatModifyer().",																NULL, 0, 0, 0 },
		{ "calcThreat ",			'd', &ChatHandler::HandleCalcThreatCommand,						".calcThreat <dmg> <spellId> - calculates threat.",																		NULL, 0, 0, 0 },
		{ "threatList ",			'd', &ChatHandler::HandleThreatListCommand,						".threatList - returns all AI_Targets of the selected Creature.",														NULL, 0, 0, 0 },
		{ "gettptime",				'd', &ChatHandler::HandleGetTransporterTime,					"grabs transporter travel time",																						NULL, 0, 0, 0 },
		{ "itempushresult",			'd', &ChatHandler::HandleSendItemPushResult,					"sends item push result",																								NULL, 0, 0, 0 },
		{ "setbit",					'd', &ChatHandler::HandleModifyBitCommand,						"",																														NULL, 0, 0, 0 },
		{ "setvalue",				'd', &ChatHandler::HandleModifyValueCommand,					"",																														NULL, 0, 0, 0 },
		{ "aispelltestbegin",		'd', &ChatHandler::HandleAIAgentDebugBegin,						"",																														NULL, 0, 0, 0 },
		{ "aispelltestcontinue",	'd', &ChatHandler::HandleAIAgentDebugContinue,					"",																														NULL, 0, 0, 0 },
		{ "aispelltestskip",		'd', &ChatHandler::HandleAIAgentDebugSkip,						"",																														NULL, 0, 0, 0 },
		{ "dumpcoords",				'd', &ChatHandler::HandleDebugDumpCoordsCommmand,				"",																														NULL, 0, 0, 0 },
		{ "sendpacket",				'd', &ChatHandler::HandleSendpacket,							"<opcode ID>, <data>",																									NULL, 0, 0, 0 },
		{ "sqlquery",				'd', &ChatHandler::HandleSQLQueryCommand,						"<sql query>",																											NULL, 0, 0, 0 },
		{ "rangecheck",				'd', &ChatHandler::HandleRangeCheckCommand,						"Checks the 'yard' range and internal range between the player and the target.",										NULL, 0, 0, 0 },
		{ "setallratings",			'd', &ChatHandler::HandleRatingsCommand,						"Sets rating values to incremental numbers based on their index.",														NULL, 0, 0, 0 },
		{ "sendmirrortimer",		'd', &ChatHandler::HandleMirrorTimerCommand,					"Sends a mirror Timer opcode to target syntax: <type>",																	NULL, 0, 0, 0 },
		{ NULL,						'0', NULL,														"",																														NULL, 0, 0, 0 }
	};
	dupe_command_table(debugCommandTable, _debugCommandTable);

	static ChatCommand DBItemCommandTable[] =
	{
		{ "create",							'z', &ChatHandler::HandleDBItemCreateCommand,							"Creates an item by ID in DB and Server", NULL, 0, 0, 0 },
		{ "setclass",						'z', &ChatHandler::HandleDBItemSetClassCommand,							"Sets an item's class by item id", NULL, 0, 0, 0 },
		{ "setsubclass",					'z', &ChatHandler::HandleDBItemSetSubClassCommand,						"Sets an item's subclass by item id", NULL, 0, 0, 0 },
		{ "setname",						'z', &ChatHandler::HandleDBItemSetNameCommand,							"", NULL, 0, 0, 0 },
		{ "setdisplayid",					'z', &ChatHandler::HandleDBItemSetDisplayIdCommand,						"", NULL, 0, 0, 0 },
		{ "setquality",						'z', &ChatHandler::HandleDBItemSetQualityCommand,						"", NULL, 0, 0, 0 },
		{ "setflags",						'z', &ChatHandler::HandleDBItemSetFlagsCommand,							"", NULL, 0, 0, 0 },
		{ "setbuyprice",					'z', &ChatHandler::HandleDBItemSetBuyPriceCommand,						"", NULL, 0, 0, 0 },
		{ "setsellprice",					'z', &ChatHandler::HandleDBItemSetSellPriceCommand,						"", NULL, 0, 0, 0 },
		{ "setinventorytype",				'z', &ChatHandler::HandleDBItemSetInventoryTypeCommand,					"", NULL, 0, 0, 0 },
		{ "setallowableclass",				'z', &ChatHandler::HandleDBItemSetAllowableClassCommand,				"", NULL, 0, 0, 0 },
		{ "setallowablerace",				'z', &ChatHandler::HandleDBItemSetAllowableRaceCommand,					"", NULL, 0, 0, 0 },
		{ "setitemlevel",					'z', &ChatHandler::HandleDBItemSetItemLevelCommand,						"", NULL, 0, 0, 0 },
		{ "setrequiredlevel",				'z', &ChatHandler::HandleDBItemSetRequiredLevelCommand,					"", NULL, 0, 0, 0 },
		{ "setrequiredskill",				'z', &ChatHandler::HandleDBItemSetRequiredSkillCommand,					"", NULL, 0, 0, 0 },
		{ "setrequiredskillrank",			'z', &ChatHandler::HandleDBItemSetRequiredSkillRankCommand,				"", NULL, 0, 0, 0 },
		{ "setrequiredspell",				'z', &ChatHandler::HandleDBItemSetRequiredSpellCommand,					"", NULL, 0, 0, 0 },
		{ "setrequiredrank1",				'z', &ChatHandler::HandleDBItemSetRequiredRank1Command,					"", NULL, 0, 0, 0 },
		{ "setrequiredrank2",				'z', &ChatHandler::HandleDBItemSetRequiredRank2Command,					"", NULL, 0, 0, 0 },
		{ "setrequiredfaction",				'z', &ChatHandler::HandleDBItemSetRequiredFactionCommand,				"", NULL, 0, 0, 0 },
		{ "setrequiredfactionstanding",		'z', &ChatHandler::HandleDBItemSetRequiredFactionStandingCommand,		"", NULL, 0, 0, 0 },
		{ "setunique",						'z', &ChatHandler::HandleDBItemSetUniqueCommand,						"", NULL, 0, 0, 0 },
		{ "setmaxcount",					'z', &ChatHandler::HandleDBItemSetMaxCountCommand,						"", NULL, 0, 0, 0 },
		{ "setcontainerslots",				'z', &ChatHandler::HandleDBItemSetContainerSlotsCommand,				"", NULL, 0, 0, 0 },
		{ "setstattype",					'z', &ChatHandler::HandleDBItemSetStatTypeCommand,						"", NULL, 0, 0, 0 },
		{ "setstatvalue",					'z', &ChatHandler::HandleDBItemSetStatValueCommand,						"", NULL, 0, 0, 0 },
		{ "setscalingstatid",				'z', &ChatHandler::HandleDBItemSetScalingStatIDCommand,					"", NULL, 0, 0, 0 },
		{ "setscalingstatflags",			'z', &ChatHandler::HandleDBItemSetScalingStatFlagsCommand,				"", NULL, 0, 0, 0 },
		{ "setdamagemin",					'z', &ChatHandler::HandleDBItemSetDamageMinCommand,						"", NULL, 0, 0, 0 },
		{ "setdamagemax",					'z', &ChatHandler::HandleDBItemSetDamageMaxCommand,						"", NULL, 0, 0, 0 },
		{ "setdamagetype",					'z', &ChatHandler::HandleDBItemSetDamageTypeCommand,					"", NULL, 0, 0, 0 },
		{ "setsetarmor",					'z', &ChatHandler::HandleDBItemSetArmorCommand,							"", NULL, 0, 0, 0 },
		{ "setsetresistance",				'z', &ChatHandler::HandleDBItemSetResistanceCommand,					"", NULL, 0, 0, 0 },
		{ "setsetdelay",					'z', &ChatHandler::HandleDBItemSetDelayCommand,							"", NULL, 0, 0, 0 },
		{ "setammotype",					'z', &ChatHandler::HandleDBItemSetAmmoTypeCommand,						"", NULL, 0, 0, 0 },
		{ "setrange",						'z', &ChatHandler::HandleDBItemSetRangeCommand,							"", NULL, 0, 0, 0 },
		{ "setspellinfo",					'z', &ChatHandler::HandleDBItemSetSpellInfoCommand,						"", NULL, 0, 0, 0 },
		{ "setbonding",						'z', &ChatHandler::HandleDBItemSetBondingCommand,						"", NULL, 0, 0, 0 },
		{ "setdescription",					'z', &ChatHandler::HandleDBItemSetDescriptionCommand,					"", NULL, 0, 0, 0 },
		{ "setpageid",						'z', &ChatHandler::HandleDBItemSetPageIDCommand,						"", NULL, 0, 0, 0 },
		{ "setpagelanguage",				'z', &ChatHandler::HandleDBItemSetPageLanguageCommand,					"", NULL, 0, 0, 0 },
		{ "setpagematerial",				'z', &ChatHandler::HandleDBItemSetPageMaterialCommand,					"", NULL, 0, 0, 0 },
		{ "setquestid",						'z', &ChatHandler::HandleDBItemSetQuestIDCommand,						"", NULL, 0, 0, 0 },
		{ "setlockid",						'z', &ChatHandler::HandleDBItemSetLockIDCommand,						"", NULL, 0, 0, 0 },
		{ "setsheathid",					'z', &ChatHandler::HandleDBItemSetSheathIDCommand,						"", NULL, 0, 0, 0 },
		{ "setrandompropertyid",			'z', &ChatHandler::HandleDBItemSetRandomPropertyIDCommand,				"", NULL, 0, 0, 0 },
		{ "setrandomsuffixid",				'z', &ChatHandler::HandleDBItemSetRandomSuffixIDCommand,				"", NULL, 0, 0, 0 },
		{ "setblock",						'z', &ChatHandler::HandleDBItemSetBlockCommand,							"", NULL, 0, 0, 0 },
		{ "setitemset",						'z', &ChatHandler::HandleDBItemSetItemSetCommand,						"", NULL, 0, 0, 0 },
		{ "setmaxdurability",				'z', &ChatHandler::HandleDBItemSetMaxDurabilityCommand,					"", NULL, 0, 0, 0 },
		{ "setzonenameid",					'z', &ChatHandler::HandleDBItemSetZoneNameIDCommand,					"", NULL, 0, 0, 0 },
		{ "setmapid",						'z', &ChatHandler::HandleDBItemSetMapIdCommand,							"", NULL, 0, 0, 0 },
		{ "setbagfamily",					'z', &ChatHandler::HandleDBItemSetBagFamilyCommand,						"", NULL, 0, 0, 0 },
		{ "settotemcategory",				'z', &ChatHandler::HandleDBItemSetTotemCategoryCommand,					"", NULL, 0, 0, 0 },
		{ "setsocketinfo",					'z', &ChatHandler::HandleDBItemSetSocketInfoCommand,					"", NULL, 0, 0, 0 },
		{ "setsocketbonus",					'z', &ChatHandler::HandleDBItemSetSocketBonusCommand,					"", NULL, 0, 0, 0 },
		{ "setgemproperties",				'z', &ChatHandler::HandleDBItemSetGemPropertiesCommand,					"", NULL, 0, 0, 0 },
		{ "setrequireddisenchantskill",		'z', &ChatHandler::HandleDBItemSetRequiredDisenchantSkillCommand,		"", NULL, 0, 0, 0 },
		{ "setlootgold",					'z', &ChatHandler::HandleDBItemSetLootGoldCommand,						"", NULL, 0, 0, 0 },
		{ "setgoldloot",					'z', &ChatHandler::HandleDBItemSetLootGoldCommand,						"", NULL, 0, 0, 0 },
		{ "setarmordamagemodifier",			'z', &ChatHandler::HandleDBItemSetArmorDamageModifierCommand,			"", NULL, 0, 0, 0 },
		{ NULL,								'0', NULL,																"", NULL, 0, 0, 0 }
	};
	dupe_command_table(DBItemCommandTable, _DBItemCommandTable);

	static ChatCommand DatabaseCommandTable[] =
	{
		{ "item",	'z', NULL, "", DBItemCommandTable,		0, 0, 0 },
		{ NULL,		'0', NULL, "", NULL,					0, 0, 0 }
	};
	dupe_command_table(DatabaseCommandTable, _DatabaseCommandTable);

	static ChatCommand waypointCommandTable[] =
	{
		{ "add",			'w', &ChatHandler::HandleWPAddCommand,					"Add wp at current pos",								NULL, 0, 0, 0 },
		{ "show",			'w', &ChatHandler::HandleWPShowCommand,					"Show wp's for creature",								NULL, 0, 0, 0 },
		{ "hide",			'w', &ChatHandler::HandleWPHideCommand,					"Hide wp's for creature",								NULL, 0, 0, 0 },
		{ "delete",			'w', &ChatHandler::HandleWPDeleteCommand,				"Delete selected wp",									NULL, 0, 0, 0 },
		{ "movehere",		'w', &ChatHandler::HandleWPMoveHereCommand,				"Move to this wp",										NULL, 0, 0, 0 },
		{ "flags",			'w', &ChatHandler::HandleWPFlagsCommand,				"Wp flags",												NULL, 0, 0, 0 },
		{ "waittime",		'w', &ChatHandler::HandleWPWaitCommand,					"Wait time at this wp",									NULL, 0, 0, 0 },
		{ "standstate",		'w', &ChatHandler::HandleWPStandStateCommand,			"StandState at this wp <direction><standstate0-8)",		NULL, 0, 0, 0 },
		{ "SpellToCast",	'w', &ChatHandler::HandleWPSpellToCastCommand,			"Spell to cast at this wp <direction><spell_id>",		NULL, 0, 0, 0 },
		{ "emote",			'w', &ChatHandler::HandleWPEmoteCommand,				"Emote at this wp",										NULL, 0, 0, 0 },
		{ "skin",			'w', &ChatHandler::HandleWPSkinCommand,					"Skin at this wp",										NULL, 0, 0, 0 },
		{ "change",			'w', &ChatHandler::HandleWPChangeNoCommand,				"Change at this wp",									NULL, 0, 0, 0 },
		{ "info",			'w', &ChatHandler::HandleWPInfoCommand,					"Show info for wp",										NULL, 0, 0, 0 },
		{ "movetype",		'w', &ChatHandler::HandleWPMoveTypeCommand,				"Movement type at wp",									NULL, 0, 0, 0 },
		{ "generate",		'w', &ChatHandler::HandleGenerateWaypoints,				"Randomly generate wps",								NULL, 0, 0, 0 },
		{ "save",			'w', &ChatHandler::HandleSaveWaypoints,					"Save all waypoints",									NULL, 0, 0, 0 },
		{ "deleteall",		'w', &ChatHandler::HandleDeleteWaypoints,				"Delete all waypoints",									NULL, 0, 0, 0 },
		{ "addfly",			'w', &ChatHandler::HandleWaypointAddFlyCommand,			"Adds a flying waypoint",								NULL, 0, 0, 0 },
		{ "gettext",		'w', &ChatHandler::HandleWaypointGettextCommand,		"Shows saytext for current waypoint.",					NULL, 0, 0, 0 },
		{ "backwardtext",	'w', &ChatHandler::HandleWaypointBackwardTextCommand,	"Adds backward text to current waypoint <SayText>",		NULL, 0, 0, 0 },
		{ "forwardtext",	'w', &ChatHandler::HandleWaypointForwardTextCommand,	"Adds forward text to current waypoint <SayText>",		NULL, 0, 0, 0 },
		{ NULL,				'0', NULL,												"",														NULL, 0, 0, 0 }
	};
	dupe_command_table(waypointCommandTable, _waypointCommandTable);

	static ChatCommand GMTicketCommandTable[] =
	{
#ifdef GM_TICKET_MY_MASTER_COMPATIBLE
		{ "get",				'c', &ChatHandler::HandleGMTicketListCommand,						"Gets GM Ticket list.",												NULL, 0, 0, 0 },
		{ "getId",				'c', &ChatHandler::HandleGMTicketGetByIdCommand,					"Gets GM Ticket by player name.",									NULL, 0, 0, 0 },
		{ "delId",				'c', &ChatHandler::HandleGMTicketRemoveByIdCommand,					"Deletes GM Ticket by player name.",								NULL, 0, 0, 0 },
#else
		{ "list",				'c', &ChatHandler::HandleGMTicketListCommand,						"Lists all active GM Tickets.",										NULL, 0, 0, 0 },
		{ "get",				'c', &ChatHandler::HandleGMTicketGetByIdCommand,					"Gets GM Ticket with ID x.",										NULL, 0, 0, 0 },
		{ "remove",				'c', &ChatHandler::HandleGMTicketRemoveByIdCommand,					"Removes GM Ticket with ID x.",										NULL, 0, 0, 0 },
		{ "deletepermanent",	'z', &ChatHandler::HandleGMTicketDeletePermanentCommand,			"Deletes GM Ticket with ID x permanently.",							NULL, 0, 0, 0 },
		{ "assign",				'c', &ChatHandler::HandleGMTicketAssignToCommand,					"Assigns GM Ticket with id x to GM y (if empty to your self).",		NULL, 0, 0, 0 },
		{ "release",			'c', &ChatHandler::HandleGMTicketReleaseCommand,					"Releases assigned GM Ticket with ID x.",							NULL, 0, 0, 0 },
		{ "comment",			'c', &ChatHandler::HandleGMTicketCommentCommand,					"Sets comment x to GM Ticket with ID y.",							NULL, 0, 0, 0 },
#endif
		{ "toggle",				'z', &ChatHandler::HandleGMTicketToggleTicketSystemStatusCommand,	"Toggles the ticket system status.",								NULL, 0, 0, 0 },
		{ NULL,					'0', NULL,															"",																	NULL, 0, 0, 0 }
	};
	dupe_command_table(GMTicketCommandTable, _GMTicketCommandTable);

	static ChatCommand GuildCommandTable[] =
	{
		{ "create",				'm', &ChatHandler::CreateGuildCommand,				"Creates a guild.",											NULL, 0, 0, 0 },
		{ "rename",				'm', &ChatHandler::HandleRenameGuildCommand,		"Renames a guild Syntax:<new name>.",						NULL, 0, 0, 0 },
		{ "members",			'm', &ChatHandler::HandleGuildMembersCommand,		"Lists guildmembers and their ranks.",						NULL, 0, 0, 0 },
		{ "removeplayer",		'm', &ChatHandler::HandleGuildRemovePlayerCommand,	"Removes the target from its guild.",						NULL, 0, 0, 0 },
		{ "disband",			'm', &ChatHandler::HandleGuildDisbandCommand,		"Disbands the guild of your target.",						NULL, 0, 0, 0 },
		{ "setleader",			'm', &ChatHandler::HandleGuildSetLeaderCommand,		"Sets the guild leader of the target's guild to <player>",	NULL, 0, 0, 0 },
		{ NULL,					'0', NULL,											"",															NULL, 0, 0, 0 }
	};
	dupe_command_table(GuildCommandTable, _GuildCommandTable);

	static ChatCommand TitleCommandTable[] =
	{
		{ "add",		'm', &ChatHandler::HandleAddTitleCommand,			"<TitleNumber> - Adds known title to the selected player",			NULL, 0, 0, 0 },
		{ "remove",		'm', &ChatHandler::HandleRemoveTitleCommand,		"<TitleNumber> - Removes known title from the selected player",		NULL, 0, 0, 0 },
		{ "known",		'm', &ChatHandler::HandleGetKnownTitlesCommand,		"Shows all titles known by the player",								NULL, 0, 0, 0 },
		{ "setchosen",	'm', &ChatHandler::HandleSetChosenTitleCommand,		"<TitleNumber> - Sets chosen title for the selected player",		NULL, 0, 0, 0 },
		{ NULL,			'0', NULL,											"",																	NULL, 0, 0, 0 }
	};
	dupe_command_table(TitleCommandTable, _TitleCommandTable);

	static ChatCommand GameObjectCommandTable[] =
	{
		{ "select",			'o', &ChatHandler::HandleGOSelect,			"Selects the nearest GameObject to you",		NULL, 0,					0, 0 },
		{ "delete",			'o', &ChatHandler::HandleGODelete,			"Deletes selected GameObject",					NULL, 0,					0, 0 },
		{ "spawn",			'o', &ChatHandler::HandleGOSpawn,			"Spawns a GameObject by ID",					NULL, 0,					0, 0 },
		{ "info",			'o', &ChatHandler::HandleGOInfo,			"Gives you informations about selected GO",		NULL, 0,					0, 0 },
		{ "activate",		'o', &ChatHandler::HandleGOActivate,		"Activates/Opens the selected GO.",				NULL, 0,					0, 0 },
		{ "enable",			'o', &ChatHandler::HandleGOEnable,			"Enables the selected GO for use.",				NULL, 0,					0, 0 },
		{ "scale",			'o', &ChatHandler::HandleGOScale,			"Sets scale of selected GO",					NULL, 0,					0, 0 },
		{ "animprogress",	'o', &ChatHandler::HandleGOAnimProgress,	"Sets anim progress",							NULL, 0,					0, 0 },
		{ "move",			'g', &ChatHandler::HandleGOMove,			"Moves gameobject to player xyz",				NULL, 0,					0, 0 },
		{ "rotate",			'g', &ChatHandler::HandleGORotate,			"Rotates gameobject x degrees",					NULL, 0,					0, 0 },
		{ "damage",			'g', &ChatHandler::HandleDebugGoDamage,		"Damages the gameobject for <args>",			NULL, 0,					0, 0 },
		{ "rebuild",		'g', &ChatHandler::HandleDebugGoRepair,		"Resets the gameobject health and state",		NULL, 0,					0, 0 },
		{ "f_flags",		'g', NULL,									"Flags",										NULL, GAMEOBJECT_FLAGS,		0, 3 },
		{ "f_dynflags",		'g', NULL,									"Dynflags",										NULL, GAMEOBJECT_DYNAMIC,	0, 3 },
		{ NULL,				'0', NULL,									"",												NULL, 0,					0, 0 }
	};
	dupe_command_table(GameObjectCommandTable, _GameObjectCommandTable);

	static ChatCommand BattlegroundCommandTable[] =
	{
		{ "setbgscore",		'e', &ChatHandler::HandleSetBGScoreCommand,					"<Teamid> <Score> - Sets battleground score. 2 Arguments. ",		NULL, 0, 0, 0 },
		{ "startbg",		'e', &ChatHandler::HandleStartBGCommand,					"Starts current battleground match.",								NULL, 0, 0, 0 },
		{ "pausebg",		'e', &ChatHandler::HandlePauseBGCommand,					"Pauses current battleground match.",								NULL, 0, 0, 0 },
		{ "bginfo",			'e', &ChatHandler::HandleBGInfoCommnad,						"Displays information about current battleground.",					NULL, 0, 0, 0 },
		{ "setworldstate",	'e', &ChatHandler::HandleSetWorldStateCommand,				"<var> <val> - Var can be in hex. WS Value.",						NULL, 0, 0, 0 },
		{ "playsound",		'e', &ChatHandler::HandlePlaySoundCommand,					"<val>. Val can be in hex.",										NULL, 0, 0, 0 },
		{ "setbfstatus",	'e', &ChatHandler::HandleSetBattlefieldStatusCommand,		".setbfstatus - NYI.",												NULL, 0, 0, 0 },
		{ "leave",			'e', &ChatHandler::HandleBattlegroundExitCommand,			"Leaves the current battleground.",									NULL, 0, 0, 0 },
		{ "forcestart",		'e', &ChatHandler::HandleBattlegroundForcestartCommand,		"Forcestart current battlegrounds.",								NULL, 0, 0, 0 },
		{ NULL,				'0', NULL,													"",																	NULL, 0, 0, 0 }
	};
	dupe_command_table(BattlegroundCommandTable, _BattlegroundCommandTable);

	static ChatCommand NPCCommandTable[] =
	{
		{ "vendorsetadd",		'z', &ChatHandler::HandleItemSetCommand,			"Adds item set to vendor",																														NULL, 0, 0, 0 },
		{ "vendorsetremove",	'z', &ChatHandler::HandleItemSetRemoveCommand,		"Removes item set from vendor",																													NULL, 0, 0, 0 },
		{ "vendoradditem",		'z', &ChatHandler::HandleItemCommand,				"Adds to vendor",																																NULL, 0, 0, 0 },
		{ "vendorremoveitem",	'z', &ChatHandler::HandleItemRemoveCommand,			"Removes from vendor.",																															NULL, 0, 0, 0 },
		{ "traineraddlearn",	'z', &ChatHandler::HandleTrainerAddLearnSpell,		"Adds a spell that is learned to the player. req:<spellid> option:<spellcost><reqspell><reqskill><reqskillval><reqlvl><deletespell><isprof>",	NULL, 0, 0, 0 },
		{ "traineraddcast",		'z', &ChatHandler::HandleTrainerAddCastSpell,		"Adds a spell that is cast on the player. req:<spellid> option:<spellcost><reqspell><reqskill><reqskillval><reqlvl><deletespell><isprof>",		NULL, 0, 0, 0 },
		{ "flags",				'n', &ChatHandler::HandleNPCFlagCommand,			"Changes NPC flags",																															NULL, 0, 0, 0 },
		{ "emote",				'n', &ChatHandler::HandleEmoteCommand,				".emote - Sets emote state",																													NULL, 0, 0, 0 },
		{ "setstandstate",		'n', &ChatHandler::HandleStandStateCommand,			".setstandstate - Sets stand state",																											NULL, 0, 0, 0 },
		{ "delete",				'n', &ChatHandler::HandleDeleteCommand,				"Deletes mobs from db and world.",																												NULL, 0, 0, 0 },
		{ "info",				'n', &ChatHandler::HandleNpcInfoCommand,			"Displays NPC information",																														NULL, 0, 0, 0 },
		{ "addAgent",			'n', &ChatHandler::HandleAddAIAgentCommand,			".npc addAgent <agent> <procEvent> <procChance> <procCount> <spellId> <spellType> <spelltargetType> <spellCooldown> <floatMisc1> <Misc2>",		NULL, 0, 0, 0 },
		{ "listAgent",			'n', &ChatHandler::HandleListAIAgentCommand,		".npc listAgent",																																NULL, 0, 0, 0 },
		{ "say",				'n', &ChatHandler::HandleMonsterSayCommand,			".npc say <text> - Makes selected mob say text <text>.",																						NULL, 0, 0, 0 },
		{ "yell",				'n', &ChatHandler::HandleMonsterYellCommand,		".npc yell <Text> - Makes selected mob yell text <text>.",																						NULL, 0, 0, 0 },
		{ "come",				'n', &ChatHandler::HandleNpcComeCommand,			".npc come - Makes npc move to your position",																									NULL, 0, 0, 0 },
		{ "jumpto",				'n', &ChatHandler::HandleNpcJumpCommand,			".npc come - Makes npc move to your position",																									NULL, 0, 0, 0 },
		{ "return",				'n', &ChatHandler::HandleNpcReturnCommand,			".npc return - Returns ncp to spawnpoint.",																										NULL, 0, 0, 0 },
		{ "respawn",			'n', &ChatHandler::HandleCreatureRespawnCommand,	".respawn - Respawns a dead npc from its corpse.",																								NULL, 0, 0, 0 },
		{ "spawn",				'n', &ChatHandler::HandleCreatureSpawnCommand,		".npc spawn <id> <save> - Spawns npc of entry <id>",																							NULL, 0, 0, 0 },
		{ "spawnlink",			'n', &ChatHandler::HandleNpcSpawnLinkCommand,		".spawnlink sqlentry",																															NULL, 0, 0, 0 },
		{ "possess",			'n', &ChatHandler::HandleNpcPossessCommand,			".npc possess - Possess an npc (mind control)",																									NULL, 0, 0, 0 },
		{ "unpossess",			'n', &ChatHandler::HandleNpcUnPossessCommand,		".npc unpossess - Unposses any currently possessed npc.",																						NULL, 0, 0, 0 },
		{ "select",				'n', &ChatHandler::HandleNpcSelectCommand,			".npc select - selects npc closest",																											NULL, 0, 0, 0 },
		{ "cast",				'd', &ChatHandler::HandleMonsterCastCommand,		".npc cast <spellId> - Makes selected mob cast the specified spell on you.",																	NULL, 0, 0, 0 },
		{ "equip",				'a', &ChatHandler::HandleNPCEquipCommand,			"Use: .npc equip <slot> <itemid> - use .npc equip <slot> 0 to remove the item",																	NULL, 0, 0, 0 },
		{ "setongameobject",	'a', &ChatHandler::HandleNPCSetOnObjectCommand,		"Updates spawn information so that the creature does not fall through objects when spawning into world.",										NULL, 0, 0, 0 },
		{ "save",				'z', NULL,											"",																																				NULL, 0, 0, 0 },
		{ NULL,					'0', NULL,											"",																																				NULL, 0, 0, 0 }
	};
	dupe_command_table(NPCCommandTable, _NPCCommandTable);

	static ChatCommand GMCommandTable[] =
	{
		{ "list",			'1', &ChatHandler::HandleGMListCommand,			"Shows active GM's",									NULL,					0, 0, 0 },
		{ "off",			't', &ChatHandler::HandleGMOffCommand,			"Sets GM tag off",										NULL,					0, 0, 0 },
		{ "on",				't', &ChatHandler::HandleGMOnCommand,			"Sets GM tag on",										NULL,					0, 0, 0 },
		{ "disabledev",		'z', &ChatHandler::HandleToggleDevCommand,		"Toggles <Dev> Tag",									NULL,					0, 0, 0 },
		{ "announce",		'u', &ChatHandler::HandleGMAnnounceCommand,		"Announce to GM's and Admin's",							NULL,					0, 0, 0 },
		{ "allowwhispers",	'c', &ChatHandler::HandleAllowWhispersCommand,	"Allows whispers from player <s> while in gmon mode.",	NULL,					0, 0, 0 },
		{ "blockwhispers",	'c', &ChatHandler::HandleBlockWhispersCommand,	"Blocks whispers from player <s> while in gmon mode.",	NULL,					0, 0, 0 },
		{ "ticket",			'c', NULL,										"",														GMTicketCommandTable,	0, 0, 0 },
		{ NULL,				'0', NULL,										"",														NULL,					0, 0, 0 }
	};
	dupe_command_table(GMCommandTable, _gamemasterCommandTable);

	static ChatCommand trackerCommandTable[] =
	{
		{ "add",				'z', &ChatHandler::HandleAddTrackerCommand,			"Attaches a tracker to a GM's IP allowing them to be followed across accounts.",	NULL, 0, 0, 0 },
		{ "del",				'z', &ChatHandler::HandleDelTrackerCommand,			"Removes a tracker from a GM's IP, syntax: .tracker del <trackerId>",				NULL, 0, 0, 0 },
		{ "list",				'z', &ChatHandler::HandleTrackerListCommand,		"Checks if there are any accounts connected from the tracked GMs' IP.",				NULL, 0, 0, 0 },
		{ NULL,					'0', NULL,											"",																					NULL, 0, 0, 0 }
	};
	dupe_command_table(trackerCommandTable, _trackerCommandTable);

	static ChatCommand AdminCommandTable[] =
	{
		{ "announce",				'z', &ChatHandler::HandleAdminAnnounceCommand,		"Command to Announce to Admin's",				NULL, 0, 0, 0 },
		{ "castall",				'z', &ChatHandler::HandleCastAllCommand,			"Makes all players online cast spell <x>.",		NULL, 0, 0, 0 },
		{ "playall",				'z', &ChatHandler::HandleGlobalPlaySoundCommand,	"Plays a sound to the entire server.",			NULL, 0, 0, 0 },
		{ "saveall",				'z', &ChatHandler::HandleSaveAllCommand,			"Save's all playing characters",				NULL, 0, 0, 0 },
		{ "rehash",					'z', &ChatHandler::HandleRehashCommand,				"Reloads config file.",							NULL, 0, 0, 0 },
		{ "enableauctionhouse",		'z', &ChatHandler::HandleEnableAH,					"Enables Auctionhouse",							NULL, 0, 0, 0 },
		{ "disableauctionhouse",	'z', &ChatHandler::HandleDisableAH,					"Disables Auctionhouse",						NULL, 0, 0, 0 },
		{ "masssummon",				'z', &ChatHandler::HandleMassSummonCommand,			"Summons all players.",							NULL, 0, 0, 0 },
		{ "restart",				'z', &ChatHandler::HandleRestartCommand,			"Initiates server restart in <x> seconds.",		NULL, 0, 0, 0 },
		{ "shutdown",				'z', &ChatHandler::HandleShutdownCommand,			"Initiates server shutdown in <x> seconds.",	NULL, 0, 0, 0 },
		{ NULL,						'0', NULL,											"",												NULL, 0, 0, 0 }
	};
	dupe_command_table(AdminCommandTable, _administratorCommandTable);

	static ChatCommand CheatCommandTable[] =
	{
		{ "status",			'm', &ChatHandler::HandleShowCheatsCommand,				"Shows active cheats.",								NULL, 0, 0, 0 },
		{ "taxi",			'm', &ChatHandler::HandleTaxiCheatCommand,				"Enables all taxi nodes.",							NULL, 0, 0, 0 },
		{ "cooldown",		'm', &ChatHandler::HandleCooldownCheatCommand,			"Enables no cooldown cheat.",						NULL, 0, 0, 0 },
		{ "casttime",		'm', &ChatHandler::HandleCastTimeCheatCommand,			"Enables no cast time cheat.",						NULL, 0, 0, 0 },
		{ "power",			'm', &ChatHandler::HandlePowerCheatCommand,				"Disables mana consumption etc.",					NULL, 0, 0, 0 },
		{ "god",			'm', &ChatHandler::HandleGodModeCommand,				"Sets god mode, prevents you from taking damage.",	NULL, 0, 0, 0 },
		{ "fly",			'm', &ChatHandler::HandleFlyCommand,					"Sets fly mode",									NULL, 0, 0, 0 },
		{ "explore",		'm', &ChatHandler::HandleExploreCheatCommand,			"Reveals the unexplored parts of the map.",			NULL, 0, 0, 0 },
		{ "flyspeed",		'm', &ChatHandler::HandleFlySpeedCheatCommand,			"Modifies fly speed.",								NULL, 0, 0, 0 },
		{ "stack",			'm', &ChatHandler::HandleStackCheatCommand,				"Enables aura stacking cheat.",						NULL, 0, 0, 0 },
		{ "triggerpass",	'm', &ChatHandler::HandleTriggerpassCheatCommand,		"Ignores area trigger prerequisites.",				NULL, 0, 0, 0 },
		{ "vendorpass",		'm', &ChatHandler::HandleVendorPassCheatCommand,		"Ignores npc vendor prerequisites.",				NULL, 0, 0, 0 },
		{ "itempass",		'm', &ChatHandler::HandleItemReqCheatCommand,			"Ignores item requirements(incomplete).",			NULL, 0, 0, 0 },
		{ NULL,				'0', NULL,												"",													NULL, 0, 0, 0 },
	};
	dupe_command_table(CheatCommandTable, _CheatCommandTable);

	static ChatCommand accountCommandTable[] =
	{
		{ "ban",		'a', &ChatHandler::HandleAccountBannedCommand,		"Ban account. .account ban name timeperiod reason",		NULL, 0, 0, 0 },
		{ "unban",		'z', &ChatHandler::HandleAccountUnbanCommand,		"Unbans account x.",									NULL, 0, 0, 0 },
		{ "level",		'z', &ChatHandler::HandleAccountLevelCommand,		"Sets gm level on account. <username><gm_lvl>.",		NULL, 0, 0, 0 },
		{ "mute",		'a', &ChatHandler::HandleAccountMuteCommand,		"Mutes account for <timeperiod>.",						NULL, 0, 0, 0 },
		{ "unmute",		'a', &ChatHandler::HandleAccountUnmuteCommand,		"Unmutes account <x>",									NULL, 0, 0, 0 },
		{ NULL,			'0', NULL,											"",														NULL, 0, 0, 0 },
	};
	dupe_command_table(accountCommandTable, _accountCommandTable);

	static ChatCommand honorCommandTable[] =
	{
		{ "addpoints",			'm', &ChatHandler::HandleAddHonorCommand,						"Adds x amount of honor points/currency",					NULL, 0, 0, 0 },
		{ "addkills",			'm', &ChatHandler::HandleAddKillCommand,						"Adds x amount of honor kills",								NULL, 0, 0, 0 },
		{ "globaldailyupdate",	'm', &ChatHandler::HandleGlobalHonorDailyMaintenanceCommand,	"Daily honor field moves",									NULL, 0, 0, 0 },
		{ "singledailyupdate",	'm', &ChatHandler::HandleNextDayCommand,						"Daily honor field moves for selected player only",			NULL, 0, 0, 0 },
		{ "pvpcredit",			'm', &ChatHandler::HandlePVPCreditCommand,						"Sends PVP credit packet, with specified rank and points",	NULL, 0, 0, 0 },
		{ NULL,					'0', NULL,														"",															NULL, 0, 0, 0 },
	};
	dupe_command_table(honorCommandTable, _honorCommandTable);

	static ChatCommand petCommandTable[] =
	{
		{ "createpet",			'm', &ChatHandler::HandleCreatePetCommand,		"Creates a pet with <entry>.",	NULL, 0, 0, 0 },
		{ "renamepet",			'm', &ChatHandler::HandleRenamePetCommand,		"Renames a pet to <name>.",		NULL, 0, 0, 0 },
		{ "addspell",			'm', &ChatHandler::HandleAddPetSpellCommand,	"Teaches pet <spell>.",			NULL, 0, 0, 0 },
		{ "removespell",		'm', &ChatHandler::HandleRemovePetSpellCommand,	"Removes pet spell <spell>.",	NULL, 0, 0, 0 },
		{ "addtalentpoints",	'm', &ChatHandler::HandleAddPetTalentPoints,	"Adds <x> talent points to pet",NULL, 0, 0, 0 },
		{ "resettalents",		'm', &ChatHandler::HandleResetPetTalents,		"Resets the pets talents",		NULL, 0, 0, 0 },
		{ NULL,					'0', NULL,										"",								NULL, 0, 0, 0 },
	};
	dupe_command_table(petCommandTable, _petCommandTable);

	static ChatCommand recallCommandTable[] =
	{
		{ "list",			'q', &ChatHandler::HandleRecallListCommand,			"List recall locations [filter].",	NULL, 0, 0, 0 },
		{ "port",			'q', &ChatHandler::HandleRecallGoCommand,			"Port to recalled location",		NULL, 0, 0, 0 },
		{ "add",			'q', &ChatHandler::HandleRecallAddCommand,			"Add recall location",				NULL, 0, 0, 0 },
		{ "del",			'q', &ChatHandler::HandleRecallDelCommand,			"Remove a recall location",			NULL, 0, 0, 0 },
		{ "portplayer",		'm', &ChatHandler::HandleRecallPortPlayerCommand,	"recall ports player",				NULL, 0, 0, 0 },
		{ NULL,				'0', NULL,											"",									NULL, 0, 0, 0 },
	};
	dupe_command_table(recallCommandTable, _recallCommandTable);

	static ChatCommand lookupCommandTable[] =
	{
		{ "item",		'l', &ChatHandler::HandleLookupItemCommand,			"Looks up item string x.",					NULL, 0, 0, 0 },
		{ "quest",		'l', &ChatHandler::HandleQuestLookupCommand,		"Looks up quest string x.",					NULL, 0, 0, 0 },
		{ "creature",	'l', &ChatHandler::HandleLookupCreatureCommand,		"Looks up item string x.",					NULL, 0, 0, 0 },
		{ "object",		'l', &ChatHandler::HandleLookupObjectCommand,		"Looks up object string x.",				NULL, 0, 0, 0 },
		{ "spell",		'l', &ChatHandler::HandleLookupSpellCommand,		"Looks up spell string x.",					NULL, 0, 0, 0 },
		{ "title",		'l', &ChatHandler::HandleLookupTitleCommand,		"Looks up title based on inputted name.",	NULL, 0, 0, 0 },
		{ NULL,			'0', NULL,											"",											NULL, 0, 0, 0 },
	};
	dupe_command_table(lookupCommandTable, _lookupCommandTable);

	static ChatCommand questCommandTable[] =
	{
		{ "addboth",	'2', &ChatHandler::HandleQuestAddBothCommand,		"Add quest <id> to the targeted NPC as start & finish",			NULL, 0, 0, 0 },
		{ "addfinish",	'2', &ChatHandler::HandleQuestAddFinishCommand,		"Add quest <id> to the targeted NPC as finisher",				NULL, 0, 0, 0 },
		{ "addstart",	'2', &ChatHandler::HandleQuestAddStartCommand,		"Add quest <id> to the targeted NPC as starter",				NULL, 0, 0, 0 },
		{ "delboth",	'2', &ChatHandler::HandleQuestDelBothCommand,		"Delete quest <id> from the targeted NPC as start & finish",	NULL, 0, 0, 0 },
		{ "delfinish",	'2', &ChatHandler::HandleQuestDelFinishCommand,		"Delete quest <id> from the targeted NPC as finisher",			NULL, 0, 0, 0 },
		{ "delstart",	'2', &ChatHandler::HandleQuestDelStartCommand,		"Delete quest <id> from the targeted NPC as starter",			NULL, 0, 0, 0 },
		{ "complete",	'2', &ChatHandler::HandleQuestFinishCommand,		"Complete/Finish quest <id>",									NULL, 0, 0, 0 },
		{ "finisher",	'2', &ChatHandler::HandleQuestFinisherCommand,		"Lookup quest finisher for quest <id>",							NULL, 0, 0, 0 },
		{ "item",		'2', &ChatHandler::HandleQuestItemCommand,			"Lookup itemid necessary for quest <id>",						NULL, 0, 0, 0 },
		{ "list",		'2', &ChatHandler::HandleQuestListCommand,			"Lists the quests for the npc <id>",							NULL, 0, 0, 0 },
		{ "load",		'2', &ChatHandler::HandleQuestLoadCommand,			"Loads quests from database",									NULL, 0, 0, 0 },
		{ "lookup",		'2', &ChatHandler::HandleQuestLookupCommand,		"Looks up quest string x",										NULL, 0, 0, 0 },
		{ "giver",		'2', &ChatHandler::HandleQuestGiverCommand,			"Lookup quest giver for quest <id>",							NULL, 0, 0, 0 },
		{ "remove",		'2', &ChatHandler::HandleQuestRemoveCommand,		"Removes the quest <id> from the targeted player",				NULL, 0, 0, 0 },
		{ "reward",		'2', &ChatHandler::HandleQuestRewardCommand,		"Shows reward for quest <id>",									NULL, 0, 0, 0 },
		{ "status",		'2', &ChatHandler::HandleQuestStatusCommand,		"Lists the status of quest <id>",								NULL, 0, 0, 0 },
		{ "spawn",		'2', &ChatHandler::HandleQuestSpawnCommand,			"Port to spawn location for quest <id>",						NULL, 0, 0, 0 },
		{ "start",		'2', &ChatHandler::HandleQuestStartCommand,			"Starts quest <id>",											NULL, 0, 0, 0 },
		{ NULL,			'0', NULL,											"",																NULL, 0, 0, 0 },
	};
	dupe_command_table(questCommandTable, _questCommandTable);

	static ChatCommand wintergraspCommandTable[] =
	{
		{ "start",	'z', &ChatHandler::HandleForceStartWintergrasp,		"Force starts wintergrasp.",	NULL, 0, 0, 0 },
		{ "end",	'z', &ChatHandler::HandleForceEndWintergrasp,		"Ends current wintergrasp.",	NULL, 0, 0, 0 },
		{ NULL,		'0', NULL,											"",								NULL, 0, 0, 0 },
	};
	dupe_command_table(wintergraspCommandTable, _wintergraspCommandTable);

	static ChatCommand FactionCommandTable[] =
	{
		{ "modstanding",	'z', &ChatHandler::HandleFactionModStanding,	"Mods a player's faction's standing based on a value",	NULL, 0, 0, 0 },
		{ "setstanding",	'z', &ChatHandler::HandleFactionSetStanding,	"Changes a player's faction's standing to a value",		NULL, 0, 0, 0 },
		{ NULL,				'0', NULL,										"",														NULL, 0, 0, 0 },
	};
	dupe_command_table(FactionCommandTable, _FactionCommandTable);

	static ChatCommand commandTable[] = {
		//{ "renameguild",			'a', &ChatHandler::HandleRenameGuildCommand,						"Renames a guild.",				NULL, 0, 0, 0 },
		{ "commands",				'0', &ChatHandler::HandleCommandsCommand,							"Shows Commands",				NULL, 0, 0, 0 },
		{ "help",					'0', &ChatHandler::HandleHelpCommand,								"Shows help for command",		NULL, 0, 0, 0 },
		{ "announce",				'u', &ChatHandler::HandleAnnounceCommand,							"Sends Msg To All",				NULL, 0, 0, 0 },
		{ "wannounce",				'u', &ChatHandler::HandleWAnnounceCommand,							"Sends Widescreen Msg To All",	NULL, 0, 0, 0 },
		{ "appear",					'v', &ChatHandler::HandleAppearCommand,								"Teleports to x's position.",	NULL, 0, 0, 0 },
		{ "summon",					'v', &ChatHandler::HandleSummonCommand,								"Summons x to your position",	NULL, 0, 0, 0 },
		{ "banchar",				'b', &ChatHandler::HandleBanCharacterCommand,						"Bans character x with or without reason",				NULL, 0, 0, 0 },
		{ "unbanchar",				'b', &ChatHandler::HandleUnBanCharacterCommand,						"Unbans character x",			NULL, 0, 0, 0 },
		{ "kick",					'b', &ChatHandler::HandleKickCommand,								"Kicks player from server",		NULL, 0, 0, 0 },
		{ "kill",					'r', &ChatHandler::HandleKillCommand,								".kill - Kills selected unit.",	NULL, 0, 0, 0 },
		{ "killplr" ,				'r', &ChatHandler::HandleKillByPlrCommand,							".killplr <name> - Kills specified player" , NULL , 0 , 0 , 0 },
		{ "revive",					'r', &ChatHandler::HandleReviveCommand,								"Revives you.",					NULL, 0, 0, 0 },
		{ "reviveplr",				'r', &ChatHandler::HandleReviveStringcommand,						"Revives player specified.",		NULL, 0, 0, 0 },
		{ "demorph",				'm', &ChatHandler::HandleDeMorphCommand,							"Demorphs from morphed model.",	NULL, 0, 0, 0 },
		{ "mount",					'm', &ChatHandler::HandleMountCommand,								"Mounts into modelid x.",		NULL, 0, 0, 0 },
		{ "dismount",				'm', &ChatHandler::HandleDismountCommand,							"Dismounts.",					NULL, 0, 0, 0 },
		{ "fulldismount",			'm', &ChatHandler::HandleFullDismountCommand,						"Force a player to full dismount (taxi)",	NULL, 0, 0, 0 },
		{ "gps",					'0', &ChatHandler::HandleGPSCommand,								"Shows Position",				NULL, 0, 0, 0 },
		{ "info",					'0', &ChatHandler::HandleInfoCommand,								"Server info",					NULL, 0, 0, 0 },
		{ "worldport",				'v', &ChatHandler::HandleWorldPortCommand,							"",								NULL, 0, 0, 0 },
		{ "save",					's', &ChatHandler::HandleSaveCommand,								"Save's your character",			NULL, 0, 0, 0 },
		{ "start",					'm', &ChatHandler::HandleStartCommand,								"Teleport's you to a starting location",								NULL, 0, 0, 0 },
		{ "additem",				'm', &ChatHandler::HandleAddInvItemCommand,							"",								NULL, 0, 0, 0 },
		{ "removeitem",				'm', &ChatHandler::HandleRemoveItemCommand,							"Removes item %u count %u.", NULL, 0, 0, 0 },
		{ "invincible",				'j', &ChatHandler::HandleInvincibleCommand,							".invincible - Toggles INVINCIBILITY (mobs won't attack you)", NULL, 0, 0, 0 },
		{ "invisible",				'i', &ChatHandler::HandleInvisibleCommand,							".invisible - Toggles INVINCIBILITY and INVISIBILITY (mobs won't attack you and nobody can see you, but they can see your chat messages)", NULL, 0, 0, 0 },
		{ "resetreputation",		'n', &ChatHandler::HandleResetReputationCommand,					".resetreputation - Resets reputation to start levels. (use on characters that were made before reputation fixes.)", NULL, 0, 0, 0 },
		{ "resetspells",			'n', &ChatHandler::HandleResetSpellsCommand,						".resetspells - Resets all spells to starting spells of targeted player. DANGEROUS.", NULL, 0, 0, 0 },
		{ "resettalents",			'n', &ChatHandler::HandleResetTalentsCommand,						".resettalents - Resets all talents of targeted player to that of their current level. DANGEROUS.", NULL, 0, 0, 0 },
		{ "resetskills",			'n', &ChatHandler::HandleResetSkillsCommand ,						".resetskills - Resets all skills.", NULL, 0, 0, 0 },
		{ "learn",					'm', &ChatHandler::HandleLearnCommand,								"Learns spell",					NULL, 0, 0, 0 },
		{ "unlearn",				'm', &ChatHandler::HandleUnlearnCommand,							"Unlearns spell",				NULL, 0, 0, 0 },
		{ "getskilllevel",			'm', &ChatHandler::HandleGetSkillLevelCommand,						"Gets the current level of a skill",NULL,0,0,0 }, //DGM (maybe add to playerinfo?)
		{ "getskillinfo",			'm', &ChatHandler::HandleGetSkillsInfoCommand,						"Gets all the skills from a player",NULL,0,0,0 },
		{ "learnskill",				'm', &ChatHandler::HandleLearnSkillCommand,							".learnskill <skillid> (optional) <value> <maxvalue> - Learns skill id skillid.", NULL, 0, 0, 0 },
		{ "advanceskill",			'm', &ChatHandler::HandleModifySkillCommand,						"advanceskill <skillid> <amount, optional, default = 1> - Advances skill line x times..", NULL, 0, 0, 0 },
		{ "removeskill",			'm', &ChatHandler::HandleRemoveSkillCommand,						".removeskill <skillid> - Removes skill",		NULL, 0, 0, 0 },
		{ "increaseweaponskill",	'm', &ChatHandler::HandleIncreaseWeaponSkill,						".increaseweaponskill <count> - Increase eqipped weapon skill x times (defaults to 1).", NULL, 0, 0, 0 },
		{ "playerinfo",				'm', &ChatHandler::HandlePlayerInfo,								".playerinfo - Displays informations about the selected character (account...)", NULL, 0, 0, 0 },

		{ "modify",					'm', NULL,															"",					modifyCommandTable, 0, 0, 0 },
		{ "waypoint",				'w', NULL,															"",					waypointCommandTable, 0, 0, 0 },
		{ "debug",					'd', NULL,															"",					debugCommandTable, 0, 0, 0 },
		{ "database",				'z', NULL,															"",					DatabaseCommandTable, 0, 0, 0 },
		{ "gobject",				'o', NULL,															"",					GameObjectCommandTable, 0, 0, 0 },
		{ "battleground",			'e', NULL,															"",					BattlegroundCommandTable, 0, 0, 0 },
		{ "npc",					'n', NULL,															"",					NPCCommandTable, 0, 0, 0 },
		{ "gm",						'n', NULL,															"",					GMCommandTable, 0, 0, 0 },
		{ "tracker",				'n', NULL,															"",					trackerCommandTable, 0, 0, 0 },
		{ "warn",					'0', NULL,															"",					warnCommandTable, 0, 0, 0 },
		{ "admin",					'z', NULL,															"",					AdminCommandTable, 0, 0, 0 },
		{ "cheat",					'm', NULL,															"",					CheatCommandTable, 0, 0, 0 },
		{ "account",				'a', NULL,															"",					accountCommandTable, 0, 0, 0 },
		{ "honor",					'm', NULL,															"",					honorCommandTable, 0, 0, 0 },
		{ "quest",					'q', NULL,															"",					questCommandTable, 0, 0, 0 },
		{ "pet",					'm', NULL,															"",					petCommandTable, 0, 0, 0 },
		{ "recall",					'q', NULL,															"",					recallCommandTable, 0, 0, 0 },
		{ "guild",					'm', NULL,															"",					GuildCommandTable, 0, 0, 0 },
		{ "title",					'm', NULL,															"",					TitleCommandTable, 0, 0, 0 },
		{ "lookup",					'0', NULL,															"",					lookupCommandTable, 0, 0, 0 },
		{ "wintergrasp",			'0', NULL,															"",					wintergraspCommandTable, 0, 0, 0 },
		{ "faction",				'0', NULL,															"",					FactionCommandTable, 0, 0, 0 },
		{ "getpos",					'd', &ChatHandler::HandleGetPosCommand,								"",					NULL, 0, 0, 0 },
		{ "clearcooldowns",			'm', &ChatHandler::HandleClearCooldownsCommand,						"Clears all cooldowns for your class.", NULL, 0, 0, 0 },
		{ "removeauras",			'm', &ChatHandler::HandleRemoveAurasCommand,						"Removes all auras from target",	NULL, 0, 0, 0 },
		{ "paralyze",				'b', &ChatHandler::HandleParalyzeCommand,							"Roots/Paralyzes the target.",	NULL, 0, 0, 0 },
		{ "unparalyze",				'b', &ChatHandler::HandleUnParalyzeCommand,							"Unroots/Unparalyzes the target.",NULL, 0, 0, 0 },
		{ "setmotd",				'm', &ChatHandler::HandleSetMotdCommand,							"Sets MOTD",		NULL, 0, 0, 0 },
		{ "additemset",				'm', &ChatHandler::HandleAddItemSetCommand,							"Adds item set to inv.",			NULL, 0, 0, 0 },
		{ "gotrig",					'v', &ChatHandler::HandleTriggerCommand,							"Warps to areatrigger <id>",		NULL, 0, 0, 0 },
		{ "exitinstance",			'm', &ChatHandler::HandleExitInstanceCommand,						"Exits current instance, return to entry point.", NULL, 0, 0, 0 },
		{ "reloadtable",			'm', &ChatHandler::HandleDBReloadCommand,							"Reloads some of the database tables", NULL, 0, 0, 0 },
		{ "advanceallskills",		'm', &ChatHandler::HandleAdvanceAllSkillsCommand,					"Advances all skills <x> points.", NULL, 0, 0, 0 },
		{ "killbyplayer",			'f', &ChatHandler::HandleKillByPlayerCommand,						"Disconnects the player with name <s>.", NULL, 0, 0, 0 },
		{ "killbyaccount",			'f', &ChatHandler::HandleKillBySessionCommand,						"Disconnects the session with account name <s>.", NULL, 0, 0, 0 },
		{ "castspell",				'd', &ChatHandler::HandleCastSpellCommand,							".castspell <spellid> - Casts spell on target.", NULL, 0, 0, 0 },
		{ "modperiod",				'm', &ChatHandler::HandleModPeriodCommand,							"Changes period of current transporter.", NULL, 0, 0, 0 },
		{ "npcfollow",				'm', &ChatHandler::HandleNpcFollowCommand,							"Sets npc to follow you", NULL, 0, 0, 0 },
		{ "nullfollow",				'm', &ChatHandler::HandleNullFollowCommand,							"Sets npc to not follow anything", NULL, 0, 0, 0 },
		{ "formationlink1",			'm', &ChatHandler::HandleFormationLink1Command,						"Sets formation master.", NULL, 0, 0, 0 },
		{ "formationlink2",			'm', &ChatHandler::HandleFormationLink2Command,						"Sets formation slave with distance and angle", NULL, 0, 0, 0 },
		{ "formationclear",			'm', &ChatHandler::HandleFormationClearCommand,						"Removes formation from creature", NULL, 0, 0, 0 },
		{ "addipban",				'm', &ChatHandler::HandleIPBanCommand,								"Adds an address to the IP ban table: <address>/<mask> <duration> <reason>\n Mask represents a subnet mask, use /32 to ban a single ip.\nDuration should be a number followed by a character representing the calendar subdivision to use (h>hours, d>days, w>weeks, m>months, y>years, default minutes).", NULL, 0, 0, 0 },
		{ "delipban",				'm', &ChatHandler::HandleIPUnBanCommand,							"Deletes an address from the IP ban table: <address>", NULL, 0, 0, 0 },
		{ "renamechar",				'm', &ChatHandler::HandleRenameCommand,								"Renames character x to y.", NULL, 0, 0, 0 },
		{ "forcerenamechar",		'm', &ChatHandler::HandleForceRenameCommand,						"Forces character x to rename his char next login", NULL, 0, 0, 0 },
		{ "recustomizechar",		'm', &ChatHandler::HandleRecustomizeCharCommand,					"Flags character x for character recustomization", NULL, 0, 0, 0 },
		{ "getstanding",			'm', &ChatHandler::HandleGetStandingCommand,						"Gets standing of faction %u.", NULL, 0, 0, 0 },
		{ "setstanding",			'm', &ChatHandler::HandleSetStandingCommand,						"Sets stanging of faction %u.", NULL, 0, 0, 0 },
		{ "createarenateam",		'g', &ChatHandler::HandleCreateArenaTeamCommands,					"Creates arena team", NULL, 0, 0, 0 },
		{ "whisperblock",			'g', &ChatHandler::HandleWhisperBlockCommand,						"Blocks like .gmon except without the <GM> tag", NULL, 0, 0, 0 },
		{ "logcomment",				'1', &ChatHandler::HandleGmLogCommentCommand,						"Adds a comment to the GM log for the admins to read." , NULL , 0 , 0 , 0 },
		{ "showitems",				'm', &ChatHandler::HandleShowItems,									"test for ItemIterator", NULL, 0, 0, 0 },
		{ "testlos",				'm', &ChatHandler::HandleCollisionTestLOS,							"tests los", NULL, 0, 0, 0 },
		{ "testindoor",				'm', &ChatHandler::HandleCollisionTestIndoor,						"tests indoor", NULL, 0, 0, 0 },
		{ "debugvmapareainfo",		'm', &ChatHandler::HandleDebugVmapAreaInfo,							"Generates information of Vmap area.", NULL, 0, 0, 0 },
		{ "getheight",				'm', &ChatHandler::HandleCollisionGetHeight,						"Gets height", NULL, 0, 0, 0 },
		{ "renameallinvalidchars",	'z', &ChatHandler::HandleRenameAllCharacter,						"Renames all invalid character names", NULL, 0,0, 0 },
		{ "removesickness",			'm', &ChatHandler::HandleRemoveRessurectionSickessAuraCommand,		"Removes ressurrection sickness from the target", NULL, 0, 0, 0 },
		{ "fixscale",				'm', &ChatHandler::HandleFixScaleCommand,							"",					NULL, 0, 0, 0 },
		{ "clearcorpses",			'm', &ChatHandler::HandleClearCorpsesCommand,						"",					NULL, 0, 0, 0 },
		{ "clearbones",				'm', &ChatHandler::HandleClearBonesCommand,							"",					NULL, 0, 0, 0 },

		{ "multimute",				'b', &ChatHandler::HandleMultiMuteCommand,							"mutes multiple , .multimute <reason> <player1> <player2> ...",			NULL, 0, 0, 0 },
		{ "multiban",				'b', &ChatHandler::HandleMultiBanCommand,							"bans multiple , .multimute <reason> <player1> <player2> ...",			NULL, 0, 0, 0 },
		{ "multiaccountban",		'b', &ChatHandler::HandleMultiAccountBanCommand,					"account bans multiple , .multimute <reason> <player1> <player2> ...",	NULL, 0, 0, 0 },
		{ "multikick",				'b', &ChatHandler::HandleMultiKickCommand,							"kicks multiple , .multimute <reason> <player1> <player2> ...",			NULL, 0, 0, 0 },
		{ NULL,						'0', NULL,															"",					NULL, 0, 0, 0 }
	};
	dupe_command_table(commandTable, _commandTable);

	/* set the correct pointers */
	ChatCommand * p = &_commandTable[0];
	while(p->Name != 0)
	{
		if(p->ChildCommands != 0)
		{
			// Set the correct pointer.
			ChatCommand * np = GetSubCommandTable(p->Name);
			ASSERT(np);
			p->ChildCommands = np;
		}
		++p;
	}
}

/*struct SpecStruct
{
	std::map<uint32, uint8> talents;	// map of <talentId, talentRank>
	uint16 glyphs[GLYPHS_COUNT];
};

bool ChatHandler::xxx(const char* args, WorldSession *m_session)
{
	char * end;
	char * start;
	QueryResult* result = CharacterDatabase.Query("SELECT * FROM characters_extra");

	do
	{
		std::set<uint32> m_spells;
		SpecStruct m_spec[2];

		Field *fields = result->Fetch();
		uint32 i = 1;

		// Load Spells from CSV data.
		start = (char*)fields[i++].GetString();//buff;
		SpellEntry * spProto;
		while(true)
		{
			end = strchr(start,',');
			if(!end)break;
			*end = 0;
			//mSpells.insert(atol(start));
			spProto = dbcSpell.LookupEntryForced(atol(start));

			if(spProto)
				m_spells.insert(spProto->Id);

			start = end +1;
		}

		for( uint8 s = 0; s < MAX_SPEC_COUNT; ++s )
		{
			start = (char*)fields[i++].GetString();
			uint8 glyphid = 0;
			while(glyphid < GLYPHS_COUNT)
			{
				end = strchr(start,',');
				if(!end)break;
				*end= 0;
				m_spec[s].glyphs[glyphid] = (uint16)atol(start);
				++glyphid;
				start = end + 1;
			}

			//Load talents for spec
			start = (char*)fields[i++].GetString();
			while(end != NULL)
			{
				end = strchr(start,',');
				if(!end)
					break;
				*end= 0;
				uint32 talentid = atol(start);
				start = end + 1;

				end = strchr(start,',');
				if(!end)
					break;
				*end = 0;
				uint8 rank = (uint8)atol(start);
				start = end + 1;

				m_spec[s].talents.insert(make_pair<uint32, uint8>(talentid, rank));
			}
		}

		std::stringstream ss;
		ss << "INSERT INTO playerspells (guid, spellid) VALUES ";
		SpellSet::iterator spellItr = m_spells.begin();
		bool first = true;
		for(; spellItr != m_spells.end(); ++spellItr)
		{
			SpellEntry * sp = dbcSpell.LookupEntry( *spellItr );
			if( !sp)
				continue;

			if(!first)
				ss << ",";
			else
				first = false;

			ss << "("<< fields[0].GetUInt32() << "," << uint32(*spellItr) << ")";
		}
		CharacterDatabase.Execute(ss.str().c_str());

		for(uint8 s = 0; s < 2; s++)
		{
			std::map<uint32, uint8> *talents = &m_spec[s].talents;
			std::map<uint32, uint8>::iterator itr;
			for(itr = talents->begin(); itr != talents->end(); itr++)
			{
				std::stringstream ss;
				ss << "INSERT INTO playertalents (guid, spec, tid, rank) VALUES "
					<< "(" << fields[0].GetUInt32() << ","
					<< uint32(s) << ","
					<< itr->first << ","
					<< uint32(itr->second) << ")";

				CharacterDatabase.Execute(ss.str().c_str());
			}
		}

	}while(result->NextRow());

	RedSystemMessage(m_session, "No values specified.");
	return true;
}*/

ChatHandler::ChatHandler()
{
	new CommandTableStorage;
	sComTableStore.Init();
	SkillNameManager = new SkillNameMgr;
}

ChatHandler::~ChatHandler()
{
	sComTableStore.Dealloc();
	delete CommandTableStorage::getSingletonPtr();
	delete SkillNameManager;
}

bool ChatHandler::hasStringAbbr(const char* s1, const char* s2)
{
	for(;;)
	{
		if( !*s2 )
			return true;
		else if( !*s1 )
			return false;
		else if( tolower( *s1 ) != tolower( *s2 ) )
			return false;
		s1++; s2++;
	}
}

void ChatHandler::SendMultilineMessage(WorldSession *m_session, const char *str)
{
	char * start = (char*)str, *end;
	for(;;)
	{
		end = strchr(start, '\n');
		if(!end)
			break;

		*end = '\0';
		SystemMessage(m_session, start);
		start = end + 1;
	}
	if(*start != '\0')
		SystemMessage(m_session, start);
}

bool ChatHandler::ExecuteCommandInTable(ChatCommand *table, const char* text, WorldSession *m_session)
{
	std::string cmd = "";

	// get command
	while (*text != ' ' && *text != '\0')
	{
		cmd += *text;
		text++;
	}

	while (*text == ' ') text++; // skip whitespace

	if(!cmd.length())
		return false;

	for(uint32 i = 0; table[i].Name != NULL; i++)
	{
		if(!hasStringAbbr(table[i].Name, cmd.c_str()))
			continue;

		if(table[i].CommandGroup != '0' && !m_session->CanUseCommand(table[i].CommandGroup))
			continue;

		if(table[i].ChildCommands != NULL)
		{
			if(!ExecuteCommandInTable(table[i].ChildCommands, text, m_session))
			{
				if(table[i].Help != "")
					SendMultilineMessage(m_session, table[i].Help.c_str());
				else
				{
					GreenSystemMessage(m_session, "Available Subcommands:");
					for(uint32 k=0; table[i].ChildCommands[k].Name;k++)
					{
						if(table[i].ChildCommands[k].CommandGroup != '0' && m_session->CanUseCommand(table[i].ChildCommands[k].CommandGroup))
							BlueSystemMessage(m_session, " %s - %s", table[i].ChildCommands[k].Name, table[i].ChildCommands[k].Help.size() ? table[i].ChildCommands[k].Help.c_str() : "No Help Available");
					}
				}
			}

			return true;
		}

		// Check for field-based commands
		if(table[i].Handler == NULL && (table[i].MaxValueField || table[i].NormalValueField))
		{
			bool result = false;
			if(strlen(text) == 0)
			{
				RedSystemMessage(m_session, "No values specified.");
			}
			if(table[i].ValueType == 2)
				result = CmdSetFloatField(m_session, table[i].NormalValueField, table[i].MaxValueField, table[i].Name, text);
			else if(table[i].ValueType == 3)
			{
				result = true;
				GameObject* go = NULL;
				if((go = m_session->GetPlayer()->m_GM_SelectedGO) != NULL)
				{
					go->SetUInt32Value(table[i].NormalValueField, atoi(text));
					go->SaveToDB();
				}
				else
					result = false;
			}
			else
				result = CmdSetValueField(m_session, table[i].NormalValueField, table[i].MaxValueField, table[i].Name, text);

			if(!result)
				RedSystemMessage(m_session, "Must be in the form of (command) <value>, or, (command) <value> <maxvalue>");
		}
		else
		{
			if(!(this->*(table[i].Handler))(text, m_session))
			{
				if(table[i].Help != "")
					SendMultilineMessage(m_session, table[i].Help.c_str());
				else
				{
					RedSystemMessage(m_session, "Incorrect syntax specified. Try .help %s for the correct syntax.", table[i].Name);
				}
			}
		}

		return true;
	}
	return false;
}

int ChatHandler::ParseCommands(const char* text, WorldSession *m_session)
{
	if (!m_session)
		return 0;

	if(!*text)
		return 0;

	if(m_session->GetPermissionCount() == 0 && sWorld.m_reqGmForCommands)
		return 0;

	if(text[0] != '.') // let's not confuse users
		return 0;

	//skip '..' :P that pisses me off
	if(text[1] == '.')
		return 0;

	text++;
	if(!ExecuteCommandInTable(CommandTableStorage::getSingleton().Get(), text, m_session))
	{
		SystemMessage(m_session, "There is no such command, or you do not have access to it.");
	}

	return 1;
}

WorldPacket * ChatHandler::FillMessageData( uint32 type, int32 language, const char *message,uint64 guid , uint8 flag) const
{
	ASSERT(type != CHAT_MSG_CHANNEL);
	//channels are handled in channel handler and so on
	uint32 messageLength = (uint32)strlen((char*)message) + 1;

	WorldPacket *data = new WorldPacket(SMSG_MESSAGECHAT, messageLength + 30);

	*data << (uint8)type;
	*data << language;

	*data << guid;
	*data << uint32(0);

	*data << guid;

	*data << messageLength;
	*data << message;

	*data << uint8(flag);
	return data;
}

WorldPacket* ChatHandler::FillSystemMessageData(const char *message) const
{
	uint32 messageLength = (uint32)strlen((char*)message) + 1;

	WorldPacket * data = new WorldPacket(SMSG_MESSAGECHAT, 30 + messageLength);
	*data << (uint8)CHAT_MSG_SYSTEM;
	*data << (uint32)LANG_UNIVERSAL;

	*data << (uint64)0; // Who cares about guid when there's no nickname displayed heh ?
	*data << (uint32)0;
	*data << (uint64)0;

	*data << messageLength;
	*data << message;

	*data << uint8(0);

	return data;
}

Player* ChatHandler::getSelectedChar(WorldSession *m_session, bool showerror)
{
	uint64 guid;
	Player* chr;

	guid = m_session->GetPlayer()->GetSelection();

	if (guid == 0)
	{
		if(showerror)
			GreenSystemMessage(m_session, "Auto-targeting self.");

		chr = m_session->GetPlayer(); // autoselect
	}
	else
		chr = m_session->GetPlayer()->GetMapMgr()->GetPlayer(GET_LOWGUID_PART(guid));

	if(chr == NULL)
	{
		if(showerror)
			RedSystemMessage(m_session, "This command requires that you select a player.");
		return NULLPLR;
	}

	return chr;
}

Creature* ChatHandler::getSelectedCreature(WorldSession *m_session, bool showerror)
{
	uint64 guid;
	Creature* creature = NULLCREATURE;

	guid = m_session->GetPlayer()->GetSelection();
	if(GET_TYPE_FROM_GUID(guid) == HIGHGUID_TYPE_PET)
		creature = m_session->GetPlayer()->GetMapMgr()->GetPet( GET_LOWGUID_PART(guid) );
	else if(GET_TYPE_FROM_GUID(guid) == HIGHGUID_TYPE_CREATURE)
		creature = m_session->GetPlayer()->GetMapMgr()->GetCreature( GET_LOWGUID_PART(guid) );
	else if(GET_TYPE_FROM_GUID(guid) == HIGHGUID_TYPE_VEHICLE)
		creature = m_session->GetPlayer()->GetMapMgr()->GetVehicle( GET_LOWGUID_PART(guid) );

	if(creature != NULL)
		return creature;
	else
	{
		if(showerror)
			RedSystemMessage(m_session, "This command requires that you select a creature.");
		return NULLCREATURE;
	}
}

Unit* ChatHandler::getSelectedUnit(WorldSession *m_session, bool showerror)
{
	uint64 guid;
	guid = m_session->GetPlayer()->GetSelection();
	Unit * unit = m_session->GetPlayer()->GetMapMgr()->GetUnit(guid);
	if(unit != NULL)
		return unit;
	else
	{
		if(showerror)
			RedSystemMessage(m_session, "This command requires that you select a unit.");
		return NULLUNIT;
	}
}

void ChatHandler::SystemMessage(WorldSession *m_session, const char* message, ...)
{
	if( !message )
		return;

	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024, message,ap);
	WorldPacket * data = FillSystemMessageData(msg1);
	if(m_session != NULL)
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::ColorSystemMessage(WorldSession *m_session, const char* colorcode, const char *message, ...)
{
	if( !message ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024, message,ap);
	char msg[1024];
	snprintf(msg, 1024, "%s%s|r", colorcode, msg1);
	WorldPacket * data = FillSystemMessageData(msg);
	if(m_session != NULL)
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::RedSystemMessage(WorldSession *m_session, const char *message, ...)
{
	if( !message ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024,message,ap);
	char msg[1024];
	snprintf(msg, 1024,"%s%s|r", MSG_COLOR_LIGHTRED/*MSG_COLOR_RED*/, msg1);
	WorldPacket * data = FillSystemMessageData(msg);
	if(m_session != NULL)
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::GreenSystemMessage(WorldSession *m_session, const char *message, ...)
{
	if( !message ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024, message,ap);
	char msg[1024];
	snprintf(msg, 1024, "%s%s|r", MSG_COLOR_GREEN, msg1);
	WorldPacket * data = FillSystemMessageData(msg);
	if(m_session != NULL)
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::WhiteSystemMessage(WorldSession *m_session, const char *message, ...)
{
	if( !message ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024, message,ap);
	char msg[1024];
	snprintf(msg, 1024, "%s%s|r", MSG_COLOR_WHITE, msg1);
	WorldPacket * data = FillSystemMessageData(msg);
	if(m_session != NULL)
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::BlueSystemMessage(WorldSession *m_session, const char *message, ...)
{
	if( !message ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024, message,ap);
	char msg[1024];
	snprintf(msg, 1024,"%s%s|r", MSG_COLOR_LIGHTBLUE, msg1);
	WorldPacket * data = FillSystemMessageData(msg);
	if(m_session != NULL)
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::RedSystemMessageToPlr(Player* plr, const char *message, ...)
{
	if( !message || !plr->GetSession() ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024,message,ap);
	RedSystemMessage(plr->GetSession(), (const char*)msg1);
}

void ChatHandler::GreenSystemMessageToPlr(Player* plr, const char *message, ...)
{
	if( !message || !plr->GetSession() ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024,message,ap);
	GreenSystemMessage(plr->GetSession(), (const char*)msg1);
}

void ChatHandler::BlueSystemMessageToPlr(Player* plr, const char *message, ...)
{
	if( !message || !plr->GetSession() ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024,message,ap);
	BlueSystemMessage(plr->GetSession(), (const char*)msg1);
}

void ChatHandler::SystemMessageToPlr(Player* plr, const char* message, ...)
{
	if( !message || !plr->GetSession() ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024,message,ap);
	SystemMessage(plr->GetSession(), msg1);
}

bool ChatHandler::CmdSetValueField(WorldSession *m_session, uint32 field, uint32 fieldmax, const char *fieldname, const char *args)
{
	if(!args)
		return false;

	char* pvalue = strtok((char*)args, " ");
	uint32 mv, av;

	if (pvalue)
		av = atol(pvalue);
	else
		return false;

	if(fieldmax)
	{
		char* pvaluemax = strtok(NULL, " ");
		if (!pvaluemax)
			return false;
		else
			mv = atol(pvaluemax);
	}
	else
		mv = 0;

	//valid UNIT_FIELD?
	if(field <= OBJECT_END || field > UNIT_END )
	{
		RedSystemMessage(m_session, "Specified field is not valid.");
		return true;
	}
	if (av <= 0)
	{
		RedSystemMessage(m_session, "Values are invalid. Value must be > 0.");
		return true;
	}
	if(fieldmax && (mv < av || mv <= 0))
	{
		RedSystemMessage(m_session, "Values are invalid. Max value must be >= new value.");
		return true;
	}

	Player* plr = getSelectedChar(m_session, false);
	if(plr!=NULL)
	{
		sWorld.LogGM(m_session, "used modify field value: %s, %u on %s", fieldname, av, plr->GetName());
		if(fieldmax)
		{
			BlueSystemMessage(m_session, "You set the %s of %s to %d/%d.", fieldname, plr->GetName(), av, mv);
			GreenSystemMessageToPlr(plr, "%s set your %s to %d/%d.", m_session->GetPlayer()->GetName(), fieldname, av, mv);
		}
		else
		{
			BlueSystemMessage(m_session, "You set the %s of %s to %d.", fieldname, plr->GetName(), av);
			GreenSystemMessageToPlr(plr, "%s set your %s to %d.", m_session->GetPlayer()->GetName(), fieldname, av);
		}

		if(field == UNIT_FIELD_STAT1) av /= 2;
		if(field == UNIT_FIELD_BASE_HEALTH)
			plr->SetUInt32Value(UNIT_FIELD_HEALTH, av);

		plr->SetUInt32Value(field, av);

		if(fieldmax)
			plr->SetUInt32Value(fieldmax, mv);
	}
	else
	{
		Creature* cr = getSelectedCreature(m_session, false);
		if(cr)
		{
			std::string creaturename = cr->GetCreatureInfo() ? cr->GetCreatureInfo()->Name : "Unknown Being";

			sWorld.LogGM(m_session, "used modify field value: [creature]%s, %u on %s", fieldname, av, creaturename.c_str());

			switch(field)
			{
			case UNIT_FIELD_STAT1:
				{
					av /= 2;
				}break;
			case UNIT_FIELD_BASE_HEALTH:
				{
					cr->SetUInt32Value(UNIT_FIELD_HEALTH, av);
				}break;
			}

			cr->SetUInt32Value(field, av);

			if(fieldmax)
			{
				cr->SetUInt32Value(fieldmax, mv);
				BlueSystemMessage(m_session, "Setting %s of %s to %u/%u.", fieldname, creaturename.c_str(), av, mv);
			}
			else
				BlueSystemMessage(m_session, "Setting %s of %s to %u.", fieldname, creaturename.c_str(), av);

			if(m_session->CanUseCommand('z'))
				cr->SaveToDB();
		}
		else
		{
			RedSystemMessage(m_session, "Invalid Selection.");
		}
	}
	return true;
}

bool ChatHandler::CmdSetFloatField(WorldSession *m_session, uint32 field, uint32 fieldmax, const char *fieldname, const char *args)
{
	char* pvalue = strtok((char*)args, " ");
	float mv, av;

	if (!pvalue)
		return false;
	else
		av = (float)atof(pvalue);

	if(fieldmax)
	{
		char* pvaluemax = strtok(NULL, " ");
		if (!pvaluemax)
			return false;
		else
			mv = (float)atof(pvaluemax);
	}
	else
	{
		mv = 0;
	}

	if (av <= 0)
	{
		RedSystemMessage(m_session, "Values are invalid. Value must be < max (if max exists), and both must be > 0.");
		return true;
	}
	if(fieldmax)
	{
		if(mv < av || mv <= 0)
		{
			RedSystemMessage(m_session, "Values are invalid. Value must be < max (if max exists), and both must be > 0.");
			return true;
		}
	}

	Player* plr = getSelectedChar(m_session, false);
	if(plr != NULL)
	{
		sWorld.LogGM(m_session, "used modify field value: %s, %f on %s", fieldname, av, plr->GetName());

		if(fieldmax)
		{
			BlueSystemMessage(m_session, "You set the %s of %s to %.1f/%.1f.", fieldname, plr->GetName(), av, mv);
			GreenSystemMessageToPlr(plr, "%s set your %s to %.1f/%.1f.", m_session->GetPlayer()->GetName(), fieldname, av, mv);
		}
		else
		{
			BlueSystemMessage(m_session, "You set the %s of %s to %.1f.", fieldname, plr->GetName(), av);
			GreenSystemMessageToPlr(plr, "%s set your %s to %.1f.", m_session->GetPlayer()->GetName(), fieldname, av);
		}
		plr->SetFloatValue(field, av);
		if(fieldmax) plr->SetFloatValue(fieldmax, mv);
	}
	else
	{
		Creature* cr = getSelectedCreature(m_session, false);
		if(cr)
		{
			if(!(field < UNIT_END && fieldmax < UNIT_END)) return false;
			std::string creaturename = "Unknown Being";
			if(cr->GetCreatureInfo())
				creaturename = cr->GetCreatureInfo()->Name;
			if(fieldmax)
				BlueSystemMessage(m_session, "Setting %s of %s to %.1f/%.1f.", fieldname, creaturename.c_str(), av, mv);
			else
				BlueSystemMessage(m_session, "Setting %s of %s to %.1f.", fieldname, creaturename.c_str(), av);
			cr->SetFloatValue(field, av);
			sWorld.LogGM(m_session, "used modify field value: [creature]%s, %f on %s", fieldname, av, creaturename.c_str());
			if(fieldmax)
				cr->SetFloatValue(fieldmax, mv);
		}
		else
		{
			RedSystemMessage(m_session, "Invalid Selection.");
		}
	}
	return true;
}

bool ChatHandler::HandleGetPosCommand(const char* args, WorldSession *m_session)
{
	Creature* creature = getSelectedCreature(m_session);
	if(!creature) 
		return false;
	BlueSystemMessage(m_session, "Creature Position: \nX: %f\nY: %f\nZ: %f\n", creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ());
	return true;
}


bool ChatHandler::HandleDebugRetroactiveQuestAchievements(const char *args, WorldSession *m_session)
{
	Player* pTarget = getSelectedChar(m_session, true );
	if(!pTarget) return true;

	pTarget->RetroactiveCompleteQuests();
	m_session->GetPlayer()->BroadcastMessage("Done.");
	return true;
}

bool ChatHandler::HandleModifyFactionCommand(const char *args, WorldSession *m_session)
{
	Player* player = m_session->GetPlayer();
	if(player == NULL)
		return true;

	Unit* unit = getSelectedUnit(m_session, false);

	if(unit == NULL)
		unit = player;

	uint32 faction = atol(args);
	if(!faction && unit->IsCreature())
		faction = TO_CREATURE(unit)->GetProto()->Faction;

	BlueSystemMessage(m_session, "Set target's faction to %u", faction);

	unit->SetFaction(faction, m_session->CanUseCommand('z'));
	return true;
}

bool ChatHandler::HandleModifyScaleCommand(const char *args, WorldSession *m_session)
{
	Player* player = m_session->GetPlayer();
	if(player == NULL)
		return true;

	Unit* unit = getSelectedUnit(m_session, false);
	if(unit == NULL)
		unit = player;

	float scale = atof(args);
	int save = m_session->CanUseCommand('z') ? 1 : 0;
	if(sscanf(args, "%f %i", &scale, &save) < 1)
		return false;

	if(scale > 255 || scale < 0)
		return false;

	if(!scale && unit->IsCreature())
		scale = TO_CREATURE(unit)->GetProto() ? TO_CREATURE(unit)->GetProto()->Scale : 1.0f;

	BlueSystemMessage(m_session, "Set target's scale to %f", scale);
	unit->SetFloatValue(OBJECT_FIELD_SCALE_X, scale);
	if(unit->IsCreature() && (save > 0))
		TO_CREATURE(unit)->SaveToDB();

	return true;
}

uint16 ChatHandler::GetItemIDFromLink(const char* link, uint32* itemid)
{
	if(link== NULL)
	{
		*itemid = 0;
		return 0;
	}
	uint16 slen = (uint16)strlen(link);

	const char* ptr = strstr(link, "|Hitem:");
	if(ptr == NULL)
	{
		*itemid = 0;
		return slen;
	}

	ptr += 7;
	*itemid = atoi(ptr);
	
	ptr = strstr(link, "|r");
	if(ptr == NULL)
	{
		*itemid = 0;
		return slen;
	}

	ptr += 2;
	return (ptr-link) & 0x0000ffff;
}

int32 ChatHandler::GetSpellIDFromLink(const char* link)
{
	if(link== NULL)
		return 0;

	const char* ptr = strstr(link, "|Hspell:");
	if(ptr == NULL)
		return 0;

	return atol(ptr+8);
}