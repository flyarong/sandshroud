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

initialiseSingleton(ClusterInterface);

extern bool bServerShutdown;

ClusterInterfaceHandler ClusterInterface::PHandlers[MSGR_NUM_TYPES];
void ClusterInterface::InitHandlers()
{
	memset(PHandlers, 0, sizeof(void*) * MSGR_NUM_TYPES);
	PHandlers[SMSGR_AUTH_REQUEST]							= &ClusterInterface::HandleAuthRequest;
	PHandlers[SMSGR_AUTH_RESULT]							= &ClusterInterface::HandleAuthResult;
	PHandlers[SMSGR_REGISTER_RESULT]						= &ClusterInterface::HandleRegisterResult;
	PHandlers[SMSGR_CREATE_INSTANCES]						= &ClusterInterface::HandleCreateInstance;
	PHandlers[SMSGR_PLAYER_LOGIN]							= &ClusterInterface::HandlePlayerLogin;
	PHandlers[SMSGR_WOW_PACKET]								= &ClusterInterface::HandleWoWPacket;
	PHandlers[SMSGR_TELEPORT_RESULT]						= &ClusterInterface::HandleTeleportResult;
	PHandlers[SMSGR_ERROR_HANDLER]							= &ClusterInterface::HandleServerError;
	PHandlers[SMSGR_SESSION_REMOVED]						= &ClusterInterface::HandleSessionRemoved;
	PHandlers[SMSGR_SAVE_ALL_PLAYERS]						= &ClusterInterface::HandleSaveAllPlayers;
	PHandlers[SMSGR_TRANSPORTER_MAP_CHANGE]					= &ClusterInterface::HandleTransporterMapChange;
	PHandlers[SMSGR_PLAYER_TELEPORT]						= &ClusterInterface::HandlePlayerTeleport;
	PHandlers[SMSGR_PACKED_PLAYER_INFO]						= &ClusterInterface::HandlePackedPlayerInfo;
	PHandlers[SMSGR_DESTROY_PLAYER_INFO]					= &ClusterInterface::HandleDestroyPlayerInfo;
	PHandlers[SMSGR_PLAYER_INFO]							= &ClusterInterface::HandlePlayerInfo;
	PHandlers[SMSGR_CHANNEL_ACTION]							= &ClusterInterface::HandleChannelAction;
	PHandlers[SMSGR_CHANNEL_LFG_DUNGEON_STATUS_REQUEST]		= &ClusterInterface::HandleChannelLFGDungeonStatusRequest;
}

ClusterInterface::ClusterInterface()
{
	ClusterInterface::InitHandlers();
	m_connected = false;
	memset(_sessions, NULL, sizeof(WorldSession*)*MAX_SESSIONS);
}

ClusterInterface::~ClusterInterface()
{

}

void ClusterInterface::HandleServerError(WorldPacket & pck)
{
	uint32 reason;
	pck >> reason;

	Log.Warning("ClusterInterface", "Slave rejected, shutting down(%u).", reason);
	m_connected = false;
	_clientSocket = NULL;
	sWorld.QueueShutdown(5, 0);
}

string ClusterInterface::GenerateVersionString()
{
	char str[200];
	snprintf(str, 200, "Hearthstone r%u/%s-%s-%s", BUILD_REVISION, CONFIG, PLATFORM_TEXT, ARCH);
	return string(str);
}

void ClusterInterface::ConnectionDropped()
{
	Log.Warning("ClusterInterface", "Socket disconnected, will attempt reconnect later");
	m_connected = false;
	_clientSocket = NULL;
}

void ClusterInterface::ForwardWoWPacket(uint16 opcode, uint32 size, const void * data, uint32 sessionid)
{
	bool rv;
	uint16 id = CMSGR_WOW_PACKET;
	uint32 size2 = 10 + size;

	if(!_clientSocket)
		return;			// Shouldn't happen
	DEBUG_LOG("ForwardWoWPacket", "Forwarding %s to server", LookupName(opcode, g_worldOpcodeNames));

	_clientSocket->LockWriteBuffer();
	_clientSocket->WriteButHold((const uint8*)&id, 2);
	_clientSocket->WriteButHold((const uint8*)&size2, 4);
	_clientSocket->WriteButHold((const uint8*)&sessionid, 4);
	_clientSocket->WriteButHold((const uint8*)&opcode, 2);
	rv = _clientSocket->WriteButHold((const uint8*)&size, 4);
	if(size && rv)
		rv =_clientSocket->Write((const uint8*)data, size);
	else if(rv)
		_clientSocket->ForceSend();

	_clientSocket->UnlockWriteBuffer();
}

void ClusterInterface::ConnectToRealmServer()
{
	_lastConnectTime = UNIXTIME;

	int port;
	string strkey, hostname;
	if(!Config.MainConfig.GetString("Cluster", "RSHostName", &hostname) || !Config.MainConfig.GetInt("Cluster", "RSPort", &port) || !Config.MainConfig.GetString("Cluster", "Key", &strkey))
	{
		Log.Error("ClusterInterface", "Could not get necessary fields from config file. Please fix and rehash.");
		return;
	}

	/* hash the key */
	Sha1Hash k;
	k.UpdateData(strkey);
	k.Finalize();
	memcpy(key, k.GetDigest(), 20);

	Log.Notice("ClusterInterface", "Connecting to %s port %u", hostname.c_str(), port);
	WSClient *s = ConnectTCPSocket<WSClient>(hostname.c_str(), port);
	if(!s)
	{
		Log.Error("ClusterInterface", "Could not connect to %s:%u", hostname.c_str(), port);
		return;
	}

	Log.Success("ClusterInterface", "Connected to %s:%u", hostname.c_str(), port);

	_clientSocket = s;
	m_latency = getMSTime();
	m_connected = true;
}

void ClusterInterface::HandleAuthRequest(WorldPacket & pck)
{
	uint32 x;
	pck >> x;
	OUT_DEBUG("ClusterInterface", "Incoming auth request from %s (RS build %u)", _clientSocket->GetIP(), x);

	WorldPacket data(CMSGR_AUTH_REPLY, 50);
	data.append(key, 20);
	data << uint32(BUILD_REVISION);
	data << GenerateVersionString();
	SendPacket(&data);

	m_latency = getMSTime() - m_latency;
	Log.Notice("ClusterInterface", "Latency between realm server is %u ms", m_latency);
}

void ClusterInterface::HandleAuthResult(WorldPacket & pck)
{
	uint32 res;
	pck >> res;
	DEBUG_LOG("ClusterInterface", "Auth Result: %u", res);
	if(!res)
	{
		Log.Error("ClusterInterface", "Authentication Failed");
		_clientSocket->Disconnect();
		_clientSocket = 0;
		return;
	}

	Log.Notice("ClusterInterface", "Loading BaseMap and InstanceMap Info");
	std::map<uint32, uint32> maplist;
	for(set<uint32>::iterator itr = sWorld.loadmaps.begin(); itr != sWorld.loadmaps.end(); itr++)
		maplist.insert(make_pair(*itr, LimitedMapInfoStorage.LookupEntry(*itr)->type));

	WorldPacket data(CMSGR_REGISTER_WORKER, 12);
	data << uint32(BUILD_REVISION);
	data << maplist;
	SendPacket(&data);
}

void ClusterInterface::HandleRegisterResult(WorldPacket & pck)
{
	uint32 res;
	pck >> res;
	if(!res)
	{
		_clientSocket->Disconnect();
		_clientSocket = NULL;
	}
	DEBUG_LOG("ClusterInterface", "Register Result: %s.", ((res > 0) ? "true" : "false"));
}

void ClusterInterface::HandleCreateInstance(WorldPacket & pck)
{
	set<uint32> mapids;
	pck >> mapids;
	if(!mapids.size())
	{
		_clientSocket->Disconnect();
		_clientSocket = NULL;
	}

	sInstanceMgr.Load(mapids);
}

void ClusterInterface::HandleDestroyInstance(WorldPacket & pck)
{

}

void ClusterInterface::HandlePlayerLogin(WorldPacket & pck)
{
	/* player x logging into instance y */
	uint32 Guid, InstanceId, MapId;
	uint32 AccountId, Account_Flags, sessionid, ClientBuild;
	string GMPermissions, accountname;
	pck >> Guid >> MapId >> InstanceId >> AccountId >> Account_Flags >> sessionid >> GMPermissions >> accountname >> ClientBuild;

	/* find the instance */
	Map* ma = sInstanceMgr.GetMap(MapId);
	ASSERT(ma);
	MapMgr* mm = sInstanceMgr.GetInstance(MapId, InstanceId);
	ASSERT(mm);

	/* create the session */
	WorldSession * s = sWorld.FindSession(AccountId);

	/* create the socket */
	WorldSocket * so = new WorldSocket(sessionid);
	if (s == NULL)
		s = new WorldSession(AccountId, accountname, so);
	_sessions[sessionid] = s;
	sWorld.AddSession(s);

	for(uint8 i = 0; i < 8; i++)
		s->SetAccountData(i, NULL, true, 0);

	if(pck.rpos() != pck.wpos())
	{
		uint32 size = 0;
		for(uint8 i = 0; i < 8; i++)
		{
			if(pck.rpos()+4 > pck.wpos())
				break; // Out of packet.

			pck >> size;
			if(size)
			{
				char* data = new char[size];
				pck.read(((uint8*)data), size);
				s->SetAccountData(i, data, true, size);
			}
		}
	}

	bool login_result = s->ClusterTryPlayerLogin(Guid, ClientBuild, GMPermissions, Account_Flags);
	if(login_result)
	{
		/* login was ok. send a message to the realm server telling him to distribute our info to all other realm server */
		WorldPacket data(CMSGR_PLAYER_LOGIN_RESULT, 5);
		data << Guid << sessionid <<  uint8(1);
		SendPacket(&data);
	}
	else
	{
		/* for some reason the login failed */
		WorldPacket data(CMSGR_PLAYER_LOGIN_RESULT, 5);
		data << Guid << sessionid << uint8(0);
		SendPacket(&data);

		/* tell the client his login failed before deleting the session */
		data.Initialize(SMSG_CHARACTER_LOGIN_FAILED);
		data << uint8(62);
		so->SendPacket(&data);

		/* destroy the session */
		DestroySession(sessionid);
	}
}

void ClusterInterface::HandleDestroyPlayerInfo(WorldPacket & pck)
{
	uint32 sessionid, guid;
	pck >> sessionid >> guid;

	if(_onlinePlayers[guid])
	{
		delete _onlinePlayers[guid];
		_onlinePlayers[guid] = NULL;
	}

	Player * player = objmgr.GetPlayer(guid);
	if(player)
	{
		WorldSession* session = NULL;
		if((session = player->GetSession()) != NULL)
		{
			player->SetSession(NULL);
			session->SetPlayer(NULL);
			session->SetSocket(NULL);
			DestroySession(sessionid);
		}
	}
}

void ClusterInterface::HandlePackedPlayerInfo(WorldPacket & pck)
{
	RPlayerInfo * pi;
	// todo: uncompress
	uint32 count;
	pck >> count;
	for(uint32 i = 0; i < count; i++)
	{
		pi = new RPlayerInfo;
		pi->Unpack(pck);
		_onlinePlayers[pi->Guid] = pi;
	}
}

void ClusterInterface::Update()
{
	if(bServerShutdown)
	{
		WorldPacket * pck;
		while((pck = _pckQueue.Pop()))
			delete pck;
		return;
	}

	if(bServerShutdown && !m_connected && UNIXTIME >= (_lastConnectTime + 3))
		ConnectToRealmServer();

	WorldPacket * pck;
	uint16 opcode;
	while((pck = _pckQueue.Pop()))
	{
		opcode = pck->GetOpcode();
		if(opcode < MSGR_NUM_TYPES && ClusterInterface::PHandlers[opcode] != 0)
			(this->*ClusterInterface::PHandlers[opcode])(*pck);
		else
			Log.Error("ClusterInterface", "Unhandled packet %u\n", opcode);
	}
}

void ClusterInterface::DestroySession(uint32 sid)
{
	WorldSession * s = _sessions[sid];
	_sessions[sid] = NULL;
	if(s)
	{
		/* todo: replace this with an event so we don't remove from the wrong thread */
		if(s->GetPlayer())
			s->LogoutPlayer(true);

		delete s->GetSocket();
		sWorld.RemoveGlobalSession(s);
		sWorld.RemoveSession(sid);
	}
}


void ClusterInterface::HandleWoWPacket(WorldPacket & pck)
{
	uint32 size, sid;
	uint16 opcode;
	pck >> sid >> opcode >> size;

	if(!_sessions[sid])
	{
		Log.Error("HandleWoWPacket", "Invalid session: %u", sid);
		return;
	}

	DEBUG_LOG("HandleWoWPacket", "Forwarding %s to client", LookupName(opcode, g_worldOpcodeNames));

	WorldPacket * npck = new WorldPacket(opcode, size);
	npck->resize(size);
	memcpy((void*)npck->contents(), pck.contents()+10, size);
	_sessions[sid]->QueuePacket(npck);
}

void ClusterInterface::RequestTransfer(Player* plr, uint32 MapId, uint32 InstanceId, LocationVector & vec)
{
	WorldPacket data(CMSGR_TELEPORT_REQUEST, 32);
	data << plr->GetSession()->GetSocket()->GetSessionId() << MapId << InstanceId << vec << vec.o;
	SendPacket(&data);
}

/*
void ClusterInterface::RequestWhisper(PlayerPointer plr, uint32 senderguid, uint32 recieveguid, int32 lang, string msg, string misc)
{
	WorldPacket data(CMSGR_WHISPER, 1000);
	data << plr->GetSession()->GetSocket()->GetSessionId() << senderguid << recieveguid << lang << msg << misc;
	SendPacket(&data);
}

void ClusterInterface::HandleWhisperResult(WorldPacket & pck)
{
	int32 lang;
	uint32 senderguid, recieveguid;
	string msg, misc;
	PlayerPointer player;

	pck >> senderguid >> recieveguid;

	player = objmgr.GetPlayer(recieveguid);

	if (player == NULL)
		return;


	pck >> lang >> msg >> misc;

	player->BuildWhisperData(senderguid, recieveguid, lang, msg, misc);

}
*/

void ClusterInterface::HandleTeleportResult(WorldPacket & pck)
{
	uint32 sessionid;
	uint8 result;
	uint32 mapid, instanceid;
	LocationVector vec;
	float o;

	pck >> sessionid;

	WorldSession* s = GetSession(sessionid);

	if (!s)
	{
		//tell the realm-server we have no session
		WorldPacket data(CMSGR_ERROR_HANDLER, 5);
		data << uint8(1); //1 = no session
		data << sessionid;
		sClusterInterface.SendPacket(&data);
		return;
	}

	pck >> result >> mapid >> instanceid >> vec >> o;

	//the destination is on the same server
	if (result == 1)
	{
		if (s->GetPlayer() != NULL)
			sEventMgr.AddEvent(s->GetPlayer(), &Player::EventClusterMapChange, mapid, instanceid, vec, EVENT_UNK, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	}
	else
	{
		//make this non-async, needs redone to support packing the player
		//since were saving it HAS TO BE HERE so the new server has the correct data
		WorldPacket nw(SMSG_NEW_WORLD);
		nw << mapid << vec << o;
		s->SendPacket(&nw);

		uint32 oldmapid = s->GetPlayer()->GetMapId();
		uint32 oldinstanceid = s->GetPlayer()->GetInstanceID();
		uint32 playerlowguid = s->GetPlayer()->GetLowGUID();
		s->GetPlayer()->SetMapId(mapid);
		s->GetPlayer()->SetInstanceID(instanceid);
		s->GetPlayer()->SetPosition(vec.x, vec.y, vec.z, o);
		s->GetPlayer()->SaveToDB(true);

		//need to shift back to old ones for removing from world :)
		s->GetPlayer()->SetMapId(oldmapid);
		s->GetPlayer()->SetInstanceID(oldinstanceid);

		WorldPacket data(CMSGR_SWITCH_SERVER, 100);
		data << sessionid << playerlowguid << mapid << instanceid << vec << o;
		sClusterInterface.SendPacket(&data);

		RPlayerInfo * pRPlayer = GetPlayer(playerlowguid);
		bool newRplr = false;
		if(pRPlayer == NULL)
		{
			pRPlayer = new RPlayerInfo;
			newRplr = true;
		}
		s->GetPlayer()->UpdateRPlayerInfo(pRPlayer, newRplr);
		pRPlayer->MapId = mapid;
		pRPlayer->InstanceId = instanceid;
		data.Initialize(CMSGR_PLAYER_INFO);
		pRPlayer->Pack(data);
		sClusterInterface.SendPacket(&data);

		//oh ye, we need to set player to delete later, remove player from world, and destroy session
		//we must set sessions player to null so we don't send any logout crap
		//WE MUST REMOVE ALL AURAS BEFORE REMOVING FROM WORLD :D
		sEventMgr.AddEvent(s->GetPlayer(), &Player::HandleClusterRemove, EVENT_UNK, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	}
}

void ClusterInterface::HandlePlayerInfo(WorldPacket & pck)
{
	uint32 guid;
	pck >> guid;
	RPlayerInfo * pRPlayer = GetPlayer(guid);
	if(!pRPlayer)
		pRPlayer = new RPlayerInfo;

	pRPlayer->Unpack(pck);

	_onlinePlayers[pRPlayer->Guid] = pRPlayer;
}

bool WorldSession::ClusterTryPlayerLogin(uint32 Guid, uint32 ClientBuild, string GMPermissions, uint32 Account_Flags)
{
	DEBUG_LOG( "WorldSession"," Recvd Player Logon Message" );

	if(objmgr.GetPlayer(Guid) != NULL || m_loggingInPlayer || _player)
	{
		// A character with that name already exists 0x3E
		uint8 respons = 0x3E;
		OutPacket(SMSG_CHARACTER_LOGIN_FAILED, 1, &respons);
		return false;
	}

	Player* plr = new Player(Guid);
	ASSERT(plr);
	plr->Init();

	permissioncount = 0;//just to make sure it's 0
	SetClientBuild(ClientBuild);
	LoadSecurity(GMPermissions);
	SetAccountFlags(Account_Flags);

	plr->SetSession(this);
	m_bIsWLevelSet = false;

	DEBUG_LOG("WorldSession", "Async loading player %u", Guid);
	m_loggingInPlayer = plr;
	plr->LoadFromDB(Guid);
	return true;
}

//This must be an event to stay in the correct context!
void Player::EventClusterMapChange(uint32 mapid, uint32 instanceid, LocationVector location)
{
	WorldPacket data;
	uint32 status = sInstanceMgr.PreTeleport(mapid, this, instanceid);
	if(status != INSTANCE_OK)
	{
		data.Initialize(SMSG_TRANSFER_ABORTED);
		data << mapid << status;
		GetSession()->SendPacket(&data);
		return;
	}

	if(instanceid)
		m_instanceId = instanceid;

	if(IsInWorld())
		RemoveFromWorld();

	data.Initialize(SMSG_NEW_WORLD);
	data << (uint32)mapid << location << location.o;
	GetSession()->SendPacket( &data );
	SetMapId(mapid);

	SetPlayerStatus(TRANSFER_PENDING);
	m_sentTeleportPosition = location;
	SetPosition(location);
	ResetHeartbeatCoords();
	z_axisposition = 0.0f;
}

void Player::HandleClusterRemove()
{
	RemoveAllAuras();
	if (IsInWorld())
		RemoveFromWorld();

	sEventMgr.AddEvent(TO_OBJECT(this), &Object::Destruct, EVENT_UNK, 30000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT | EVENT_FLAG_DELETES_OBJECT);

	if (GetSession() != NULL)
	{
		GetSession()->SetPlayer(NULLPLR);
		if (GetSession()->GetSocket() != NULL)
		{
			uint32 sessionid = GetSession()->GetSocket()->GetSessionId();
			sClusterInterface.DestroySession(sessionid);
		}
		SetSession(NULL);
	}
	objmgr.RemovePlayer(this);
}

void ClusterInterface::HandleSessionRemoved(WorldPacket & pck)
{
	uint32 sessionid;
	pck >> sessionid;
	DestroySession(sessionid);
}

void ClusterInterface::HandleSaveAllPlayers(WorldPacket & pck)
{
	sWorld.SaveAllPlayers();
}

void ClusterInterface::HandleTransporterMapChange(WorldPacket & pck)
{
	//remove when this is stable, ROFL
	DEBUG_LOG("Transport", "Handling clustered map change");
	uint32 tentry, mapid;
	float x, y, z;
	pck >> tentry >> mapid >> x >> y >> z;

	Transporter* t = objmgr.GetTransporterByEntry(tentry);

	//we need to add to our map
	MapMgr* mgr = sInstanceMgr.GetMapMgr(mapid);

	LocationVector l;
	l.x = x;
	l.y = y;
	l.z = z;

	if (mgr == NULL)
		return;

	if (t->IsInWorld())
		t->RemoveFromWorld(false);
	t->SetMapId(mapid);
	//don't start instantly, we start after eventclustermapchange is finished :P
	sEventMgr.RemoveEvents(t);
	//t->m_canmove = false;
	t->AddToWorld(mgr);
	sEventMgr.AddEvent(t, &Transporter::EventClusterMapChange, mapid, l, EVENT_UNK, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT | EVENT_FLAG_MOVE_TO_WORLD_CONTEXT);
}

void ClusterInterface::HandlePlayerTeleport(WorldPacket & pck)
{
	//result has to be 2 here
	uint8 result, method;
	uint32 sessionid, mapid, instanceid;
	LocationVector location;
	uint32 sessionid2;

	pck >> result >> method;

	//someones messing with something
	if (result != 2)
		return;

	pck >> sessionid >> mapid >> instanceid >> location >> sessionid2;

	//we need to get sessionid2!
	WorldSession* s = GetSession(sessionid2);
	if (s == NULL)
		return;

	//if method is 0, simply teleport us to the location provided :)
	if (method == 0)
		sEventMgr.AddEvent(s->GetPlayer(), &Player::EventSafeTeleport, mapid, instanceid, location, 1, EVENT_UNK, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	else
	{
		Player* p = s->GetPlayer();
		if (p == NULL)
			return; //wtf
		//change the method for 0, and fill it reversely, so the player is teleported to us :)
		//result has to be 2 when sending back here, so it relays it to the original players server
		WorldPacket data(CMSGR_PLAYER_TELEPORT);
		data << uint8(2) << uint8(0) << sessionid2 << p->GetMapId() << p->GetInstanceID() << p->GetPosition() << sessionid;
	}
}

void ClusterInterface::HandleChannelAction(WorldPacket & pck)
{
/*	uint8 action;
	pck >> action;

	switch(action)
	{
	case CHANNEL_JOIN:
		{
			uint32 guid;
			uint32 cid;
			Player * pPlayer;

			pck >> guid;
			pck >> cid;

			pPlayer = objmgr.GetPlayer(guid);
			if(pPlayer == NULL)
				return;

			pPlayer->JoinedChannel(cid);
			break;

		}
	case CHANNEL_PART:
		{
			uint32 guid;
			uint32 cid;
			Player * pPlayer;

			pck >> guid;
			pck >> cid;

			pPlayer = objmgr.GetPlayer(guid);
			if(pPlayer == NULL)
				return;

			pPlayer->LeftChannel(cid);
			break;
		}
	default:
		{
			DEBUG_LOG("ClusterInterface", "HandleChannelAction opcode, unhandled action %u", action);
			return;
		}
	}*/
}

void ClusterInterface::HandleChannelLFGDungeonStatusRequest(WorldPacket & pck)
{
/*	uint32 guid;
	uint32 dbc_id;
	uint16 unk;
	string channelname;
	string pass;
	pck >> guid;
	pck >> dbc_id >> unk;
	pck >> channelname;
	pck >> pass;
	uint8 i = 0;

	Player * pPlayer = NULL;
	pPlayer = objmgr.GetPlayer(guid);
	if(!pPlayer)
		return;

	WorldPacket data(CMSGR_CHANNEL_LFG_DUNGEON_STATUS_REPLY, 1+4+4+2+channelname.size()+pass.size());


	//make sure we have lfg dungeons
	for(i = 0; i < 3; ++i)
	{
		if(pPlayer->LfgDungeonId[i] != 0)
			break;
	}

	data << i;
	data << guid;
	data << dbc_id;
	data << unk;
	data << channelname;
	data << pass;
	sClusterInterface.SendPacket(&data);*/
}

void ClusterInterface::PlayerLoggedOut(uint32 sid, uint32 guid)
{
	WorldPacket result(CMSGR_PLAYER_LOGOUT, 8);
	result << sid << guid;
	SendPacket(&result);
}
