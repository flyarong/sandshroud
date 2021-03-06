/*
 * Sandshroud Hearthstone
 * FeatherMoonEmu by Crow@Sandshroud
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


#ifndef AUTHSOCKET_H
#define AUTHSOCKET_H

#include "AccountCache.h"
#include "AuthStructs.h"

class LogonCommServerSocket;
struct Patch;
class PatchJob;

class AuthSocket : public TcpSocket
{
	friend class LogonCommServerSocket;
public:

	///////////////////////////////////////////////////
	// Netcore shit
	//////////////////////////
	AuthSocket(SOCKET fd, const sockaddr_in * peer);
	~AuthSocket();

	void OnRecvData();

	///////////////////////////////////////////////////
	// Client Packet Handlers
	//////////////////////////

	void HandleChallenge();
	void HandleProof();
	void HandleRealmlist();
	void HandleReconnectChallenge();
	void HandleReconnectProof();
	void HandleTransferAccept();
	void HandleTransferResume();
	void HandleTransferCancel();
	void HandleCMD19();

	///////////////////////////////////////////////////
	// Server Packet Builders
	//////////////////////////

	void SendChallengeError(uint8 Error);
	void SendProofError(uint8 Error, uint8 * M2);
	HEARTHSTONE_INLINE sAuthLogonChallenge_C * GetChallenge() { return &m_challenge; }
	HEARTHSTONE_INLINE void Send(const uint8* data, const uint16 len) { SendPacket(data, len); };
	HEARTHSTONE_INLINE void SendPacket(const uint8* data, const uint16 len)
	{
		LockWriteBuffer();
		Write(data, len);
		UnlockWriteBuffer();
	}

	void OnDisconnect();
	HEARTHSTONE_INLINE time_t GetLastRecv() { return last_recv; }
	bool removedFromSet;
	HEARTHSTONE_INLINE uint32 GetAccountID() { return m_account ? m_account->AccountId : 0; }
	HEARTHSTONE_INLINE string GetAccountName() { return AccountName; }

	uint8 GetClientExpansion() { return m_challenge.version[0]; };
	uint8 GetClientPatch() { return m_challenge.version[1]; };
	uint8 GetClientVersion() { return m_challenge.version[2]; };
	uint16 GetBuild() { return m_challenge.build; };

protected:

	sAuthLogonChallenge_C m_challenge;
	Account * m_account;
	bool m_authenticated;
	std::string AccountName;

	//////////////////////////////////////////////////
	// Authentication BigNumbers
	/////////////////////////
	BigNumber N, s, g, v;
	BigNumber b, B;
	BigNumber K;
	BigNumber rs;

	//////////////////////////////////////////////////
	// Session Key
	/////////////////////////

	BigNumber m_sessionkey;
	time_t last_recv;

	//////////////////////////////////////////////////////////////////////////
	// Patching stuff
	//////////////////////////////////////////////////////////////////////////
public:
	Patch * m_patch;
	PatchJob * m_patchJob;
};

#endif
