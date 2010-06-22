/*
 * Sun++ Scripts for Sandshroud MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2007-2008 Moon++ Team <http://www.moonplusplus.info/>
 * Copyright (C) 2008-2009 Sun++ Team <http://www.sunscripting.com/>
 * Copyright (C) 2009-2011 Sandshroud <http://www.sandshroud.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INSTANCE_SCRIPTS_SETUP_H
#define INSTANCE_SCRIPTS_SETUP_H

//By this you can put off scripts.
#define Classic
#define BurningCrusade
#define LichKing


#ifdef Classic
void SetupBlackfathomDeeps(ScriptMgr * mgr);
void SetupBlackrockDepths(ScriptMgr * mgr);
void SetupBlackrockSpire(ScriptMgr * mgr);
void SetupDireMaul(ScriptMgr * mgr);
//void SetupGnomeregan(ScriptMgr * mgr);
void SetupMaraudon(ScriptMgr * mgr);
void SetupRagefireChasm(ScriptMgr * mgr);
void SetupRazorfenDowns(ScriptMgr * mgr);
//void SetupRazorfenKraul(ScriptMgr * mgr);
void SetupScarletMonastery(ScriptMgr * mgr);
void SetupScholomance(ScriptMgr * mgr);
void SetupShadowfangKeep(ScriptMgr * mgr);
//void SetupStratholme(ScriptMgr * mgr);
//void SetupSunkenTemple(ScriptMgr * mgr);
void SetupDeadmines(ScriptMgr * mgr);
void SetupTheStockade(ScriptMgr * mgr);
void SetupUldaman(ScriptMgr * mgr);
void SetupWailingCaverns(ScriptMgr * mgr);
void SetupZulFarrak(ScriptMgr * mgr);
void SetupBlackwingLair(ScriptMgr * mgr);
void SetupMoltenCore(ScriptMgr * mgr);
//void SetupRuinsOfAhnQiraj(ScriptMgr * mgr);
//void SetupTempleOfAhnQiraj(ScriptMgr * mgr);
void SetupZulGurub(ScriptMgr * mgr);
#endif


#ifdef BurningCrusade
void SetupAuchenaiCrypts(ScriptMgr * mgr);				//Auchindoun
void SetupManaTombs(ScriptMgr * mgr);					//Auchindoun
void SetupShadowLabyrinth(ScriptMgr * mgr);				//Auchindoun
void SetupSethekkHalls(ScriptMgr * mgr);				//Auchindoun
void SetupBlackTemple(ScriptMgr * mgr);					//Black Temple
void SetupHyjalSummit(ScriptMgr * mgr);					//Caverns of Time
void SetupTheBlackMorass(ScriptMgr * mgr);				//Caverns of Time
void SetupOldHillsbradFoothills(ScriptMgr * mgr);		//Caverns of Time
void SetupSerpentshrineCavern(ScriptMgr * mgr);			//Coilfang Reservoir
void SetupTheUnderbog(ScriptMgr * mgr);					//Coilfang Reservoir
void SetupTheSteamvault(ScriptMgr * mgr);				//Coilfang Reservoir
void SetupTheSlavePens(ScriptMgr * mgr);				//Coilfang Reservoir
void SetupGruulsLair(ScriptMgr * mgr);					//Gruul's Lair
void SetupHellfireRamparts(ScriptMgr * mgr);			//Hellfire Citadel
void SetupMagtheridonsLair(ScriptMgr * mgr);			//Hellfire Citadel
void SetupTheShatteredHalls(ScriptMgr * mgr);			//Hellfire Citadel
void SetupTheBloodFurnace(ScriptMgr * mgr);				//Hellfire Citadel
void SetupKarazhan(ScriptMgr * mgr);					//Karazhan
void SetupMagistersTerrace(ScriptMgr * mgr);			//Magisters' Terrace
void SetupSunwellPlateau(ScriptMgr * mgr);		//Sunwell Plateau
void SetupTheMechanar(ScriptMgr * mgr);					//Tempest Keep
void SetupArcatraz(ScriptMgr * mgr);					//Tempest Keep
void SetupBotanica(ScriptMgr * mgr);					//Tempest Keep
void SetupTheEye(ScriptMgr * mgr);						//Tempest Keep
void SetupZulAman(ScriptMgr * mgr);						//Zul'Aman
#endif


#ifdef LichKing
//void SetupTheOldKingdom(ScriptMgr * mgr);				//Azjol-Nerub
void SetupAzjolNerub(ScriptMgr * mgr);					//Azjol-Nerub
//void SetupTrialOfTheCrusader(ScriptMgr * mgr);		//Crusaders' Coliseum
//void SetupTrialOfTheChampion(ScriptMgr * mgr);		//Crusaders' Coliseum
void SetupDrakTharonKeep(ScriptMgr* mgr);		//Drak'TharonKeep
//void SetupGundrak(ScriptMgr * mgr);					//Gundrak
//void SetupHallsOfReflection(ScriptMgr * mgr);			//Icecrown Citadel
//void SetupIcecrownCitadel(ScriptMgr * mgr);			//Icecrown Citadel
//void SetupTheForgeOfSouls(ScriptMgr * mgr);			//Icecrown Citadel
//void SetupPitOfSaron(ScriptMgr * mgr);				//Icecrown Citadel
void SetupICC(ScriptMgr * mgr);                         //Icecrown Citadel
void SetupNaxxramas(ScriptMgr * mgr);					//Naxxramas
void SetupOnyxiasLair(ScriptMgr * mgr);					//Onyxias Lair
void SetupCullingOfStratholme(ScriptMgr * mgr);			//The Culling of Stratholme
//void SetupTheEyeOfEternity(ScriptMgr * mgr);			//The Nexus
//void SetupTheOculus(ScriptMgr * mgr);					//The Nexus
void SetupNexus(ScriptMgr * mgr);						//The Nexus
//void SetupTheVioletHold(ScriptMgr * mgr);				//The Violet Hold
void SetupHallsOfStone(ScriptMgr * mgr);				//Ulduar
void SetupHallsofLightning(ScriptMgr * mgr);			//Ulduar
void SetupUlduar(ScriptMgr * mgr);						//Ulduar
//void SetupUtgardePinnacle(ScriptMgr * mgr);			//Utgarde Pinnacle
void SetupUtgardeKeep(ScriptMgr * mgr);					//Utgarde Keep
void SetupTheRubySanctum(ScriptMgr * mgr);			//Wyrmrest Template
void SetupTheObsidianSanctum(ScriptMgr * mgr);		//Wyrmrest Template
#endif


void SetupWorldBosses(ScriptMgr * mgr);
//void SetupGenericAI(ScriptMgr * mgr);


/* All this shit that remains under this comment - will be removed soon. */

#endif
