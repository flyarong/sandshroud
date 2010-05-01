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

/* Arena and Honor Point Calculation System
 *    Copyright (c) 2007 Burlex
 */

#include "StdAfx.h"

initialiseSingleton( DayWatcherThread );

#ifdef WIN32
static HANDLE m_abortEvent = INVALID_HANDLE_VALUE;
#else
static pthread_cond_t abortcond;
static pthread_mutex_t abortmutex;
#endif

DayWatcherThread::DayWatcherThread()
{
	m_threadRunning = true;
	m_dirty = false;
	m_creatureEventSpawnMaps.clear();
	m_gameobjectEventSpawnMaps.clear();
}

DayWatcherThread::~DayWatcherThread()
{
}

void DayWatcherThread::terminate()
{
	m_threadRunning = false;
#ifdef WIN32
	SetEvent(m_abortEvent);
#else
	pthread_cond_signal(&abortcond);
#endif
}

void DayWatcherThread::dupe_tm_pointer(tm * returnvalue, tm * mypointer)
{
	memcpy(mypointer, returnvalue, sizeof(tm));
}

void DayWatcherThread::update_settings()
{
	CharacterDatabase.Execute("REPLACE INTO server_settings VALUES(\"last_arena_update_time\", %u)", last_arena_time);
	CharacterDatabase.Execute("REPLACE INTO server_settings VALUES(\"last_dailies_reset_time\", %u)", last_daily_reset_time);
	CharacterDatabase.Execute("REPLACE INTO server_settings VALUES(\"last_eventid_time\", %u)", last_eventid_time);
}

void DayWatcherThread::load_settings()
{
	string arena_timeout = Config.MainConfig.GetStringDefault("Periods", "ArenaUpdate", "weekly");
	arena_period = get_timeout_from_string(arena_timeout.c_str(), WEEKLY);

	QueryResult * result = CharacterDatabase.Query("SELECT setting_value FROM server_settings WHERE setting_id = \"last_arena_update_time\"");
	if(result)
	{
		last_arena_time = result->Fetch()[0].GetUInt32();
		delete result;
	}
	else
	{
		DEBUG_LOG("DayWatcherThread", "Initialized Arena Updates.");
		last_arena_time = 0;
	}

	result = CharacterDatabase.Query("SELECT setting_value FROM server_settings WHERE setting_id = \"last_dailies_reset_time\"");
	if(result)
	{
		last_daily_reset_time = result->Fetch()[0].GetUInt32();
		delete result;
	}
	else
	{
		DEBUG_LOG("DayWatcherThread", "Initialized Daily Updates.");
		last_daily_reset_time = 0;
	}

	result = CharacterDatabase.Query("SELECT setting_value FROM server_settings WHERE setting_id = \"last_eventid_time\"");
	if(result)
	{
		last_eventid_time = result->Fetch()[0].GetUInt32();
		delete result;
	}
	else
		last_eventid_time = 0;

	LoadEventIdSettings();
}

void DayWatcherThread::set_tm_pointers()
{
	dupe_tm_pointer(localtime(&last_arena_time), &local_last_arena_time);
	dupe_tm_pointer(localtime(&last_daily_reset_time), &local_last_daily_reset_time);
	dupe_tm_pointer(localtime(&last_eventid_time), &local_last_eventid_time);
}

uint32 DayWatcherThread::get_timeout_from_string(const char * string, uint32 def)
{
	if(!stricmp(string, "weekly"))
		return WEEKLY;
	else if(!stricmp(string, "monthly"))
		return MONTHLY;
	else if(!stricmp(string, "daily"))
		return DAILY;
	else if(!stricmp(string, "hourly"))
		return HOURLY;
	else if(!stricmp(string, "minutely"))
		return MINUTELY;
	else
		return def;
}

bool DayWatcherThread::has_timeout_expired(tm * now_time, tm * last_time, uint32 timeoutval)
{
	switch(timeoutval)
	{
	case WEEKLY:
		{
			if( (now_time->tm_mon != last_time->tm_mon) )
				return true;

			return ( (now_time->tm_mday / 7) != (last_time->tm_mday / 7) );
		}
		
	case MONTHLY:
		return (now_time->tm_mon != last_time->tm_mon);

	case HOURLY:
		return ((now_time->tm_hour != last_time->tm_hour) || (now_time->tm_mday != last_time->tm_mday) || (now_time->tm_mon != last_time->tm_mon));

	case DAILY:
		return ((now_time->tm_mday != last_time->tm_mday) || (now_time->tm_mday != last_time->tm_mday));
	case MINUTELY:
			return ((now_time->tm_min != last_time->tm_min) || (now_time->tm_hour != last_time->tm_hour) || (now_time->tm_mday != last_time->tm_mday) || (now_time->tm_mon != last_time->tm_mon));
	}
	return false;
}

bool DayWatcherThread::run()
{
	SetThreadName("DayWatcher");

	Log.Notice("DayWatcherThread", "Started.");
	_loaded = false;
	currenttime = UNIXTIME;
	dupe_tm_pointer(localtime(&currenttime), &local_currenttime);
	load_settings();
	set_tm_pointers();
	m_busy = false;
	_firstrun[0] = true;
	_firstrun[1] = true;
	m_heroic_reset = false;

#ifdef WIN32
	m_abortEvent = CreateEvent(NULL, NULL, FALSE, NULL);
#else

	struct timeval now;
	struct timespec tv;

	pthread_mutex_init(&abortmutex,NULL);
	pthread_cond_init(&abortcond,NULL);
#endif
	uint32 interv = 120000;//Daywatcher check interval (in ms), must be >> 30secs !
	uint32 counter = 0;

	while(m_threadRunning)
	{
		m_busy = true;
		currenttime = UNIXTIME;
		dupe_tm_pointer(localtime(&currenttime), &local_currenttime);

		if(has_timeout_expired(&local_currenttime, &local_last_arena_time, arena_period))
			update_arena();

		if(has_timeout_expired(&local_currenttime, &local_last_daily_reset_time, DAILY))
		{
			update_daily();
			runEvents = true;
		}

		// reset will occur daily between 07:59:00 CET and 08:01:30 CET (players inside will get 60 sec countdown)
		// 8AM = 25200s
		uint32 umod = uint32(currenttime + 3600) % 86400;
		if(!m_heroic_reset && umod >= 25140 && umod <= 25140 + (interv/1000) + 30 )
		{
			//It's approx 8AM, let's reset (if not done so already)
			Reset_Heroic_Instances();
			m_heroic_reset = true;
		}
		if(m_heroic_reset && umod > 25140 + (interv/1000) + 30 )
			m_heroic_reset = false;

		if(has_timeout_expired(&local_currenttime, &local_last_eventid_time, HOURLY))
		{
			Log.Notice("DayWatcherThread", "Running Hourly In Game Events checks...");
			for(EventsList::iterator itr = m_eventIdList.begin(); itr != m_eventIdList.end(); ++itr)
			{
				if(!(*itr)->eventbyhour)
					continue;

				if((*itr)->isactive)
				{
					if((*itr)->lastactivated && !CheckHourlyEvent(&local_currenttime, (*itr)->starthour, (*itr)->endhour))
					{
						(*itr)->isactive = false;
						SpawnEventId((*itr)->eventId, false);
						update_event_settings((*itr)->eventId,0);
					}
					else
					{
						if((*itr)->lastactivated && _firstrun[0])
						{
							if(!SpawnEventId((*itr)->eventId))
									break;
						}
						if(!(*itr)->lastactivated)
						{
						time_t activated = (*itr)->lastactivated = UNIXTIME;
						update_event_settings((*itr)->eventId, activated);
						runEvents = true;
						}
						continue;
					}
				}
				else
				{
					if(CheckHourlyEvent(&local_currenttime, (*itr)->starthour, (*itr)->endhour))
					{
						if(!SpawnEventId((*itr)->eventId))
							break;

						(*itr)->isactive = true;
						time_t activated = (*itr)->lastactivated = UNIXTIME;
						update_event_settings((*itr)->eventId, activated);
						continue;
					}
				}
			}
			_firstrun[0] = false;
			last_eventid_time = UNIXTIME;
			dupe_tm_pointer(localtime(&last_eventid_time), &local_last_eventid_time);
			m_dirty = true;
		}

		if(runEvents = true) // We run checks every 2 minutes.
		{
			if(_loaded)
			{
				runEvents = false;
				bool monthexpired = false;
				counter++;
				/* If we used sWorld.SpamWaitTime, we would do counter == (sWorld.SpamWaitTime/2 + 3)
				This would be say 30 minutes in config, so 30/2 = 15 + 2 = 17 and we set counter to 2
				every time so 17 - 2 = 15 so 15x2 for every two minutes = 30 minutes timings.*/
				if((counter <= 2) || (counter == 17/*15 + 2*/)) // First few ticks and Tick every 30 minutes and reset.
				{
					Log.Notice("DayWatcherThread", "Running In Game Events checks...");
					counter = 2;
				}

				for(EventsList::iterator itr = m_eventIdList.begin(); itr != m_eventIdList.end(); ++itr)
				{
					if((*itr)->eventbyhour)
						continue;

					if((*itr)->isactive)
					{
						if((*itr)->lastactivated && has_eventid_expired((*itr)->activedays, (*itr)->lastactivated))
						{
							(*itr)->isactive = false;
							SpawnEventId((*itr)->eventId, false);
							update_event_settings((*itr)->eventId,0);
						}
						else
						{
							if((*itr)->lastactivated && _firstrun[1])
							{
								if(!SpawnEventId((*itr)->eventId))
										break;
							}

							if(!(*itr)->lastactivated)
							{
							time_t activated = (*itr)->lastactivated = UNIXTIME;
							update_event_settings((*itr)->eventId, activated);
							runEvents = true;
							}
							continue;
						}
					}
					else
					{
						if((*itr)->monthnumber)
						{
							if(has_eventid_timeout_expired(&local_currenttime, ((*itr)->monthnumber - 1), MONTHLY))
							{
								if(!(*itr)->daynumber)
								{
									if(!SpawnEventId((*itr)->eventId))
											break;
									(*itr)->isactive = true;
									time_t activated = (*itr)->lastactivated = UNIXTIME;
									update_event_settings((*itr)->eventId, activated);
									continue;
								}
								monthexpired = true;
							}
						}
						if(monthexpired && (*itr)->daynumber && has_eventid_timeout_expired(&local_currenttime, (*itr)->daynumber, DAILY))
						{
							monthexpired = false;
							time_t activated = (*itr)->lastactivated = UNIXTIME;
							update_event_settings((*itr)->eventId, activated);
							continue;
						}
						if((*itr)->daynumber && !(*itr)->monthnumber && has_eventid_timeout_expired(&local_currenttime, (*itr)->daynumber, DAILY))
						{
							if(!SpawnEventId((*itr)->eventId))
								break;
							(*itr)->isactive = true;
							time_t activated = (*itr)->lastactivated = UNIXTIME;
							update_event_settings((*itr)->eventId, activated);
						}
					}
				}
				_firstrun[1] = false;
			}
		}
		if(m_dirty)
			update_settings();

		m_busy = false;
		if(!m_threadRunning)
			break;

#ifdef WIN32
		WaitForSingleObject(m_abortEvent, interv);
#else
		gettimeofday(&now, NULL);
		tv.tv_sec = now.tv_sec + 120;
		tv.tv_nsec = now.tv_usec * 1000;
		pthread_mutex_lock(&abortmutex);
		pthread_cond_timedwait(&abortcond, &abortmutex, &tv);
		pthread_mutex_unlock(&abortmutex);
#endif
		if(!m_threadRunning)
			break;
	}
#ifdef WIN32
	CloseHandle(m_abortEvent);
#else
	pthread_mutex_destroy(&abortmutex);
	pthread_cond_destroy(&abortcond);
#endif
	return true;
}

void DayWatcherThread::update_arena()
{
	Log.Notice("DayWatcherThread", "Running Weekly Arena Point Maintenance...");
	QueryResult * result = CharacterDatabase.Query("SELECT guid, arenaPoints FROM characters");		/* this one is a little more intensive. */
	Player* plr;
	uint32 guid, arenapoints, orig_arenapoints;
	ArenaTeam * team;
	PlayerInfo * inf;
	uint32 arenapointsPerTeam[3] = {0};
	double X, Y;
	if(result)
	{
		do
		{
			Field * f = result->Fetch();
			guid = f[0].GetUInt32();

			inf = objmgr.GetPlayerInfo(guid);
			if( inf == NULL )
				continue;

			arenapoints = f[1].GetUInt32();
			orig_arenapoints = arenapoints;

			for(uint32 i = 0; i < 3; ++i)
				arenapointsPerTeam[i] = 0;

			/* are we in any arena teams? */
			for(uint32 i = 0; i < 3; ++i)			// 3 arena team types
			{
				team = inf->arenaTeam[i];
				if(team)
				{
					ArenaTeamMember *member = team->GetMemberByGuid(guid);
					if(member == NULL || team->m_stat_gamesplayedweek < 10 || ((member->Played_ThisWeek * 100) / team->m_stat_gamesplayedweek < 30))
 						continue;

					/* we're in an arena team of this type! */
					/* Source: http://www.wowwiki.com/Arena_point */
					X = (double)team->m_stat_rating;
					if(X <= 510.0)	// "if X<=510"
						continue;		// no change
					else if(X > 510.0 && X <= 1500.0)		// "if 510 < X <= 1500"
					{
						Y = (0.22 * X) + 14.0;
					}
					else			// "if X > 1500"
					{
						// http://eu.wowarmory.com/arena-calculator.xml
						//              1511.26
						//   ---------------------------
						//                   -0.00412*X
						//    1+1639.28*2.71828

						double power = ((-0.00412) * X);
						//if(power < 1.0)
						//	power = 1.0;

						double divisor = pow(((double)(2.71828)), power);						
						divisor *= 1639.28;
						divisor += 1.0;
						//if(divisor < 1.0)
						//	divisor = 1.0;

						Y = 1511.26 / divisor;
					}

					// 2v2 teams only earn 70% (Was 60% until 13th March 07) of the arena points, 3v3 teams get 80%, while 5v5 teams get 100% of the arena points.
					// 2v2 - 76%, 3v3 - 88% as of patch 2.2
					if(team->m_type == ARENA_TEAM_TYPE_2V2)
					{
						Y *= 0.76;
						Y *= sWorld.getRate(RATE_ARENAPOINTMULTIPLIER2X);
					}
					else if(team->m_type == ARENA_TEAM_TYPE_3V3)
					{
						Y *= 0.88;
						Y *= sWorld.getRate(RATE_ARENAPOINTMULTIPLIER3X);
					}
					else
					{
						Y *= sWorld.getRate(RATE_ARENAPOINTMULTIPLIER5X);
					}
					
					if(Y > 1.0)
						arenapointsPerTeam[i] += long2int32(double(ceil(Y)));
				}
			}

			arenapointsPerTeam[0] = (uint32)max(arenapointsPerTeam[0],arenapointsPerTeam[1]);
			arenapoints += (uint32)max(arenapointsPerTeam[0],arenapointsPerTeam[2]);

			if(orig_arenapoints != arenapoints)
			{
				plr = objmgr.GetPlayer(guid);
				if(plr!=NULL)
				{
					plr->m_arenaPoints = arenapoints;
					
					/* update visible fields (must be done through an event because of no uint lock */
					sEventMgr.AddEvent(plr, &Player::RecalculateHonor, EVENT_PLAYER_UPDATE, 100, 1, 0);
	
					/* send a little message :> */
					sChatHandler.SystemMessage(plr->GetSession(), "Your arena points have been updated! Check your PvP tab!");
				}

				/* update in sql */
				CharacterDatabase.Execute("UPDATE characters SET arenaPoints = %u WHERE guid = %u", arenapoints, guid);
			}
		}while(result->NextRow());
		delete result;
	}

	objmgr.UpdateArenaTeamWeekly();

	//===========================================================================
	last_arena_time = UNIXTIME;
	dupe_tm_pointer(localtime(&last_arena_time), &local_last_arena_time);
	m_dirty = true;
}

void DayWatcherThread::update_daily()
{
	Log.Notice("DayWatcherThread", "Running Daily Quest Reset...");
	CharacterDatabase.WaitExecute("UPDATE characters SET finished_daily_quests = ''");
	objmgr.ResetDailies();
	last_daily_reset_time = UNIXTIME;
	dupe_tm_pointer(localtime(&last_daily_reset_time), &local_last_daily_reset_time);
	m_dirty = true;
}

void DayWatcherThread::Reset_Heroic_Instances()
{
	Log.Notice("DayWatcherThread", "Reseting heroic instances...");
	sInstanceMgr.ResetHeroicInstances();
}
