/*
 * Lacrimi Scripts Copyright 2010 - 2011
 *
 * ############################################################
 * # ##            #       ####### ####### ##    #    #    ## #
 * # ##           ###      ##      ##   ## ##   ###  ###   ## #
 * # ##          ## ##     ##      ##   ## ##   ###  ###   ## #
 * # ##         #######    ##      ####### ##  ## #### ##  ## #
 * # ##        ##     ##   ##      #####   ##  ## #### ##  ## #
 * # ##       ##       ##  ##      ##  ##  ## ##   ##   ## ## #
 * # ####### ##         ## ####### ##   ## ## ##   ##   ## ## #
 * # :::::::.::.........::.:::::::.::...::.::.::...::...::.:: #
 * ############################################################
 *
 */

#ifndef LACRIMI_H
#define LACRIMI_H

class SCRIPT_DECL Lacrimi : public ThreadContext
{
private:
	ConfigFile LacrimiConfig;
	Database* LacrimiDB;
	ScriptMgr* sMgr;
	bool dumpstats;
	bool database;
	bool config;
	bool first;

public:
	Lacrimi(ScriptMgr* mgr);
	~Lacrimi();

	bool run();
	void terminate();
	void Delay(uint32 time);
	bool _StartDB();
	void _StopDB();
	string GetConfigString(char* configfamily, char* configoption, char* cdefault = "");
	float GetConfigfloat(char* configfamily, char* configoption, float fdefault = 0.0f);
	bool GetConfigBool(char* configfamily, char* configoption, bool bdefault = false);
	int GetConfigInt(char* configfamily, char* configoption, int intdefault = 0);
	Database* GetLDB() { if(database == true) return LacrimiDB; return NULL; };

public: // Script Related
	void SetupScripts();
	void SetupZoneScripts();
	void SetupCityScripts();

///////////////////
////// Zone Setups.
///////////////////

	// Eastern Kingdom
	void SetupEasternKingdomScripts();
	void SetupAlteracMountains();
	void SetupArathiHighlands();
	void SetupBlastedLands();
	void SetupBurningSteppes();
	void SetupDunMorogh();
	void SetupDuskwood();
	void SetupEasternPlaguelands();
	void SetupElwynnForest();
	void SetupEversongWoods();
	void SetupGhostlands();
	void SetupHinterlands();
	void SetupIsleOfQuelDanas();
	void SetupLochModan();
	void SetupSearingGorge();
	void SetupSilverpineForest();
	void SetupStranglethornVale();
	void SetupTrisfalGlades();
	void SetupWesternPlaguelands();
	void SetupWestfall();
	void SetupWetlands();

	// Kalimdor
	void SetupKalimdorScripts();
	void SetupAshenvale();
	void SetupAzshara();
	void SetupAzuremystIsle();
	void SetupBloodmystIsle();
	void SetupDarkshore();
	void SetupDesolace();
	void SetupDustwallowMarsh();
	void SetupFelwood();
	void SetupFeralas();
	void SetupMoonglade();
	void SetupMulgore();
	void SetupSilithus();
	void SetupStonetalonMountains();
	void SetupTanaris();
	void SetupTeldrassil();
	void SetupTheBarrens();
	void SetupThousandNeedles();
	void SetupUngoroCrater();
	void SetupWinterspring();

	// Outland
	void SetupOutlandScripts();
	void SetupBladesEdgeMountains();
	void SetupHellfirePeninsula();
	void SetupNagrand();
	void SetupNetherstorm();
	void SetupShadowmoonValley();
	void SetupTerrokarForest();
	void SetupZangarmarsh();

	// Northrend
	void SetupNorthrendScripts();
	void SetupBoreanTundra();
	void SetupDragonblight();
	void SetupGrizzlyHills();
	void SetupHowlingFjord();
	void SetupIcecrown();
	void SetupSholazarBasin();
	void SetupStormPeaks();
	void SetupZulDrak();

	// Ebon Hold
	void SetupEbonHoldScripts();

///////////////////
////// City Setups.
///////////////////

	// Eastern Kingdom
	void SetupIronforge();
	void SetupSilvermoon();
	void SetupStormwind();
	void SetupUndercity();

	// Kalimdor
	void SetupDarnassus();
	void SetupExodar();
	void SetupOrgrimmar();
	void SetupThunderbluff();

	// Outlands
	void SetupShattrath();

	// Northrend
	void SetupDalaran();


public: // Stat Dumper
	void DumpStats();
	void GenerateUptimeString(char * Dest);
	void FillOnlineTime(uint32 Time, char * Dest);

	char Filename[MAX_PATH];
};

#define LacrimiDatabase (*(sWorld.LacrimiPtr->GetLDB()))

#endif