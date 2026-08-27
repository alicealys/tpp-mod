#pragma once
#include <d3d11.h>
#include <d3d11_1.h>

namespace game
{
	// https://github.com/rlabrecque/SteamworksSDK/tree/f4d127e335a66a1615c0870830d14dcee76ed8aa/public/steam

	struct raw_steam_id final
	{
		unsigned int account_id : 32;
		unsigned int account_instance : 20;
		unsigned int account_type : 4;
		int universe : 8;
	};

	typedef union
	{
		raw_steam_id raw;
		unsigned long long bits;
	} steam_id;

	struct ISteamUser;

	enum { k_iSteamUserCallbacks = 100 };
	enum { k_iSteamGameServerCallbacks = 200 };
	enum { k_iSteamFriendsCallbacks = 300 };
	enum { k_iSteamBillingCallbacks = 400 };
	enum { k_iSteamMatchmakingCallbacks = 500 };
	enum { k_iSteamContentServerCallbacks = 600 };
	enum { k_iSteamUtilsCallbacks = 700 };
	enum { k_iSteamAppsCallbacks = 1000 };
	enum { k_iSteamUserStatsCallbacks = 1100 };
	enum { k_iSteamNetworkingCallbacks = 1200 };
	enum { k_iSteamNetworkingSocketsCallbacks = 1220 };
	enum { k_iSteamNetworkingMessagesCallbacks = 1250 };
	enum { k_iSteamNetworkingUtilsCallbacks = 1280 };
	enum { k_iSteamRemoteStorageCallbacks = 1300 };
	enum { k_iSteamGameServerItemsCallbacks = 1500 };
	enum { k_iSteamGameCoordinatorCallbacks = 1700 };
	enum { k_iSteamGameServerStatsCallbacks = 1800 };
	enum { k_iSteam2AsyncCallbacks = 1900 };
	enum { k_iSteamGameStatsCallbacks = 2000 };
	enum { k_iSteamHTTPCallbacks = 2100 };
	enum { k_iSteamScreenshotsCallbacks = 2300 };
	enum { k_iSteamStreamLauncherCallbacks = 2600 };
	enum { k_iSteamControllerCallbacks = 2800 };
	enum { k_iSteamUGCCallbacks = 3400 };
	enum { k_iSteamStreamClientCallbacks = 3500 };
	enum { k_iSteamMusicCallbacks = 4000 };
	enum { k_iSteamGameNotificationCallbacks = 4400 };
	enum { k_iSteamHTMLSurfaceCallbacks = 4500 };
	enum { k_iSteamVideoCallbacks = 4600 };
	enum { k_iSteamInventoryCallbacks = 4700 };
	enum { k_ISteamParentalSettingsCallbacks = 5000 };
	enum { k_iSteamGameSearchCallbacks = 5200 };
	enum { k_iSteamPartiesCallbacks = 5300 };
	enum { k_iSteamSTARCallbacks = 5500 };
	enum { k_iSteamRemotePlayCallbacks = 5700 };
	enum { k_iSteamChatCallbacks = 5900 };
	enum { k_iSteamTimelineCallbacks = 6000 };

	typedef unsigned __int64 SteamAPICall_t;
	typedef unsigned __int8 uint8;
	typedef unsigned __int16 uint16;
	typedef unsigned __int32 uint32;
	typedef unsigned __int64 uint64;

	struct LobbyDataUpdate_t
	{
		enum { k_iCallback = k_iSteamMatchmakingCallbacks + 5 };

		uint64 m_ulSteamIDLobby;		// steamID of the Lobby
		uint64 m_ulSteamIDMember;		// steamID of the member whose data changed, or the room itself
		uint8 m_bSuccess;				// true if we lobby data was successfully changed; 
		// will only be false if RequestLobbyData() was called on a lobby that no longer exists
	};

	struct LobbyMatchList_t
	{
		enum { k_iCallback = k_iSteamMatchmakingCallbacks + 10 };
		unsigned int num_lobbies;		// Number of lobbies that matched search criteria and we have SteamIDs for
	};

	class CCallbackBase
	{
	public:
		CCallbackBase() { m_nCallbackFlags = 0; m_iCallback = 0; }
		// don't add a virtual destructor because we export this binary interface across dll's
		virtual void Run(void* pvParam) = 0;
		virtual void Run(void* pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall) = 0;
		int GetICallback() { return m_iCallback; }
		virtual int GetCallbackSizeBytes() = 0;

	protected:
		enum { k_ECallbackFlagsRegistered = 0x01, k_ECallbackFlagsGameServer = 0x02 };
		uint8 m_nCallbackFlags;
		int m_iCallback;
		friend class CCallbackMgr;

	private:
		CCallbackBase(const CCallbackBase&);
		CCallbackBase& operator=(const CCallbackBase&);
	};

	struct LobbyEnter_t
	{
		enum { k_iCallback = k_iSteamMatchmakingCallbacks + 4 };

		steam_id lobby_id;
		int permissions;
		bool locked;
		int chatroom;
	};

	struct P2PSessionRequest_t
	{
		steam_id remote;
	};

	struct /*VFT*/ ISteamUser_vtbl
	{
		int(__fastcall* GetHSteamUser)(ISteamUser* this_);
		bool(__fastcall* LoggedOn)(ISteamUser* this_);
		__int64(__fastcall* GetSteamID)(ISteamUser* this_, steam_id* steamIDUser);
		int(__fastcall* InitiateGameConnection)(ISteamUser* this_, void* pAuthBlob, int cbMaxAuthBlob, steam_id steamIDGameServer, unsigned int unIPServer, unsigned __int16 usPortServer, bool bSecure);
		void(__fastcall* TerminateGameConnection)(ISteamUser* this_, unsigned int unIPServer, unsigned __int16 usPortServer);
		void(__fastcall* TrackAppUsageEvent)(ISteamUser* this_, steam_id gameID, int eAppUsageEvent, const char* pchExtraInfo);
		bool(__fastcall* GetUserDataFolder)(ISteamUser* this_, char* pchBuffer, int cubBuffer);
		void(__fastcall* StartVoiceRecording)(ISteamUser* this_);
		void(__fastcall* StopVoiceRecording)(ISteamUser* this_);
		int(__fastcall* GetAvailableVoice)(ISteamUser* this_, unsigned int* pcbCompressed, unsigned int* pcbUncompressed, unsigned int nUncompressedVoiceDesiredSampleRate);
		int(__fastcall* GetVoice)(ISteamUser* this_, bool bWantCompressed, void* pDestBuffer, unsigned int cbDestBufferSize, unsigned int* nBytesWritten, bool bWantUncompressed, void* pUncompressedDestBuffer, unsigned int cbUncompressedDestBufferSize, unsigned int* nUncompressBytesWritten, unsigned int nUncompressedVoiceDesiredSampleRate);
		int(__fastcall* DecompressVoice)(ISteamUser* this_, void* pCompressed, unsigned int cbCompressed, void* pDestBuffer, unsigned int cbDestBufferSize, unsigned int* nBytesWritten);
		unsigned int(__fastcall* GetVoiceOptimalSampleRate)(ISteamUser* this_);
		unsigned int(__fastcall* GetAuthSessionTicket)(ISteamUser* this_, void* pTicket, int cbMaxTicket, unsigned int* pcbTicket);
		int(__fastcall* BeginAuthSession)(ISteamUser* this_, const void* pAuthTicket, int cbAuthTicket, steam_id steamID);
		void(__fastcall* EndAuthSession)(ISteamUser* this_, steam_id steamID);
		void(__fastcall* CancelAuthTicket)(ISteamUser* this_, unsigned int hAuthTicket);
		unsigned int(__fastcall* UserHasLicenseForApp)(ISteamUser* this_, steam_id steamID, unsigned int appID);
		bool(__fastcall* BIsBehindNAT)(ISteamUser* this_);
		void(__fastcall* AdvertiseGame)(ISteamUser* this_, steam_id steamIDGameServer, unsigned int unIPServer, unsigned __int16 usPortServer);
		unsigned __int64(__fastcall* RequestEncryptedAppTicket)(ISteamUser* this_, void* pUserData, int cbUserData);
		bool(__fastcall* GetEncryptedAppTicket)(ISteamUser* this_, void* pTicket, int cbMaxTicket, unsigned int* pcbTicket);
	};

	struct ISteamUser
	{
		ISteamUser_vtbl* __vftable /*VFT*/;
	};

	struct ISteamNetworking;

	struct /*VFT*/ ISteamNetworking_vtbl
	{
		bool(__fastcall* SendP2PPacket)(ISteamNetworking* this_, steam_id steamIDRemote, const void* pubData, unsigned int cubData, int eP2PSendType, int nChannel);
		bool(__fastcall* IsP2PPacketAvailable)(ISteamNetworking* this_, unsigned int* pcubMsgSize, int nChannel);
		bool(__fastcall* ReadP2PPacket)(ISteamNetworking* this_, void* pubDest, unsigned int cubDest, unsigned int* pcubMsgSize, steam_id* psteamIDRemote, int nChannel);
		bool(__fastcall* AcceptP2PSessionWithUser)(ISteamNetworking* this_, steam_id steamIDRemote);
		bool(__fastcall* CloseP2PSessionWithUser)(ISteamNetworking* this_, steam_id steamIDRemote);
		bool(__fastcall* CloseP2PChannelWithUser)(ISteamNetworking* this_, steam_id steamIDRemote, int iVirtualPort);
		bool(__fastcall* GetP2PSessionState)(ISteamNetworking* this_, steam_id steamIDRemote, void* pConnectionState);
		bool(__fastcall* AllowP2PPacketRelay)(ISteamNetworking* this_, bool bAllow);
		unsigned int(__fastcall* CreateListenSocket)(ISteamNetworking* this_, int nVirtualP2PPort, unsigned int nIP, unsigned __int16 nPort, bool bAllowUseOfPacketRelay);
		unsigned int(__fastcall* CreateP2PConnectionSocket)(ISteamNetworking* this_, steam_id steamIDTarget, int nVirtualPort, int nTimeoutSec, bool bAllowUseOfPacketRelay);
		unsigned int(__fastcall* CreateConnectionSocket)(ISteamNetworking* this_, unsigned int nIP, unsigned __int16 nPort, int nTimeoutSec);
		bool(__fastcall* DestroySocket)(ISteamNetworking* this_, unsigned int hSocket, bool bNotifyRemoteEnd);
		bool(__fastcall* DestroyListenSocket)(ISteamNetworking* this_, unsigned int hSocket, bool bNotifyRemoteEnd);
		bool(__fastcall* SendDataOnSocket)(ISteamNetworking* this_, unsigned int hSocket, void* pubData, unsigned int cubData, bool bReliable);
		bool(__fastcall* IsDataAvailableOnSocket)(ISteamNetworking* this_, unsigned int hSocket, unsigned int* pcubMsgSize);
		bool(__fastcall* RetrieveDataFromSocket)(ISteamNetworking* this_, unsigned int hSocket, void* pubDest, unsigned int cubDest, unsigned int* pcubMsgSize);
		bool(__fastcall* IsDataAvailable)(ISteamNetworking* this_, unsigned int hListenSocket, unsigned int* pcubMsgSize, unsigned int* phSocket);
		bool(__fastcall* RetrieveData)(ISteamNetworking* this_, unsigned int hListenSocket, void* pubDest, unsigned int cubDest, unsigned int* pcubMsgSize, unsigned int* phSocket);
		bool(__fastcall* GetSocketInfo)(ISteamNetworking* this_, unsigned int hSocket, steam_id* pSteamIDRemote, int* peSocketStatus, unsigned int* punIPRemote, unsigned __int16* punPortRemote);
		bool(__fastcall* GetListenSocketInfo)(ISteamNetworking* this_, unsigned int hListenSocket, unsigned int* pnIP, unsigned __int16* pnPort);
		int(__fastcall* GetSocketConnectionType)(ISteamNetworking* this_, unsigned int hSocket);
		int(__fastcall* GetMaxPacketSize)(ISteamNetworking* this_, unsigned int hSocket);
	};

	struct ISteamNetworking
	{
		ISteamNetworking_vtbl* __vftable /*VFT*/;
	};

	struct ISteamFriends;

	typedef std::int16_t FriendsGroupID_t;

	struct /*VFT*/ ISteamFriends_vtbl
	{
		const char* (__fastcall* GetPersonaName)(ISteamFriends* this_);
		unsigned __int64(__fastcall* SetPersonaName)(ISteamFriends* this_, const char* pchPersonaName);
		int(__fastcall* GetPersonaState)(ISteamFriends* this_);
		int(__fastcall* GetFriendCount)(ISteamFriends* this_, int eFriendFlags);
		void(__fastcall* GetFriendByIndex)(ISteamFriends* this_, steam_id* out, int iFriend, int iFriendFlags);
		int(__fastcall* GetFriendRelationship)(ISteamFriends* this_, steam_id steamIDFriend);
		int(__fastcall* GetFriendPersonaState)(ISteamFriends* this_, steam_id steamIDFriend);
		const char* (__fastcall* GetFriendPersonaName)(ISteamFriends* this_, steam_id steamIDFriend);
		bool(__fastcall* GetFriendGamePlayed)(ISteamFriends* this_, steam_id steamIDFriend, void* pFriendGameInfo);
		const char* (__fastcall* GetFriendPersonaNameHistory)(ISteamFriends* this_, steam_id steamIDFriend, int iPersonaName);
		int(__fastcall* GetFriendSteamLevel)(ISteamFriends* this_, steam_id steamIDFriend);
		const char* (__fastcall* GetPlayerNickname)(ISteamFriends* this_, steam_id steamIDFriend);
		int(__fastcall* GetFriendsGroupCount)(ISteamFriends* this_);
		FriendsGroupID_t(__fastcall* GetFriendsGroupIDByIndex)(ISteamFriends* this_, int iFG);
		const char* (__fastcall* GetFriendsGroupName)(ISteamFriends* this_, FriendsGroupID_t friendsGroupID);
		int(__fastcall* GetFriendsGroupMembersCount)(ISteamFriends* this_, FriendsGroupID_t friendsGroupID);
		void(__fastcall* GetFriendsGroupMembersList)(ISteamFriends* this_, FriendsGroupID_t friendsGroupID, steam_id* pOutSteamIDMembers, int nMembersCount);
		bool(__fastcall* HasFriend)(ISteamFriends* this_, steam_id steamIDFriend, int eFriendFlags);
		int(__fastcall* GetClanCount)(ISteamFriends* this_);
		steam_id(__fastcall* GetClanByIndex)(ISteamFriends* this_, int iClan);
		const char* (__fastcall* GetClanName)(ISteamFriends* this_, steam_id steamIDClan);
		const char* (__fastcall* GetClanTag)(ISteamFriends* this_, steam_id steamIDClan);
		bool(__fastcall* GetClanActivityCounts)(ISteamFriends* this_, steam_id steamID, int* pnOnline, int* pnInGame, int* pnChatting);
		unsigned __int64(__fastcall* DownloadClanActivityCounts)(ISteamFriends* this_, steam_id groupIDs[], int nIds);
		int(__fastcall* GetFriendCountFromSource)(ISteamFriends* this_, steam_id steamIDSource);
		steam_id(__fastcall* GetFriendFromSourceByIndex)(ISteamFriends* this_, steam_id steamIDSource, int iFriend);
		bool(__fastcall* IsUserInSource)(ISteamFriends* this_, steam_id steamIDUser, steam_id steamIDSource);
		void(__fastcall* SetInGameVoiceSpeaking)(ISteamFriends* this_, steam_id steamIDUser, bool bSpeaking);
		void(__fastcall* ActivateGameOverlay)(ISteamFriends* this_, const char* pchDialog);
		void(__fastcall* ActivateGameOverlayToUser)(ISteamFriends* this_, const char* pchDialog, steam_id steamID);
		void(__fastcall* ActivateGameOverlayToWebPage)(ISteamFriends* this_, const char* pchURL);
		void(__fastcall* ActivateGameOverlayToStore)(ISteamFriends* this_, unsigned int nAppID, unsigned int eFlag);
		void(__fastcall* SetPlayedWith)(ISteamFriends* this_, steam_id steamIDUserPlayedWith);
		void(__fastcall* ActivateGameOverlayInviteDialog)(ISteamFriends* this_, steam_id steamIDLobby);
		int(__fastcall* GetSmallFriendAvatar)(ISteamFriends* this_, steam_id steamIDFriend);
		int(__fastcall* GetMediumFriendAvatar)(ISteamFriends* this_, steam_id steamIDFriend);
		int(__fastcall* GetLargeFriendAvatar)(ISteamFriends* this_, steam_id steamIDFriend);
		bool(__fastcall* RequestUserInformation)(ISteamFriends* this_, steam_id steamIDUser, bool bRequireNameOnly);
		unsigned __int64(__fastcall* RequestClanOfficerList)(ISteamFriends* this_, steam_id steamIDClan);
		steam_id(__fastcall* GetClanOwner)(ISteamFriends* this_, steam_id steamIDClan);
		int(__fastcall* GetClanOfficerCount)(ISteamFriends* this_, steam_id steamIDClan);
		steam_id(__fastcall* GetClanOfficerByIndex)(ISteamFriends* this_, steam_id steamIDClan, int iOfficer);
		int(__fastcall* GetUserRestrictions)(ISteamFriends* this_);
		bool(__fastcall* SetRichPresence)(ISteamFriends* this_, const char* pchKey, const char* pchValue);
		void(__fastcall* ClearRichPresence)(ISteamFriends* this_);
		const char* (__fastcall* GetFriendRichPresence)(ISteamFriends* this_, steam_id steamIDFriend, const char* pchKey);
		int(__fastcall* GetFriendRichPresenceKeyCount)(ISteamFriends* this_, steam_id steamIDFriend);
		const char* (__fastcall* GetFriendRichPresenceKeyByIndex)(ISteamFriends* this_, steam_id steamIDFriend, int iKey);
		void(__fastcall* RequestFriendRichPresence)(ISteamFriends* this_, steam_id steamIDFriend);
		bool(__fastcall* InviteUserToGame)(ISteamFriends* this_, steam_id steamIDFriend, const char* pchConnectString);
		int(__fastcall* GetCoplayFriendCount)(ISteamFriends* this_);
		steam_id(__fastcall* GetCoplayFriend)(ISteamFriends* this_, int iCoplayFriend);
		int(__fastcall* GetFriendCoplayTime)(ISteamFriends* this_, steam_id steamIDFriend);
		unsigned int(__fastcall* GetFriendCoplayGame)(ISteamFriends* this_, steam_id steamIDFriend);
		unsigned __int64(__fastcall* JoinClanChatRoom)(ISteamFriends* this_, steam_id steamIDClan);
		bool(__fastcall* LeaveClanChatRoom)(ISteamFriends* this_, steam_id steamIDClan);
		int(__fastcall* GetClanChatMemberCount)(ISteamFriends* this_, steam_id steamIDClan);
		steam_id(__fastcall* GetChatMemberByIndex)(ISteamFriends* this_, steam_id steamIDClan, int iUser);
		bool(__fastcall* SendClanChatMessage)(ISteamFriends* this_, steam_id steamIDClanChat, const char* pchText);
		int(__fastcall* GetClanChatMessage)(ISteamFriends* this_, steam_id steamIDClanChat, int iMessage, void* prgchText, int cchTextMax, unsigned int* peChatEntryType, steam_id* pSteamIDChatter);
		bool(__fastcall* IsClanChatAdmin)(ISteamFriends* this_, steam_id steamIDClanChat, steam_id steamIDUser);
		bool(__fastcall* IsClanChatWindowOpenInSteam)(ISteamFriends* this_, steam_id steamIDClanChat);
		bool(__fastcall* OpenClanChatWindowInSteam)(ISteamFriends* this_, steam_id steamIDClanChat);
		bool(__fastcall* CloseClanChatWindowInSteam)(ISteamFriends* this_, steam_id steamIDClanChat);
		bool(__fastcall* SetListenForFriendsMessages)(ISteamFriends* this_, bool bInterceptEnabled);
		bool(__fastcall* ReplyToFriendMessage)(ISteamFriends* this_, steam_id steamIDFriend, const char* pchMsgToSend);
		int(__fastcall* GetFriendMessage)(ISteamFriends* this_, steam_id steamIDFriend, int iMessageID, void* pvData, int cubData, unsigned int* peChatEntryType);
		unsigned __int64(__fastcall* GetFollowerCount)(ISteamFriends* this_, steam_id steamID);
		unsigned __int64(__fastcall* IsFollowing)(ISteamFriends* this_, steam_id steamID);
		unsigned __int64(__fastcall* EnumerateFollowingList)(ISteamFriends* this_, unsigned int unStartIndex);
	};

	struct ISteamFriends
	{
		ISteamFriends_vtbl* __vftable /*VFT*/;
	};

	struct LobbyCreated_t
	{
		enum { k_iCallback = k_iSteamMatchmakingCallbacks + 13 };
		int result;
		steam_id lobby_id;
	};

	struct LobbyChatMsg_t
	{
		steam_id lobby_id;
		steam_id user_id;
		std::uint8_t chat_entry_type;
		std::uint32_t chat_id;
	};

	static_assert(offsetof(LobbyChatMsg_t, chat_id) == 20);

	struct ISteamMatchmaking;

	struct /*VFT*/ ISteamMatchmaking_vtbl
	{
		int(__fastcall* GetFavoriteGameCount)(ISteamMatchmaking* this_);
		bool(__fastcall* GetFavoriteGame)(ISteamMatchmaking* this_, int iGame, unsigned int* pnAppID, unsigned int* pnIP, unsigned __int16* pnConnPort, unsigned __int16* pnQueryPort, unsigned int* punFlags, unsigned int* pRTime32LastPlayedOnServer);
		int(__fastcall* AddFavoriteGame)(ISteamMatchmaking* this_, unsigned int nAppID, unsigned int nIP, unsigned __int16 nConnPort, unsigned __int16 nQueryPort, unsigned int unFlags, unsigned int rTime32LastPlayedOnServer);
		bool(__fastcall* RemoveFavoriteGame)(ISteamMatchmaking* this_, unsigned int nAppID, unsigned int nIP, unsigned __int16 nConnPort, unsigned __int16 nQueryPort, unsigned int unFlags);
		unsigned __int64(__fastcall* RequestLobbyList)(ISteamMatchmaking* this_);
		void(__fastcall* AddRequestLobbyListStringFilter)(ISteamMatchmaking* this_, const char* pchKeyToMatch, const char* pchValueToMatch, int eComparisonType);
		void(__fastcall* AddRequestLobbyListNumericalFilter)(ISteamMatchmaking* this_, const char* pchKeyToMatch, int nValueToMatch, int eComparisonType);
		void(__fastcall* AddRequestLobbyListNearValueFilter)(ISteamMatchmaking* this_, const char* pchKeyToMatch, int nValueToBeCloseTo);
		void(__fastcall* AddRequestLobbyListFilterSlotsAvailable)(ISteamMatchmaking* this_, int nSlotsAvailable);
		void(__fastcall* AddRequestLobbyListDistanceFilter)(ISteamMatchmaking* this_, int eLobbyDistanceFilter);
		void(__fastcall* AddRequestLobbyListResultCountFilter)(ISteamMatchmaking* this_, int cMaxResults);
		void(__fastcall* AddRequestLobbyListCompatibleMembersFilter)(ISteamMatchmaking* this_, steam_id steamID);
		steam_id*(__fastcall* GetLobbyByIndex)(ISteamMatchmaking* this_, steam_id* lobby_id, int iLobby);
		unsigned __int64(__fastcall* CreateLobby)(ISteamMatchmaking* this_, int eLobbyType, int cMaxMembers);
		unsigned __int64(__fastcall* JoinLobby)(ISteamMatchmaking* this_, steam_id steamIDLobby);
		void(__fastcall* LeaveLobby)(ISteamMatchmaking* this_, steam_id steamIDLobby);
		bool(__fastcall* InviteUserToLobby)(ISteamMatchmaking* this_, steam_id steamIDLobby, steam_id steamIDInvitee);
		int(__fastcall* GetNumLobbyMembers)(ISteamMatchmaking* this_, steam_id steamIDLobby);
		void(__fastcall* GetLobbyMemberByIndex)(ISteamMatchmaking* this_, steam_id* steamIDUser, steam_id steamIDLobby, int iMember);
		const char* (__fastcall* GetLobbyData)(ISteamMatchmaking* this_, steam_id steamIDLobby, const char* pchKey);
		bool(__fastcall* SetLobbyData)(ISteamMatchmaking* this_, steam_id steamIDLobby, const char* pchKey, const char* pchValue);
		int(__fastcall* GetLobbyDataCount)(ISteamMatchmaking* this_, steam_id steamIDLobby);
		bool(__fastcall* GetLobbyDataByIndex)(ISteamMatchmaking* this_, steam_id steamIDLobby, int iLobbyData, char* pchKey, int cchKeyBufferSize, char* pchValue, int cchValueBufferSize);
		bool(__fastcall* DeleteLobbyData)(ISteamMatchmaking* this_, steam_id steamIDLobby, const char* pchKey);
		const char* (__fastcall* GetLobbyMemberData)(ISteamMatchmaking* this_, steam_id steamIDLobby, steam_id steamIDUser, const char* pchKey);
		void(__fastcall* SetLobbyMemberData)(ISteamMatchmaking* this_, steam_id steamIDLobby, const char* pchKey, const char* pchValue);
		bool(__fastcall* SendLobbyChatMsg)(ISteamMatchmaking* this_, steam_id steamIDLobby, const void* pvMsgBody, int cubMsgBody);
		int(__fastcall* GetLobbyChatEntry)(ISteamMatchmaking* this_, steam_id steamIDLobby, int iChatID, steam_id* pSteamIDUser, void* pvData, int cubData, int* peChatEntryType);
		bool(__fastcall* RequestLobbyData)(ISteamMatchmaking* this_, steam_id steamIDLobby);
		void(__fastcall* SetLobbyGameServer)(ISteamMatchmaking* this_, steam_id steamIDLobby, unsigned int unGameServerIP, unsigned __int16 unGameServerPort, steam_id steamIDGameServer);
		bool(__fastcall* GetLobbyGameServer)(ISteamMatchmaking* this_, steam_id steamIDLobby, unsigned int* punGameServerIP, unsigned __int16* punGameServerPort, steam_id* psteamIDGameServer);
		bool(__fastcall* SetLobbyMemberLimit)(ISteamMatchmaking* this_, steam_id steamIDLobby, int cMaxMembers);
		int(__fastcall* GetLobbyMemberLimit)(ISteamMatchmaking* this_, steam_id steamIDLobby);
		bool(__fastcall* SetLobbyType)(ISteamMatchmaking* this_, steam_id steamIDLobby, int eLobbyType);
		bool(__fastcall* SetLobbyJoinable)(ISteamMatchmaking* this_, steam_id steamIDLobby, bool bLobbyJoinable);
		steam_id*(__fastcall* GetLobbyOwner)(ISteamMatchmaking* this_, steam_id* owner, steam_id steamIDLobby);
		bool(__fastcall* SetLobbyOwner)(ISteamMatchmaking* this_, steam_id steamIDLobby, steam_id steamIDNewOwner);
		bool(__fastcall* SetLinkedLobby)(ISteamMatchmaking* this_, steam_id steamIDLobby, steam_id steamIDLobby2);
	};

	struct ISteamMatchmaking
	{
		ISteamMatchmaking_vtbl* __vftable /*VFT*/;
	};

	namespace Json
	{
		struct Value
		{
			union u_t
			{
				int integer;
				double value;
				char byte;
				void* ptr;
			};

			u_t u;
			char type;
			char __pad0[8];
		};
	}

	namespace lua
	{
		struct lua_State
		{

		};
	}

	namespace gn
	{
		struct swapchain
		{
			char __pad0[24];
			IDXGISwapChain* swapChain;
		};

		struct CommandList
		{
			char __pad0[4];
			unsigned int resourceId;
			void* unk;
			volatile __int32 lock;
		};

		namespace internal
		{
			namespace dx11
			{
				struct DeviceContext
				{
					char __pad0[4];
					unsigned int resourceId;
					ID3D11DeviceContext* deviceContext;
					ID3DUserDefinedAnnotation* annotation;
					gn::CommandList* commandList;
				};

				struct resource
				{
					volatile __int32 refCount;
					unsigned int resourceId;
					unsigned int a2;
					ID3D11Resource* resource;
				};
			}
		}

		namespace dx11
		{
			struct buffer
			{
				char __pad0[4];
				unsigned int resourceId;
				unsigned int byteWidth;
				unsigned int bindFlags;
				ID3D11Buffer* buffer;
			};
		}

		struct DeviceContext : gn::internal::dx11::DeviceContext
		{

		};

		struct Resource : gn::internal::dx11::resource
		{

		};

		struct Buffer : gn::dx11::buffer
		{
			char* data;
			unsigned int size;
		};
	}

	namespace Vectormath::Aos
	{
		struct Vector2
		{
			union
			{
				float values[2];
			};
		};

		struct Vector3
		{
			union
			{
				float values[4];
				__m128 value;
			};
		};

		struct Vector4
		{
			union
			{
				float values[4];
				__m128 value;
			};
		};

		struct Quat
		{
			float values[4];
		};
	}

	namespace fox
	{
		struct Rgba8;
		struct Path;

		struct SharedStringData
		{
			const char* buffer;
		};

		struct SharedString
		{
			SharedStringData* data;
		};

		struct String : SharedString
		{

		};

		struct pfimpl
		{
			__int32 refCount;
			__int32 pad;
		};

		struct SharedObject
		{
			struct vtable
			{
				void(__fastcall* __destructor)(SharedObject*);
				void(__fastcall* Release)(SharedObject*);
			};
			vtable* __vftable;
			pfimpl pf;
		};

		struct Mutex
		{
			char __pad0[16];
		};

		struct Quark
		{
			struct vtable
			{
				void(__fastcall* __destructor)(fox::Quark*);
				void(__fastcall* Initialize)(fox::Quark*);
				void(__fastcall* Terminate)(fox::Quark*);
				void(__fastcall* GetInterface)(fox::Quark*);
				void(__fastcall* CancelPermit)(fox::Quark*);
				void(__fastcall* GetCreator)(fox::Quark*);
			};

			vtable* __vftable;
			void* a1;
			void* a3;
			void* a4;
		};

		union Color
		{
			float values[4];
			__m128 value;
		};

		struct Buffer
		{
			char* data;
			char* a2;
			size_t offset;
			size_t size;
			size_t capacity;
			int flags;
		};

		static_assert(offsetof(Buffer, data) == 0);
		static_assert(offsetof(Buffer, size) == 24);
		static_assert(offsetof(Buffer, capacity) == 32);
		static_assert(offsetof(Buffer, flags) == 40);
		static_assert(sizeof(Buffer) == 0x30);

		struct StringId_fields
		{
			std::uint32_t l;
			std::uint32_t h;
		};

		struct StringId
		{
			union
			{
				StringId_fields f;
				std::uint64_t id;
			};
		};

		struct Path
		{
			__int64 id;
		};

		struct KernelString
		{
			const char* buffer;
			__int64 unk1;
			__int64 unk2;
			__int64 unk3;
		};

		struct PathId
		{
			std::uint64_t id;
		};

		typedef std::uint32_t KeyCode;

		template <typename T>
		struct EntityPtrBase
		{
			T* ptr;
		};

		struct EntityHandle
		{
			void* a1;
		};

		template <typename T>
		struct Array
		{
			std::uint32_t size;
			std::uint32_t capacity;
			T** array;
		};

		struct Entity
		{

		};

		struct RawKeyboardData
		{
			__int16 makeCode;
			char __pad0[2];
			int key;
			__int16 keyAscii;
			__int16 keyUnicode;
			bool isUp;
			bool unk;
		};

#pragma pack(push, 1)
		struct RawMouseData
		{
			int buttonStates;
			int buttonsDown;
			int buttonsUp;
			__int16 pointX;
			__int16 pointY;
			__int16 deltaX;
			__int16 deltaY;
			__int16 lastX;
			__int16 lastY;
			int buttonData;
		};
#pragma pack(pop)

		struct LuaBase
		{
			struct vftable
			{
				void(__fastcall* __destructor)();
			};

			vftable* __vftable;
			lua::lua_State* state;
			char initialized;
		};

		struct Lua : LuaBase
		{
			struct vftable
			{
				void(__fastcall* __destructor)();
			};

			void* a2;
			void* queue;
		};

		struct LuaAutoPtr
		{
			fox::Lua* lua;
			int top;
			int a2;
		};

		struct LuaScopedGlobalPolicy
		{
			char __pad0[16];
		};

		struct Script
		{

		};

		struct HashMapBase_unk1
		{
			void** ptr_array1;
			void* ptr1;
		};

		struct HashMapBase
		{
			struct vtable
			{

			};
			vtable* __vftable;
			HashMapBase_unk1* unk1;
			HashMapBase_unk1* unk2;
			int a1;
			short a2_1;
			short a2_2;
			void* end;
			void* ptr6;
			void* unk3;
			void* unk4;
			int unk5_1;
			int unk5_2;
			void* unk6;
		};

		namespace fs
		{
			struct MountPoint
			{

			};
		}

		namespace gk
		{
#pragma pack(push, 1)
			struct FirstPartyAccount
			{
				unsigned __int64 id;
				char name[129];
			};
#pragma pack(pop)
		}

		namespace gr
		{
			struct Scene;

			struct SceneObject;
			struct SceneObject
			{
				struct vtable
				{
					void* f[64];
				};
				vtable* __vftable;
				char __pad0[8];
				SceneObject* obj1;
				SceneObject* obj2;
				__int64 flags1;
				int priority;
				char __pad2[4];
				Scene* scene;
				short unk;
				short flags2;
				char __pad3[64];
			};

			struct SceneObjectList
			{
				SceneObject* obj1;
				SceneObject* obj2;
				int count;
			};

			struct Scene
			{
				Scene* next;
				int index;
				int flags;
				char __pad1[16];
				SharedString id;
				char __pad2[8];
				SceneObjectList** objectList1;
				SceneObjectList* objectList2;
				char __pad3[16];
				void* lock;
			};

			struct TonemapDesc
			{
				float minExposure;
				float maxExposure;
				float exposureCompensation;
				float bloomSize;
				float bloomBrightnessExtraction;
				float bloomWeight;
			};

			struct Camera
			{
				struct AdaptiveAddExpCompParameter
				{
					float param1;
					float param2;
					float param3;
					float param4;
					float param5;
					float param6;
					float param7;
					float param8;
					float param9;
					float param10;
				};

				struct vtable
				{

				};

				vtable* __vftable;
				float f38;
				float f39;
				float f40;
				float f41;
				float f42;
				float f43;
				float f44;
				float f45;
				TonemapDesc tonemapDesc;
				float m2[4];
				float m3[4];
				int view[4];
				float m5[4];
				float m6[4];
				float m7[4];
				float m8[4];
				float m9[4];
				float m10[4];
				float m11[4];
				float m12[4];
				float m13[4];
				unsigned int flags;
				float f29;
				float f30;
				float f31;
				float viewAngleFactor;
				float viewAngle;
				__int64 unk3;
				float f32;
				float f33;
				float f34;
				float f35;
				double d1;
				double d2;
				float f36;
				float f37;
				float fArr1[2];
				float fArr2[2];
				unsigned a1;
				unsigned a2;
				__int64 unk2;
				float f1;
				float f2;
				float f3;
				float f4;
				float f5;
				float f6;
				float f7;
				float f8;
				float f9;
				float f10;
				float f11;
				float f12;
				float f13;
				float f14;
				float f15;
				float f16;
				float f17;
				float f18;
				float f19;
				float f20;
				AdaptiveAddExpCompParameter adaptiveAddExpCompParameter;
			};

			struct Packet2DBuffer;
			struct Draw2D
			{
				struct vtable
				{

				};

				vtable* __vftable;
				char __pad0[16];
				Draw2D* next;
				char __pad1[8];
				int priority;
				char __pad2[20];
				Packet2DBuffer* packetBuffer;
				Camera* camera;
				int flags;
				char __pad3[8];
			};

			struct Viewport;
			struct Viewport
			{
				struct vtable
				{

				};

				vtable* __vftable;
				char __pad0[40];
				Viewport* next;
				char __pad1[1336];
				Camera camera;
			};

			struct Render;
			struct Render
			{
				struct vtable
				{

				};
				vtable* __vftable;
				Render* next;
				char __pad1[4];
				unsigned int flags;
				char __pad0[40];
				Mutex mutex;
				char __pad4[64];
				SharedString name;
				char __pad2[8];
				Viewport* viewportList;
				char __pad3[56];
			};

			struct RenderWork
			{
				char __pad0[32];
				Render* renderList;
				Render* defaultRender;
				char __pad1[32];
			};


			struct ResourceId
			{
				union
				{
					struct
					{
						unsigned int type : 7;
						unsigned int unk2 : 10;
						unsigned int index : 15;
					} fields;
					unsigned int data;
				};
			};

			struct Texture
			{
				char __pad0[24];
				ResourceId id;
			};

			struct RenderParameter
			{
				void* unk1;
				void* unk2;
				void* unk3;
				void* unk4;
				void* unk5;
				unsigned int a1;
				unsigned int a2;
				unsigned int a3;
				unsigned int a4;
				unsigned int a5;
				unsigned int a6;
				unsigned int a7;
				unsigned int a8;
				void* unk6;
				void* unk7;
				void* unk8;
				unsigned int a9;
				char a10;
				char a11;
				char a12;
				char a13;
				char a14;
				char a15;
			};

			namespace dg
			{
				struct StringFontMetricsCache
				{
					char a1;
					char a2;
					__int16 count;
					int stringWidth;
					__int16* glyphs;
				};

				struct _TextureGlyphData
				{
					unsigned short character;
					unsigned short a1_2;
					unsigned short a2_1;
					unsigned short a2_2;
					unsigned char a3_1;
					unsigned char a3_2;
					unsigned char a3_3;
					unsigned char horizontalSpace;
					unsigned char width;
					unsigned char height;
					char horizontalShift;
					char verticalShift;
					float a5;
					float a6;
					float a7;
					float a8;
				};

				struct _TextureAreaInfo
				{
					int a1;
					short width;
					short height;
					short a3_1;
					short a3_2;
					short a4_1;
					short a4_2;
				};

				struct ImageGlyphManager
				{
					struct vtable
					{
						void(__fastcall* __destructor)(ImageGlyphManager*, char);
					};

					vtable* __vftable;
					_TextureGlyphData* imageGlyphs;
					HashMapBase hashMap;
					int numGlyphs;
					int numUnk;
					char __pad2[256];
				};

				struct ImageAreaManager
				{
					struct vtable
					{
						void(__fastcall* __destructor)(ImageAreaManager*, char);
					};

					vtable* __vftable;
					_TextureAreaInfo areas[2048];
					char states[2048];
					int count;
					int a2;
					HashMapBase hashMap;
				};

				struct FontTextureRender
				{
					struct vtable
					{
						void(__fastcall* __destructor)(FontTextureRender*);
					};

					vtable* __vftable;
					unsigned int pixel_width;
					unsigned int pixel_height;
					unsigned int fontTextureHandle;
					char __pad1[4];
					void* unk1;
					char __pad2[16];
					void* unk2;
					char __pad3[16];
					ImageGlyphManager* imageGlyphManager;
					ImageAreaManager* imageAreaManager;
					char __pad4[1072];
				};

				struct FontSystem_unk2
				{
					char __pad0[10];
					char c1;
					char __pad1[53];
					__int16 s1;
					char c2;
				};

				struct MemoryManager_unk1
				{
					size_t unk1;
					void* unk2;
					size_t unk3;
					size_t unk4;
				};

				struct MemoryManager
				{
					MemoryManager_unk1* unk1;
					size_t size;
				};

				struct FontData_Glyph
				{
					unsigned int character;
					unsigned short xOffset;
					unsigned short yOffset;
					unsigned char width;
					unsigned char height;
					unsigned char layer;
					unsigned char horizontalSpace;
					unsigned char horizontalShift;
					char verticalShift;
					unsigned short unk1;
					unsigned int unk2;
				};

				struct FontData
				{
					struct vtable
					{

					};

					vtable* __vftable;
					unsigned char unk1_1;
					unsigned char unk1_2;
					unsigned char unk1_3;
					unsigned char unk1_4;
					short unk2;
					short glyphCount;
					int glyphDataSize;
					unsigned char spacing;
					unsigned char unk3_2;
					unsigned char unk3_3;
					unsigned char unk3_4;
					FontData_Glyph* glyphs;
					char unk4;
					char widthExp;
					char heightExp;
					char unk5;
					int fontDataSize;
					void* fontDataBuffer;
					MemoryManager* memoryManager;
					float width;
					float height;
					short defaultCharacter;
					unsigned char fontIndex;
					unsigned char unk7;
				};

				struct FontSystem
				{
					/*
					0: default
					1: FontSystem_LatinFont
					2: FontSystem_KanjiFont
					3: FontSystem_Slot3
					4: FontSystem_Slot4
					5: FontSystem_Slot5
					6: FontSystem_Slot6
					7: FontSystem_Slot7
					8: 
					*/

					char __pad0[8];
					FontData* fonts[8];
					MemoryManager* memoryManager;
					FontTextureRender* fontTextureRender;
					FontSystem_unk2* unk2;
				};

				struct DynamicVertexBuffer_unk1
				{
					Array<gn::Buffer> bufferArray;
					unsigned int a2;
					unsigned int count;
				};

				struct DynamicVertexBuffer
				{
					struct Cluster
					{
						unsigned int count;
						gn::Resource** resources;
						char __pad0[8];
					};

					char __pad0[16];
					Cluster clusters[2];
					DynamicVertexBuffer_unk1* unk1;
					unsigned int count;
					unsigned int a4;
					gn::Buffer* buffer;
					Array<void*> unkArray;
					void* ptr1;
					unsigned int a1;
					unsigned int a2;
				};
				
				struct DynamicIndexBuffer
				{
					char __pad0[0x78];
				};

				struct SubCommandContext
				{

				};

				struct BuildDraw2DParameters
				{
					char __pad0[8];
					Draw2D** drawList;
					SubCommandContext* commandContext;
					DynamicVertexBuffer* vertexBuffer;
					void* ptr2;
					void* ptr3;
					Camera* camera;
					char __pad1[4];
					float f1;
					int viewX;
					int viewY;
					int viewWidth;
					int viewHeight;
					char __pad2[8];
					void* ptr4;
				};

				struct FontTextureMetrics
				{
					float f1;
					float f2;
					float f3;
					float f4;
					float f5;
					float f6;
					float f7;
					float f8;
					float f9;
				};

				struct DgTextureStreamer
				{

				};

#pragma pack(push, 1)
				struct ShadowConstantRegister
				{
					struct Buffer
					{
						struct vector
						{
							float vec[4];
						};

						gn::Buffer* bufferInstance;
						vector* vectors;
						void* unk2;
						int count;
						int a2;
					};

					Buffer buffers[8];
					void* data;
					unsigned int bufferCount;
					unsigned int vectorCount;
					unsigned int flags;
					unsigned int unkCount;
					unsigned int a1;
					void* unk1;
					unsigned int a2;
				};
#pragma pack(pop)

				struct CommandListNode;
				struct CommandListNode
				{
					fox::gr::dg::CommandListNode* node1;
					fox::gr::dg::CommandListNode* node2;
					gn::CommandList* commandList;
					unsigned int flags;
				};

				struct MaterialResource_unk1
				{
					union
					{
						struct
						{
							unsigned char textureCount1 : 4;
							unsigned char textureCount2 : 4;
						} fields;
						unsigned char data;
					};
				};

				struct MaterialResource
				{
					char __pad0[32];
					fox::gr::ResourceId shader;
					unsigned char numConsts;
					fox::gr::dg::MaterialResource_unk1 unkFields;
					char __pad2[10];
					float consts[8][4];
					fox::gr::ResourceId textures[8];
				};

				struct MaterialManager
				{
					struct vtable
					{

					};

					vtable* __vftable;
					fox::gr::dg::MaterialResource* materials;
				};

				struct ShaderTechnique
				{
					char __pad0[80];
				};

				struct ShaderTechniqueManager
				{
					struct vtable
					{

					};

					vtable* __vftable;
					ShaderTechnique* shaderTechniques;
				};

				struct RenderResourceContext
				{
					char __pad0[0x398];
				};

				struct MacroPacketBuffer
				{
					char __pad0[0x80];
				};

				struct RenderStateContext
				{
					unsigned char flags1[8];
					short w1;
					short w2;
					short w3;
					short w4;
					unsigned int flags4;
					char __pad0[4];
					void* unk1;
					void* unk2;
					float f1;
					char __pad1[20];
					float f2;
					float f3;
					float f4;
					float f5;
					unsigned int a1;
					char __pad2[36];
					unsigned int flags2;
					unsigned int flags3;
					unsigned int a5;
				};

				static_assert(offsetof(RenderStateContext, flags2) == 0x78);

				struct CommandBuffer
				{
					char __pad0[8];
					gn::DeviceContext* deviceContext;
					char flags;
					char __pad1[7];
					fox::gr::dg::CommandListNode* commandList;
					char __pad2[8];
					fox::gr::dg::ShadowConstantRegister* shadowConstantRegister;
					fox::gr::dg::RenderStateContext* renderStateContext;
					char __pad3[8];
					fox::gr::ResourceId material;
					char __pad4[556];
				};

				static_assert(offsetof(CommandBuffer, shadowConstantRegister) == 40);
				static_assert(offsetof(CommandBuffer, material) == 64);

				struct DgDx11;
				struct DgDx11
				{
					struct vtable
					{
						void(__fastcall* meth_001)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndInstance)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetupShaderResources)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_004)(fox::gr::dg::DgDx11*);
						void(__fastcall* Wait)(fox::gr::dg::DgDx11*);
						void(__fastcall* Kick)(fox::gr::dg::DgDx11*);
						void(__fastcall* Start)(fox::gr::dg::DgDx11*);
						void(__fastcall* End)(fox::gr::dg::DgDx11*);
						void(__fastcall* SkipRendering)(fox::gr::dg::DgDx11*);
						void(__fastcall* DestroyRenderInfo)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_011)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndRender)(fox::gr::dg::DgDx11*);
						void(__fastcall* InitViewport)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndViewport)(fox::gr::dg::DgDx11*);
						void(__fastcall* RestoreViewport)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetViewportDirect)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetGamma)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_018)(fox::gr::dg::DgDx11*);
						void(__fastcall* ValidateVideoOutSettings)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetVideoOutSettings)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_021)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_022)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateRenderTarget)(fox::gr::dg::DgDx11*);
						void(__fastcall* RestoreRenderTarget)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateRenderBuffer)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetViewportScissor)(fox::gr::dg::DgDx11*);
						void(__fastcall* BeginRenderBuffer)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndRenderBuffer)(fox::gr::dg::DgDx11*);
						void(__fastcall* GetCurrentRenderBuffer)(fox::gr::dg::DgDx11*);
						void(__fastcall* ResolveRenderTexture)(fox::gr::dg::DgDx11*);
						void(__fastcall* GetPrimaryDepthTarget)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_032)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_033)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_034)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetShader)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetMatrix)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetVector)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetVectors)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetSkinningParam)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateShaderFileResource)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_041)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetShaderObject)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateInputLayoutResource)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetInputLayout)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_045)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateRenderStateResource)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_047)(fox::gr::dg::DgDx11*);
						void(__fastcall* UpdateStatus)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetupRenderStateByFrame)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateVertexBufferResource)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateIndexBufferResource)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetVertexBuffer)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetIndexBuffer)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateVertexStreamResource)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetVertexStream)(fox::gr::dg::DgDx11*);
						void(__fastcall* DummyCreateTexture)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateTexture2D)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateTexture3D)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateTextureCube)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_060)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_061)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetTexture)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetVertexTexture)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_064)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetMaterial)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_066)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateModelResource)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateCloneResource)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateModel)(fox::gr::dg::DgDx11*);
						void(__fastcall* DestroyModel)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartModelObject)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndModelObject)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_073)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_074)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndModelDebug)(fox::gr::dg::DgDx11*);
						void(__fastcall* GetModelDebugInfo)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateClone)(fox::gr::dg::DgDx11*);
						void(__fastcall* DestroyClone)(fox::gr::dg::DgDx11*);
						void(__fastcall* GetCloneMaterials)(fox::gr::dg::DgDx11*);
						void(__fastcall* ValidateClone)(fox::gr::dg::DgDx11*);
						void(__fastcall* GetCloneModelMaxSize)(fox::gr::dg::DgDx11*);
						void(__fastcall* PerformanceEventBegin)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_083)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_084)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_085)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_086)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_087)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_088)(fox::gr::dg::DgDx11*);
						void(__fastcall* InitShadow)(fox::gr::dg::DgDx11*);
						void(__fastcall* QuitShadow)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartShadow)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_092)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndShadow)(fox::gr::dg::DgDx11*);
						void(__fastcall* InitGBuffer)(fox::gr::dg::DgDx11*);
						void(__fastcall* QuitGBuffer)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartGeometryPass)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndGeometryPass)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartGeometryMaskPass)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_099)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartGeometryMaskPass1)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_101)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartGeometryDecalPass)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndGeometryDecalPass)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetupDeferredShading)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetupDeferredShadingTexture)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartDeferredShading)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_107)(fox::gr::dg::DgDx11*);
						void(__fastcall* DrawDeferredShading)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartDeferredRendering)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_110)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartGeometryVelocityPass)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndGeometryVelocityPass)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartGeometryMaskedVelocityPass)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_114)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_115)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_116)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_117)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartShrinkBuffer)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndShrinkBuffer)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_120)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_121)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_122)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_123)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_124)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_125)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_126)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_127)(fox::gr::dg::DgDx11*);
						void(__fastcall* InitLineIntegralSSAO)(fox::gr::dg::DgDx11*);
						void(__fastcall* QuitLineIntegralSSAO)(fox::gr::dg::DgDx11*);
						void(__fastcall* ResetLineIntegralSSAO)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartLineIntegralSSAO)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_132)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_133)(fox::gr::dg::DgDx11*);
						void(__fastcall* InitPostFilter)(fox::gr::dg::DgDx11*);
						void(__fastcall* QuitPostFilter)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartPostFilter)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndPostFilter)(fox::gr::dg::DgDx11*);
						void(__fastcall* InitMotionBlur)(fox::gr::dg::DgDx11*);
						void(__fastcall* QuitMotionBlur)(fox::gr::dg::DgDx11*);
						void(__fastcall* ResetMotionBlur)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartMotionBlur)(fox::gr::dg::DgDx11*);
						void(__fastcall* DrawMotionBlur)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndMotionBlur)(fox::gr::dg::DgDx11*);
						void(__fastcall* InitScreenSpaceVelocity)(fox::gr::dg::DgDx11*);
						void(__fastcall* QuitScreenSpaceVelocity)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_146)(fox::gr::dg::DgDx11*);
						void(__fastcall* StartScreenSpaceVelocity)(fox::gr::dg::DgDx11*);
						void(__fastcall* DrawScreenSpaceVelocity)(fox::gr::dg::DgDx11*);
						void(__fastcall* EndScreenSpaceVelocity)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_150)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_151)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_152)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_153)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_154)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_155)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_156)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_157)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_158)(fox::gr::dg::DgDx11*);
						void(__fastcall* GetBackBufferWidth)(fox::gr::dg::DgDx11*);
						void(__fastcall* GetBackBufferHeight)(fox::gr::dg::DgDx11*);
						void(__fastcall* DrawFullScreenTriangle)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_162)(fox::gr::dg::DgDx11*);
						void(__fastcall* ResolvePrimaryBufferReplica)(fox::gr::dg::DgDx11*);
						void(__fastcall* GetPrimaryBufferReplicaAsTexture)(fox::gr::dg::DgDx11*);
						void(__fastcall* GetPrimaryBufferAsTexture)(fox::gr::dg::DgDx11*);
						void(__fastcall* GetMacroPacketBuffer)(fox::gr::dg::DgDx11*);
						void(__fastcall* BuildCommandFromOverlayModelMacroPackets)(fox::gr::dg::DgDx11*);
						void(__fastcall* GetRenderCondition)(fox::gr::dg::DgDx11*);
						void(__fastcall* SetRenderCondition)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_170)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_171)(fox::gr::dg::DgDx11*);
						void(__fastcall* CreateSubCommandContext)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_173)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_174)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_175)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_176)(fox::gr::dg::DgDx11*);
						void(__fastcall* InitTemporaryOutput)(fox::gr::dg::DgDx11*);
						void(__fastcall* meth_178)(fox::gr::dg::DgDx11*);
					};

					vtable* __vftable;
					char __pad0[140];
					TonemapDesc tonemapDesc;
					char __pad5[52];
					DynamicVertexBuffer* dynamicVertexBuffer;
					DynamicIndexBuffer* dynamicIndexBuffer;
					RenderParameter* renderParameter;
					unsigned int a6;
					unsigned int a7;
					void* unk4;
					char __pad1[32];
					MacroPacketBuffer* macroPacketBuffer;
					ShadowConstantRegister* shadowConstantRegister;
					RenderStateContext* renderStateContext;
					RenderResourceContext* renderResourceContext;
					void* unk2;
					gn::DeviceContext* deviceContext;
					void* unk3;
					void* commandListCluster;
					char __pad3[32];
					unsigned int a1;
					unsigned int a2;
					unsigned int a3;
					unsigned int a4;
					unsigned int a5;
					char __pad4[116];
				};

				struct Dg;
				struct Dg
				{
					struct vtable
					{
						void(__fastcall* meth_001)(fox::gr::dg::Dg*);
						void(__fastcall* EndInstance)(fox::gr::dg::Dg*);
						void(__fastcall* SetupShaderResources)(fox::gr::dg::Dg*);
						void(__fastcall* meth_004)(fox::gr::dg::Dg*);
						void(__fastcall* Wait)(fox::gr::dg::Dg*);
						void(__fastcall* Kick)(fox::gr::dg::Dg*);
						void(__fastcall* Start)(fox::gr::dg::Dg*);
						void(__fastcall* End)(fox::gr::dg::Dg*);
						void(__fastcall* SkipRendering)(fox::gr::dg::Dg*);
						void(__fastcall* DestroyRenderInfo)(fox::gr::dg::Dg*);
						void(__fastcall* meth_011)(fox::gr::dg::Dg*);
						void(__fastcall* EndRender)(fox::gr::dg::Dg*);
						void(__fastcall* InitViewport)(fox::gr::dg::Dg*);
						void(__fastcall* EndViewport)(fox::gr::dg::Dg*);
						void(__fastcall* RestoreViewport)(fox::gr::dg::Dg*);
						void(__fastcall* SetViewportDirect)(fox::gr::dg::Dg*);
						void(__fastcall* SetGamma)(fox::gr::dg::Dg*);
						void(__fastcall* meth_018)(fox::gr::dg::Dg*);
						void(__fastcall* ValidateVideoOutSettings)(fox::gr::dg::Dg*);
						void(__fastcall* SetVideoOutSettings)(fox::gr::dg::Dg*);
						void(__fastcall* meth_021)(fox::gr::dg::Dg*);
						void(__fastcall* meth_022)(fox::gr::dg::Dg*);
						void(__fastcall* CreateRenderTarget)(fox::gr::dg::Dg*);
						void(__fastcall* RestoreRenderTarget)(fox::gr::dg::Dg*);
						void(__fastcall* CreateRenderBuffer)(fox::gr::dg::Dg*);
						void(__fastcall* SetViewportScissor)(fox::gr::dg::Dg*);
						void(__fastcall* BeginRenderBuffer)(fox::gr::dg::Dg*);
						void(__fastcall* EndRenderBuffer)(fox::gr::dg::Dg*);
						void(__fastcall* GetCurrentRenderBuffer)(fox::gr::dg::Dg*);
						void(__fastcall* ResolveRenderTexture)(fox::gr::dg::Dg*);
						void(__fastcall* GetPrimaryDepthTarget)(fox::gr::dg::Dg*);
						void(__fastcall* meth_032)(fox::gr::dg::Dg*);
						void(__fastcall* meth_033)(fox::gr::dg::Dg*);
						void(__fastcall* meth_034)(fox::gr::dg::Dg*);
						void(__fastcall* SetShader)(fox::gr::dg::Dg*);
						void(__fastcall* SetMatrix)(fox::gr::dg::Dg*);
						void(__fastcall* SetVector)(fox::gr::dg::Dg*);
						void(__fastcall* SetVectors)(fox::gr::dg::Dg*);
						void(__fastcall* SetSkinningParam)(fox::gr::dg::Dg*);
						void(__fastcall* CreateShaderFileResource)(fox::gr::dg::Dg*);
						void(__fastcall* meth_041)(fox::gr::dg::Dg*);
						void(__fastcall* SetShaderObject)(fox::gr::dg::Dg*);
						void(__fastcall* CreateInputLayoutResource)(fox::gr::dg::Dg*);
						void(__fastcall* SetInputLayout)(fox::gr::dg::Dg*);
						void(__fastcall* meth_045)(fox::gr::dg::Dg*);
						void(__fastcall* CreateRenderStateResource)(fox::gr::dg::Dg*);
						void(__fastcall* meth_047)(fox::gr::dg::Dg*);
						void(__fastcall* UpdateStatus)(fox::gr::dg::Dg*);
						void(__fastcall* SetupRenderStateByFrame)(fox::gr::dg::Dg*);
						void(__fastcall* CreateVertexBufferResource)(fox::gr::dg::Dg*);
						void(__fastcall* CreateIndexBufferResource)(fox::gr::dg::Dg*);
						void(__fastcall* SetVertexBuffer)(fox::gr::dg::Dg*);
						void(__fastcall* SetIndexBuffer)(fox::gr::dg::Dg*);
						void(__fastcall* CreateVertexStreamResource)(fox::gr::dg::Dg*);
						void(__fastcall* SetVertexStream)(fox::gr::dg::Dg*);
						void(__fastcall* DummyCreateTexture)(fox::gr::dg::Dg*);
						void(__fastcall* CreateTexture2D)(fox::gr::dg::Dg*);
						void(__fastcall* CreateTexture3D)(fox::gr::dg::Dg*);
						void(__fastcall* CreateTextureCube)(fox::gr::dg::Dg*);
						void(__fastcall* meth_060)(fox::gr::dg::Dg*);
						void(__fastcall* meth_061)(fox::gr::dg::Dg*);
						void(__fastcall* SetTexture)(fox::gr::dg::Dg*);
						void(__fastcall* SetVertexTexture)(fox::gr::dg::Dg*);
						void(__fastcall* meth_064)(fox::gr::dg::Dg*);
						void(__fastcall* SetMaterial)(fox::gr::dg::Dg*);
						void(__fastcall* meth_066)(fox::gr::dg::Dg*);
						void(__fastcall* CreateModelResource)(fox::gr::dg::Dg*);
						void(__fastcall* CreateCloneResource)(fox::gr::dg::Dg*);
						void(__fastcall* CreateModel)(fox::gr::dg::Dg*);
						void(__fastcall* DestroyModel)(fox::gr::dg::Dg*);
						void(__fastcall* StartModelObject)(fox::gr::dg::Dg*);
						void(__fastcall* EndModelObject)(fox::gr::dg::Dg*);
						void(__fastcall* meth_073)(fox::gr::dg::Dg*);
						void(__fastcall* meth_074)(fox::gr::dg::Dg*);
						void(__fastcall* EndModelDebug)(fox::gr::dg::Dg*);
						void(__fastcall* GetModelDebugInfo)(fox::gr::dg::Dg*);
						void(__fastcall* CreateClone)(fox::gr::dg::Dg*);
						void(__fastcall* DestroyClone)(fox::gr::dg::Dg*);
						void(__fastcall* GetCloneMaterials)(fox::gr::dg::Dg*);
						void(__fastcall* ValidateClone)(fox::gr::dg::Dg*);
						void(__fastcall* GetCloneModelMaxSize)(fox::gr::dg::Dg*);
						void(__fastcall* PerformanceEventBegin)(fox::gr::dg::Dg*);
						void(__fastcall* meth_083)(fox::gr::dg::Dg*);
						void(__fastcall* meth_084)(fox::gr::dg::Dg*);
						void(__fastcall* meth_085)(fox::gr::dg::Dg*);
						void(__fastcall* meth_086)(fox::gr::dg::Dg*);
						void(__fastcall* meth_087)(fox::gr::dg::Dg*);
						void(__fastcall* meth_088)(fox::gr::dg::Dg*);
						void(__fastcall* InitShadow)(fox::gr::dg::Dg*);
						void(__fastcall* QuitShadow)(fox::gr::dg::Dg*);
						void(__fastcall* StartShadow)(fox::gr::dg::Dg*);
						void(__fastcall* meth_092)(fox::gr::dg::Dg*);
						void(__fastcall* EndShadow)(fox::gr::dg::Dg*);
						void(__fastcall* InitGBuffer)(fox::gr::dg::Dg*);
						void(__fastcall* QuitGBuffer)(fox::gr::dg::Dg*);
						void(__fastcall* StartGeometryPass)(fox::gr::dg::Dg*);
						void(__fastcall* EndGeometryPass)(fox::gr::dg::Dg*);
						void(__fastcall* StartGeometryMaskPass)(fox::gr::dg::Dg*);
						void(__fastcall* meth_099)(fox::gr::dg::Dg*);
						void(__fastcall* StartGeometryMaskPass1)(fox::gr::dg::Dg*);
						void(__fastcall* meth_101)(fox::gr::dg::Dg*);
						void(__fastcall* StartGeometryDecalPass)(fox::gr::dg::Dg*);
						void(__fastcall* EndGeometryDecalPass)(fox::gr::dg::Dg*);
						void(__fastcall* SetupDeferredShading)(fox::gr::dg::Dg*);
						void(__fastcall* SetupDeferredShadingTexture)(fox::gr::dg::Dg*);
						void(__fastcall* StartDeferredShading)(fox::gr::dg::Dg*);
						void(__fastcall* meth_107)(fox::gr::dg::Dg*);
						void(__fastcall* DrawDeferredShading)(fox::gr::dg::Dg*);
						void(__fastcall* StartDeferredRendering)(fox::gr::dg::Dg*);
						void(__fastcall* meth_110)(fox::gr::dg::Dg*);
						void(__fastcall* StartGeometryVelocityPass)(fox::gr::dg::Dg*);
						void(__fastcall* EndGeometryVelocityPass)(fox::gr::dg::Dg*);
						void(__fastcall* StartGeometryMaskedVelocityPass)(fox::gr::dg::Dg*);
						void(__fastcall* meth_114)(fox::gr::dg::Dg*);
						void(__fastcall* meth_115)(fox::gr::dg::Dg*);
						void(__fastcall* meth_116)(fox::gr::dg::Dg*);
						void(__fastcall* meth_117)(fox::gr::dg::Dg*);
						void(__fastcall* StartShrinkBuffer)(fox::gr::dg::Dg*);
						void(__fastcall* EndShrinkBuffer)(fox::gr::dg::Dg*);
						void(__fastcall* meth_120)(fox::gr::dg::Dg*);
						void(__fastcall* meth_121)(fox::gr::dg::Dg*);
						void(__fastcall* meth_122)(fox::gr::dg::Dg*);
						void(__fastcall* meth_123)(fox::gr::dg::Dg*);
						void(__fastcall* meth_124)(fox::gr::dg::Dg*);
						void(__fastcall* meth_125)(fox::gr::dg::Dg*);
						void(__fastcall* meth_126)(fox::gr::dg::Dg*);
						void(__fastcall* meth_127)(fox::gr::dg::Dg*);
						void(__fastcall* InitLineIntegralSSAO)(fox::gr::dg::Dg*);
						void(__fastcall* QuitLineIntegralSSAO)(fox::gr::dg::Dg*);
						void(__fastcall* ResetLineIntegralSSAO)(fox::gr::dg::Dg*);
						void(__fastcall* StartLineIntegralSSAO)(fox::gr::dg::Dg*);
						void(__fastcall* meth_132)(fox::gr::dg::Dg*);
						void(__fastcall* meth_133)(fox::gr::dg::Dg*);
						void(__fastcall* InitPostFilter)(fox::gr::dg::Dg*);
						void(__fastcall* QuitPostFilter)(fox::gr::dg::Dg*);
						void(__fastcall* StartPostFilter)(fox::gr::dg::Dg*);
						void(__fastcall* EndPostFilter)(fox::gr::dg::Dg*);
						void(__fastcall* InitMotionBlur)(fox::gr::dg::Dg*);
						void(__fastcall* QuitMotionBlur)(fox::gr::dg::Dg*);
						void(__fastcall* ResetMotionBlur)(fox::gr::dg::Dg*);
						void(__fastcall* StartMotionBlur)(fox::gr::dg::Dg*);
						void(__fastcall* DrawMotionBlur)(fox::gr::dg::Dg*);
						void(__fastcall* EndMotionBlur)(fox::gr::dg::Dg*);
						void(__fastcall* InitScreenSpaceVelocity)(fox::gr::dg::Dg*);
						void(__fastcall* QuitScreenSpaceVelocity)(fox::gr::dg::Dg*);
						void(__fastcall* meth_146)(fox::gr::dg::Dg*);
						void(__fastcall* StartScreenSpaceVelocity)(fox::gr::dg::Dg*);
						void(__fastcall* DrawScreenSpaceVelocity)(fox::gr::dg::Dg*);
						void(__fastcall* EndScreenSpaceVelocity)(fox::gr::dg::Dg*);
						void(__fastcall* meth_150)(fox::gr::dg::Dg*);
						void(__fastcall* meth_151)(fox::gr::dg::Dg*);
						void(__fastcall* meth_152)(fox::gr::dg::Dg*);
						void(__fastcall* meth_153)(fox::gr::dg::Dg*);
						void(__fastcall* meth_154)(fox::gr::dg::Dg*);
						void(__fastcall* meth_155)(fox::gr::dg::Dg*);
						void(__fastcall* meth_156)(fox::gr::dg::Dg*);
						void(__fastcall* meth_157)(fox::gr::dg::Dg*);
						void(__fastcall* meth_158)(fox::gr::dg::Dg*);
						void(__fastcall* GetBackBufferWidth)(fox::gr::dg::Dg*);
						void(__fastcall* GetBackBufferHeight)(fox::gr::dg::Dg*);
						void(__fastcall* DrawFullScreenTriangle)(fox::gr::dg::Dg*);
						void(__fastcall* meth_162)(fox::gr::dg::Dg*);
						void(__fastcall* ResolvePrimaryBufferReplica)(fox::gr::dg::Dg*);
						void(__fastcall* GetPrimaryBufferReplicaAsTexture)(fox::gr::dg::Dg*);
						void(__fastcall* GetPrimaryBufferAsTexture)(fox::gr::dg::Dg*);
						void(__fastcall* GetMacroPacketBuffer)(fox::gr::dg::Dg*);
						void(__fastcall* BuildCommandFromOverlayModelMacroPackets)(fox::gr::dg::Dg*);
						void(__fastcall* GetRenderCondition)(fox::gr::dg::Dg*);
						void(__fastcall* SetRenderCondition)(fox::gr::dg::Dg*);
						void(__fastcall* meth_170)(fox::gr::dg::Dg*);
						void(__fastcall* meth_171)(fox::gr::dg::Dg*);
						void(__fastcall* CreateSubCommandContext)(fox::gr::dg::Dg*);
						void(__fastcall* meth_173)(fox::gr::dg::Dg*);
						void(__fastcall* meth_174)(fox::gr::dg::Dg*);
						void(__fastcall* meth_175)(fox::gr::dg::Dg*);
						void(__fastcall* meth_176)(fox::gr::dg::Dg*);
						void(__fastcall* InitTemporaryOutput)(fox::gr::dg::Dg*);
						void(__fastcall* meth_178)(fox::gr::dg::Dg*);
					};

					vtable* __vftable;
					char __pad0[140];
					fox::gr::TonemapDesc tonemapDesc;
					char __pad1[140];
				};

				namespace plugins
				{
					struct Draw2DRenderer
					{
						char __pad0[720];
						Vectormath::Aos::Vector4 viewVec1;
						Vectormath::Aos::Vector4 viewVec2;
						BuildDraw2DParameters* parameters;
						CommandBuffer* commandBuffer;
						RenderStateContext* renderStateContext;
						Camera* defaultCamera;
						Camera* camera;
						ResourceId shader;
						gn::Buffer* buffer;
						unsigned int size;
						unsigned int bufferSizeAllocated;
						unsigned int bufferSizeUsed;
						char __pad2[4];
						unsigned short w1;
						char unk1;
						char __pad3[5];
						DynamicVertexBuffer* vertexBuffer;
						void* ptr1;
						ResourceId unkResource;
						char __pad4[36];
						char unk2;
						char __pad5[51];
						_TextureGlyphData* glyphData;
						char __pad6[8];
					};

					static_assert(offsetof(Draw2DRenderer, glyphData) == 0x3B0);
				}
			}

			struct Packet2D
			{
				Packet2D(unsigned char type_, unsigned char size_)
					: type(type_), size(size_)
				{
				}

				template <typename T>
				T* next()
				{
					return reinterpret_cast<T*>(reinterpret_cast<size_t>(this) + this->size);
				}

				unsigned char type{};
				unsigned char flags{};
				unsigned short size{};
			};

			struct Packet2DBuffer
			{
				void* a1;
				void* a2;
				Packet2D* packet;
				void* a4;
				int packetSize;
				int flags;
				int a6;
			};

			struct _fp16
			{
				unsigned short v[4];
			};

			struct Material
			{
				struct vtable
				{
					void(__fastcall* __destructor)(Material*, char);
				};

				vtable* __vftable;
				int unk;
				char __pad0[4];
				game::fox::gr::ResourceId resource;
			};

			static_assert(offsetof(Material, resource) == 0x10);

			struct MaterialParameter
			{

			};

			struct Packet2DLine : Packet2D
			{
				Packet2DLine() : Packet2D(1, 24) {}
				char __pad0[20]{};
			};

			struct Packet2DTriangle : Packet2D
			{
				Packet2DTriangle() : Packet2D(2, 36) {}
				char __pad0[32]{};
			};

			struct Packet2DQuad : Packet2D
			{
				Packet2DQuad() : Packet2D(3, 44) {}
				char __pad0[40]{};
			};

			struct Packet2DBox : Packet2D
			{
				Packet2DBox() : Packet2D(4, 20) {}
				unsigned int color{};
				unsigned __int16 f1{};
				unsigned __int16 f2{};
				unsigned __int16 f3{};
				unsigned __int16 f4{};
				unsigned __int16 f5{};
			};

			struct Packet2DSprite : Packet2D
			{
				Packet2DSprite() : Packet2D(5, 28) {}
				char __pad0[24]{};
			};

			struct Packet2DSprite2D : Packet2D
			{
				Packet2DSprite2D() : Packet2D(6, 32) {}
				char __pad0[28]{};
			};

			struct Packet2DRSprite : Packet2D
			{
				Packet2DRSprite() : Packet2D(7, 28) {}
				char __pad0[24]{};
			};

			struct Packet2DString : Packet2D
			{
				Packet2DString() : Packet2D(8, 32) {}
				fox::gr::dg::StringFontMetricsCache* fontMetricsCache{};
				unsigned short glyphUnk{};
				unsigned short glyphHeight{};
				unsigned short glyphWidth{};
				unsigned short flags{};
				unsigned int fontType{};
				unsigned short glyphSpacing{};
				unsigned short a9{};
			};

			struct Packet2DString2D : Packet2D
			{
				Packet2DString2D() : Packet2D(9, 48) {}
				char __pad0[44]{};
			};

			struct Packet2DCube : Packet2D
			{
				Packet2DCube() : Packet2D(10, 28) {}
				char __pad0[24]{};
			};

			struct Packet2DLineStrip : Packet2D
			{
				Packet2DLineStrip() : Packet2D(11, 8) {}
				char __pad0[4]{};
			};

			struct Packet2DVertex : Packet2D
			{
				Packet2DVertex() : Packet2D(0, 16) {}
				unsigned int color{};
				unsigned short v[5]{};
			};

			template <size_t Count>
			struct Packet2DTriangleStrip : Packet2D
			{
				Packet2DTriangleStrip() : Packet2D(12, 8)
				{
					this->size += Count * sizeof(Packet2DVertex);
				}
				int count = Count;
				Packet2DVertex vertices[Count]{};
			};

			struct Packet2DViewport : Packet2D
			{
				Packet2DViewport() : Packet2D(13, 12) {}
				_fp16 v{};
			};

			struct Packet2DViewmap : Packet2D
			{
				Packet2DViewmap() : Packet2D(14, 28) {}
				float v[3][3]{};
			};

			struct Packet2DWorldCoords : Packet2D
			{
				Packet2DWorldCoords() : Packet2D(15, 4) {}
			};

			struct Packet2DScreenCoords : Packet2D
			{
				Packet2DScreenCoords() : Packet2D(16, 4) {}
			};

			struct Packet2DMatrix : Packet2D
			{
				Packet2DMatrix() : Packet2D(17, 44) {}
				float v1[3]{};
				float v2[3]{};
				float quat[4]{};
			};

			struct Packet2DRotation : Packet2D
			{
				Packet2DRotation() : Packet2D(18, 20) {}
				float quat[4]{};
			};

			struct Packet2DTranslation : Packet2D
			{
				Packet2DTranslation() : Packet2D(19, 16) {}
				float x{};
				float y{};
				float z{};
			};

			struct Packet2DScale : Packet2D
			{
				Packet2DScale() : Packet2D(20, 16) {}
				float x{};
				float y{};
				float z{};
			};

			struct Packet2DBillboard : Packet2D
			{
				Packet2DBillboard() : Packet2D(21, 8) {}
				float f1{};
				float f2{};
			};

			struct Packet2DPerspective : Packet2D
			{
				Packet2DPerspective() : Packet2D(22, 24) {}
				float f1{};
				float f2{};
				float f3{};
				float f4{};
				float f5{};
			};

			struct Packet2DFlat : Packet2D
			{
				Packet2DFlat() : Packet2D(23, 4) {}
			};

			struct Packet2DDepthMode : Packet2D
			{
				Packet2DDepthMode() : Packet2D(24, 4) {}
			};

			struct Packet2DCullMode : Packet2D
			{
				Packet2DCullMode() : Packet2D(25, 4) {}
			};

			struct Packet2DTexture : Packet2D
			{
				Packet2DTexture() : Packet2D(26, 8) {}
				int id{};
			};

			struct Packet2DAlpha : Packet2D
			{
				Packet2DAlpha() : Packet2D(27, 4) {}
			};

			struct Packet2DColor : Packet2D
			{
				Packet2DColor() : Packet2D(28, 12) {}
				_fp16 rgba{};
			};

			struct Packet2DStencil : Packet2D
			{
				Packet2DStencil() : Packet2D(29, 16) {}
				unsigned char a1{};
				unsigned char a2{};
				unsigned char a3{};
				unsigned char a4{};
				unsigned char a5{};
				unsigned char a6{};
				unsigned char a7{};
				unsigned char a8{};
				int a9{};
			};

			struct Packet2DClearStencil : Packet2D
			{
				Packet2DClearStencil() : Packet2D(30, 8) {}
				int a1{};
			};

			struct Packet2DMaterial : Packet2D
			{
				Packet2DMaterial() : Packet2D(32, 16) {}
				Material* material{};
			};

			struct Packet2DMaterialWork : Packet2D
			{
				Packet2DMaterialWork() : Packet2D(33, 24) {}
				char __pad0[20]{};
			};

			struct Packet2DUserMatrix : Packet2D
			{
				Packet2DUserMatrix() : Packet2D(34, 68) {}
				char __pad0[64]{};
			};

			struct Packet2DPush : Packet2D
			{
				Packet2DPush() : Packet2D(38, 4) {}
			};

			struct Packet2DPop : Packet2D
			{
				Packet2DPop() : Packet2D(39, 4) {}
			};

			struct Packet2DResolve : Packet2D
			{
				Packet2DResolve() : Packet2D(40, 8) {}
				char __pad0[4]{};
			};

			struct Packet2DCopyRenderTarget : Packet2D
			{
				Packet2DCopyRenderTarget() : Packet2D(41, 12) {}
				char __pad0[8]{};
			};

			struct Packet2DDrawIndices : Packet2D
			{
				Packet2DDrawIndices() : Packet2D(45, 24) {}
				char __pad0[20]{};
			};

			struct Packet2DUserVertexBuffer : Packet2D
			{
				Packet2DUserVertexBuffer() : Packet2D(46, 24) {}
				char __pad0[20]{};
			};

			struct Packet2DBeginRenderToTemporary : Packet2D
			{
				Packet2DBeginRenderToTemporary() : Packet2D(47, 16) {}
				char __pad0[12]{};
			};

			struct Packet2DEndRenderToTemporary : Packet2D
			{
				Packet2DEndRenderToTemporary() : Packet2D(48, 4) {}
			};

			struct Packet2DSetTemporaryTexture : Packet2D
			{
				Packet2DSetTemporaryTexture() : Packet2D(49, 8) {}
				char __pad0[4]{};
			};
		}

		struct MessageBox2;

		namespace impl
		{
			struct TimeSystemImpl;

			struct TimeSystemImpl_vtbl
			{
				unsigned __int64(__fastcall* GetCurrentTickTime)(TimeSystemImpl* this_);
				double(__fastcall* GetSecondWithTickTime)(TimeSystemImpl* this_, __int64);
				double(__fastcall* GetSecond)(TimeSystemImpl* this_);
				unsigned __int64(__fastcall* GetTickTimeFrequency)(TimeSystemImpl* this_);
				void(__fastcall* __destructor)(TimeSystemImpl* this_);
			};

			struct TimeSystemImpl
			{
				TimeSystemImpl_vtbl* __vftable;
				char __pad0[8];
				double deltaGameTime;
				char __pad1[8];
				double gameTimeRate;
				double frameTime;
				double elapsedTimeSinceStartup;
				int frameIndex;
			};

			static_assert(offsetof(TimeSystemImpl, deltaGameTime) == 16);
			static_assert(offsetof(TimeSystemImpl, gameTimeRate) == 32);
			static_assert(offsetof(TimeSystemImpl, frameTime) == 40);
			static_assert(offsetof(TimeSystemImpl, elapsedTimeSinceStartup) == 48);
			static_assert(offsetof(TimeSystemImpl, frameIndex) == 56);

			namespace message2
			{
				struct MessageBox2Impl;
				struct MessageBox2Impl
				{
					struct vtable
					{
						void(__fastcall* SendMessageToSubscribers)(fox::impl::message2::MessageBox2Impl*);
						void(__fastcall* SendMessageTo)(fox::impl::message2::MessageBox2Impl*);
						void(__fastcall* ResendMessage)(fox::impl::message2::MessageBox2Impl*);
						void(__fastcall* meth_004)(fox::impl::message2::MessageBox2Impl*);
						void(__fastcall* __destructor)(fox::impl::message2::MessageBox2Impl*);
					};

					vtable* __vftable;
					__int64 unk1;
					__int64 unk2;
					unsigned int a1;
					char a2;
					char __pad1[3];
					unsigned int flags;
					char __pad0[4];
				};

				struct MessageSystem2Impl;
				struct MessageSystem2Impl
				{
					struct vtable
					{
						fox::MessageBox2* (__fastcall* CreateMessageBox)(fox::impl::message2::MessageSystem2Impl*);
						void(__fastcall* DeleteMessageBox)(fox::impl::message2::MessageSystem2Impl*);
						void(__fastcall* Subscribe)(fox::impl::message2::MessageSystem2Impl*);
						void(__fastcall* SendMessage1)(fox::impl::message2::MessageSystem2Impl*);
						void(__fastcall* SendMessageToSubscribers)(fox::impl::message2::MessageSystem2Impl*);
						void(__fastcall* DeliverMessages)(fox::impl::message2::MessageSystem2Impl*);
						void(__fastcall* __destructor)(fox::impl::message2::MessageSystem2Impl*);
					};

					vtable* __vftable;
					char __pad0[9064];
				};
			}
		}

		struct MessageBox2 : fox::impl::message2::MessageBox2Impl
		{

		};

		struct MessageSystem2 : fox::impl::message2::MessageSystem2Impl
		{

		};

		struct MessageBox2DelegateInterface;
		struct MessageBox2DelegateInterface
		{
			struct vtable
			{
				void(__fastcall* ReceiveMessage)(fox::MessageBox2DelegateInterface*);
				void(__fastcall* __destructor)(fox::MessageBox2DelegateInterface*);
			};

			vtable* __vftable;
		};

		struct QuarkRecreationCallbackInterface;
		struct QuarkRecreationCallbackInterface
		{
			struct vtable
			{
				void(__fastcall* meth_001)(fox::QuarkRecreationCallbackInterface*);
				void(__fastcall* __destructor)(fox::QuarkRecreationCallbackInterface*);
			};

			vtable* __vftable;
		};

		namespace concurrent
		{
			namespace impl
			{
				struct QueueBase
				{
					struct vtable
					{

					};

					vtable* __vftable;
					char __pad0[64];
				};
			}
		}

		namespace uix
		{
			struct PrefabRecordCallFunc;
		}

		namespace ui
		{
			struct LineDraw;

			struct SoundControl;

			struct TextUnit
			{
				char* text;
				unsigned int a2;
				unsigned int flags;
				unsigned __int16 textLength;
				unsigned __int16 a5;
				float textWidth;
				float textHeight;
				unsigned int paletteColor;
			};

			static_assert(offsetof(TextUnit, textWidth) == 20);
			static_assert(offsetof(TextUnit, textHeight) == 24);
			static_assert(offsetof(TextUnit, paletteColor) == 28);

			struct ModelNode
			{

			};

			struct ModelNodeText;

			struct ModelNodeText_vtbl
			{
				void(__fastcall* __destructor)(ModelNodeText*);
				void(__fastcall* GetPacketBuffer)(ModelNodeText*);
				void(__fastcall* InitializePacketBuffer)(ModelNodeText*);
				void(__fastcall* ReleasePacketBuffer)(ModelNodeText*);
				void(__fastcall* UpdatePacket)(ModelNodeText*);
				void* (__fastcall* GetBoundingBox)(ModelNodeText*);
				void(__fastcall* nullsub1)(ModelNodeText*);
				void(__fastcall* nullsub2)(ModelNodeText*);
				void(__fastcall* nullsub3)(ModelNodeText*);
			};

#pragma pack(push, 1)
			struct ModelNodeText
			{
				fox::ui::ModelNodeText_vtbl* __vftable;
				char __pad0[106];
				char f0;
				char __pad1[21];
				void* f1;
				short flags;
				char enableTextScroll;
				char __pad2[1];
				int f4;
				void* f5;
				void* f6;
				void* f7;
				void* f8;
				int f9;
				int f10;
				int packetBufferSize;
				char __pad3[4];
				fox::gr::Packet2DBuffer* packetBuffer;
				void* f12;
				char textAlign;
				char f13;
				char f14;
				char f15;
				short f16;
				char __pad4[2];
				float displayAreaOffsetX;
				float displayAreaOffsetY;
				float displayAreaWidth;
				float displayAreaHeight;
				Vectormath::Aos::Vector4 displaySizes;
				void* f20;
				int f21;
				char __pad5[4];
				void* f22;
				void* f23;
				int f24;
				char __pad6[4];
				void* f25;
				void* f26;
				fox::ui::TextUnit* textUnit;
				int f28;
				char __pad7[4];
				void* f29;
				void* f30;
				int lineWidthsCount;
				int f31;
				float* lineWidths;
				int lineHeightsCount;
				int f33;
				float* lineHeights;
				float lineHeight;
				float lineWidth;
				void* f36;
				void* f37;
				void* f38;
				void* f39;
				void* f40;
				void* f41;
				float scrollSpeed;
				float textOffsetX;
				int textScrollUnk2;
				int textScrollUnk3;
			};
#pragma pack(pop)

			static_assert(offsetof(fox::ui::ModelNodeText, __pad0) == 8);
			static_assert(offsetof(fox::ui::ModelNodeText, f0) == 114);
			static_assert(offsetof(fox::ui::ModelNodeText, __pad1) == 115);
			static_assert(offsetof(fox::ui::ModelNodeText, f1) == 136);
			static_assert(offsetof(fox::ui::ModelNodeText, flags) == 144);
			//static_assert(offsetof(fox::ui::ModelNodeText, f3) == 146);
			static_assert(offsetof(fox::ui::ModelNodeText, __pad2) == 147);
			static_assert(offsetof(fox::ui::ModelNodeText, f4) == 148);
			static_assert(offsetof(fox::ui::ModelNodeText, f5) == 152);
			static_assert(offsetof(fox::ui::ModelNodeText, f6) == 160);
			static_assert(offsetof(fox::ui::ModelNodeText, f7) == 168);
			static_assert(offsetof(fox::ui::ModelNodeText, f8) == 176);
			static_assert(offsetof(fox::ui::ModelNodeText, f9) == 184);
			static_assert(offsetof(fox::ui::ModelNodeText, f10) == 188);
			static_assert(offsetof(fox::ui::ModelNodeText, __pad3) == 196);
			static_assert(offsetof(fox::ui::ModelNodeText, packetBuffer) == 200);
			static_assert(offsetof(fox::ui::ModelNodeText, f12) == 208);
			//static_assert(offsetof(fox::ui::ModelNodeText, f13) == 216);
			static_assert(offsetof(fox::ui::ModelNodeText, f14) == 218);
			static_assert(offsetof(fox::ui::ModelNodeText, f15) == 219);
			static_assert(offsetof(fox::ui::ModelNodeText, f16) == 220);
			static_assert(offsetof(fox::ui::ModelNodeText, __pad4) == 222);
			static_assert(offsetof(fox::ui::ModelNodeText, f20) == 256);
			static_assert(offsetof(fox::ui::ModelNodeText, f21) == 264);
			static_assert(offsetof(fox::ui::ModelNodeText, __pad5) == 268);
			static_assert(offsetof(fox::ui::ModelNodeText, f22) == 272);
			static_assert(offsetof(fox::ui::ModelNodeText, f23) == 280);
			static_assert(offsetof(fox::ui::ModelNodeText, f24) == 288);
			static_assert(offsetof(fox::ui::ModelNodeText, __pad6) == 292);
			static_assert(offsetof(fox::ui::ModelNodeText, f25) == 296);
			static_assert(offsetof(fox::ui::ModelNodeText, f26) == 304);
			static_assert(offsetof(fox::ui::ModelNodeText, textUnit) == 312);
			static_assert(offsetof(fox::ui::ModelNodeText, f28) == 320);
			static_assert(offsetof(fox::ui::ModelNodeText, __pad7) == 324);
			static_assert(offsetof(fox::ui::ModelNodeText, f29) == 328);
			static_assert(offsetof(fox::ui::ModelNodeText, f30) == 336);
			//static_assert(offsetof(fox::ui::ModelNodeText, f35) == 376);
			static_assert(offsetof(fox::ui::ModelNodeText, f36) == 384);
			static_assert(offsetof(fox::ui::ModelNodeText, f37) == 392);
			static_assert(offsetof(fox::ui::ModelNodeText, f38) == 400);
			static_assert(offsetof(fox::ui::ModelNodeText, f39) == 408);
			static_assert(offsetof(fox::ui::ModelNodeText, f40) == 416);
			static_assert(offsetof(fox::ui::ModelNodeText, f41) == 424);
			//static_assert(offsetof(fox::ui::ModelNodeText, f42) == 432);
			//static_assert(offsetof(fox::ui::ModelNodeText, f43) == 440);

			static_assert(sizeof(ModelNodeText) == 0x1C0);

			struct ModelNodeLine
			{

			};

			struct ModelNodeMesh
			{

			};

			struct Model;

			struct ComponentTraversalInfo;


			struct Model
			{
				struct vtable
				{
					void* (__fastcall* Model)(fox::ui::Model* this_, void*, unsigned int);
					void* (__fastcall* Release)(fox::ui::Model* this_, void*);
					void* (__fastcall* GetComponentInfo)(fox::ui::Model* this_, fox::ui::ModelNode const*);
					void* (__fastcall* GetModelNodeCommon1)(fox::ui::Model* this_, unsigned int);
					void* (__fastcall* GetModelNodeCommon2)(fox::ui::Model* this_, fox::StringId);
					void* (__fastcall* SetupDrawPriority)(fox::ui::Model* this_, char);
					void* (__fastcall* GetType)(fox::ui::Model* this_);
					void* (__fastcall* EnableScaleInheritance)(fox::ui::Model* this_);
					void* (__fastcall* EnableRotationInheritance)(fox::ui::Model* this_);
					void* (__fastcall* EnableTranslationInheritance)(fox::ui::Model* this_);
					void* (__fastcall* EnableColorRGBInheritance)(fox::ui::Model* this_);
					void* (__fastcall* EnableColorAlphaInheritance)(fox::ui::Model* this_);
					void* (__fastcall* DisableScaleInheritance)(fox::ui::Model* this_);
					void* (__fastcall* DisableRotationInheritance)(fox::ui::Model* this_);
					void* (__fastcall* DisableTranslationInheritance)(fox::ui::Model* this_);
					void* (__fastcall* DisableColorRGBInheritance)(fox::ui::Model* this_);
					void* (__fastcall* DisableColorAlphaInheritance)(fox::ui::Model* this_);
					void* (__fastcall* SetTranslate)(fox::ui::Model* this_, Vectormath::Aos::Vector3*);
					void* (__fastcall* SetScale)(fox::ui::Model* this_, Vectormath::Aos::Vector3*);
					void* (__fastcall* SetQuaternion)(fox::ui::Model* this_, Vectormath::Aos::Quat*);
					void* (__fastcall* SetColor)(fox::ui::Model* this_, fox::Color*);
					void* (__fastcall* SetVisible)(fox::ui::Model* this_, bool);
					void* (__fastcall* GetTranslate)(fox::ui::Model* this_);
					void* (__fastcall* GetScale)(fox::ui::Model* this_);
					void* (__fastcall* GetQuaternion)(fox::ui::Model* this_);
					void* (__fastcall* GetColor)(fox::ui::Model* this_);
					void* (__fastcall* GetVisible)(fox::ui::Model* this_);
					void* (__fastcall* IsInitialized)(fox::ui::Model* this_);
					void* (__fastcall* UpdateComponent)(fox::ui::Model* this_, fox::ui::ComponentTraversalInfo*);
					void* (__fastcall* GetWorldTransform)(fox::ui::Model* this_, fox::ui::ModelNode const*, fox::ui::ComponentTraversalInfo*);
					void* (__fastcall* GetDrawPriorityForChild)(fox::ui::Model* this_);
					void* (__fastcall* IsValid)(fox::ui::Model* this_);
				};

				vtable* __vftable;
				char __pad0[24];
				int id;
				char __pad1[156];
				__int64 sceneName;
				fox::gr::Draw2D* draw2d;
				void* unk;
				fox::gr::Packet2DBuffer* packetBuffer;
				char __pad2[24];
			};

			struct WindowInterface;
			struct TriggerPool;
			struct LayoutComponent;
			struct Layout;
			struct GraphState;

			struct ModelNodeCommon
			{
				char __pad0[108];
				std::uint32_t hash;
			};

			struct Animation;

			struct Font
			{
				char __pad0[8];
				fox::gr::Texture* texture;
				char __pad1[64];
			};

			struct RawDaemon
			{

			};

			struct FontManager
			{
				struct FontGroup
				{
					Font* font;
					float width;
					float height;
					float spacing;
					float f3;
					float f4;
					char __pad0[16];
				};
			};
		}

		namespace math
		{
			struct Vector2
			{
				float x;
				float y;
			};
		}

		namespace nio
		{
			struct Object_vtbl;

#pragma pack(push, 4)
			struct Object
			{
				Object_vtbl* __vftable;
				std::uint32_t a2;
				std::uint32_t a3;
				std::uint32_t a4;
			};
#pragma pack(pop)

			template <typename T>
			struct ObjectPtr
			{
				T* ptr;
			};

			struct MpSocket
			{

			};

			struct SteamUdpSocket
			{
				struct SteamUdpAddress
				{
					struct vtable
					{

					};

					vtable* __vftable;
					steam_id remote;
				};
			};

			namespace impl
			{
				struct SppSocketImpl_mgo
				{
					char __pad1[248];
					int error;
					char __pad2[280];
					__int16 next_seq;
					__int16 last_seq;
					__int8 a2;
					__int8 a3;
					__int8 error_count;
					char __pad3[85];
					void* a1;
					char __pad4[4728];
					int rtt_time;
				};

				struct SppSocketImpl_tpp
				{
					char __pad0[5336];
					int rtt_time;
				};

				union SppSocketImpl
				{
					SppSocketImpl_tpp tpp;
					SppSocketImpl_mgo mgo;
				};
			}
		};

		namespace nt
		{
			struct SessionUserId;

			struct SessionUserId_vtbl
			{

			};

			struct SessionUserId
			{
				SessionUserId_vtbl* __vftable;
				void* a1;
				std::uint64_t userId;
			};

			struct SteamSessionUserId;

			struct SteamSessionUserId_vtbl
			{

			};

			struct SteamSessionUserId
			{
				SteamSessionUserId_vtbl* __vftable;
				std::uint8_t type;
				std::uint64_t userId;
			};

			static_assert(sizeof(SteamSessionUserId) == 0x18);

			struct Member;

			struct Member_SessionInterface;

			struct Member_SessionInterface
			{
				struct vtable
				{
					int(__fastcall* GetIndex)(Member_SessionInterface* this_);
					void* (__fastcall* GetIntAnimationController)(Member_SessionInterface* this_);
					void(__fastcall* GetSessionIdString)(Member_SessionInterface* this_, char*, void*);
					void(__fastcall* __destructor)(Member_SessionInterface* this_);
				};

				vtable* __vftable;
			};

			struct Member
			{
				fox::nio::Object object;
				fox::nt::Member_SessionInterface sessionInterface;
				void* a1;
				char index;
				fox::nio::impl::SppSocketImpl* sppSocket;
				void* sPtr2;
				std::uint8_t a5;
				int flags;
				fox::Array<fox::EntityHandle> entityHandles;
				fox::Array<fox::nio::ObjectPtr<fox::nio::MpSocket>> mpSockets;
				void* syncActivator;
				fox::nt::SteamSessionUserId* sessionUserId;
				void* sPtr3;
			};

			static_assert(offsetof(Member, sessionUserId) == 0x70);
			static_assert(sizeof(Member) == 0x80);

			struct Group
			{
				Member** members;
				void* a2;
				void* a3;
				unsigned int size;
			};

			enum SessionNotify_t
			{
				NOTIFY_SESSION_CREATE = 0,
				NOTIFY_SESSION_CLOSE = 1,
				NOTIFY_SESSION_DELETE = 2,
				NOTIFY_UNK3 = 3, // error?/update
				NOTIFY_JOIN_MEMBER = 4,
				NOTIFY_DELETE_MEMBER = 5,
				NOTIFY_COUNT = 6,
			};

			namespace impl
			{
				struct SessionIdle;

				struct SessionIdle_vtbl
				{

				};

				struct SessionIdle_SessionInterface;

				struct SessionIdle_SessionInterface_vtbl
				{

				};

				struct SessionIdle_SessionInterface
				{
					SessionIdle_SessionInterface_vtbl* __vftable;
				};

				struct SessionIdle_MpPortAllocator;

				struct SessionIdle_MpPortAllocator_vtbl
				{

				};

				struct SessionIdle_MpPortAllocator
				{
					SessionIdle_MpPortAllocator_vtbl* __vftable;
				};

				struct SessionIdle
				{
					SessionIdle_vtbl* __vftable;
					void* a2;
					void* a3;
					SessionIdle_MpPortAllocator* mpPortAllocator;
					SessionIdle_SessionInterface sessionInterface;
					SessionIdle* session1;
					void* a7;
					void* a8;
					void* a9;
					fox::nt::Member* hostMember;
					Group memberGroup1;
					Group memberGroup2;
					unsigned int a18_2;
					unsigned int flags;
					char sessionIndex;
					void* a20;
					int a21_1;
					int a21_2;
					void* a22;
					int a23_1;
					int a23_2;
					void* sPtr5;
					void* sPtr6;
					void* sPtr7;
					void* sPtr8;
					void* sPtr9;
					void* a29;
					void* a30;
					void* a31;
					void* a32;
				};

				struct SessionImpl2;

				struct SessionImpl2_vtbl_mgo
				{
					void* (__fastcall* __destructor)(SessionImpl2* this_);
					void(__fastcall* Release)(SessionImpl2* this_);
					unsigned int(__fastcall* GetState)(SessionImpl2* this_);
					Group* (__fastcall* GetPeerMembers)(SessionImpl2* this_);
					void(__fastcall* Initialize)(SessionImpl2* this_, void*);
					void(__fastcall* Terminate)(SessionImpl2* this_);
					Member* (__fastcall* GetLocalMember)(SessionImpl2* this_);
					Member* (__fastcall* GetHostMember)(SessionImpl2* this_);
					Member* (__fastcall* DeleteMember)(SessionImpl2* this_, Member* member);
					unsigned __int64(__fastcall* GetSppTimeInMsec)(SessionImpl2* this_);
					bool(__fastcall* IsEnabled)(SessionImpl2* this_);
					void(__fastcall* Close)(SessionImpl2* this_);
					void(__fastcall* SetHeartBeat)(SessionImpl2* this_, bool, float, float);
					void(__fastcall* PrepareP2PConnection)(SessionImpl2* this_, void* sessionReserveManager);
					void(__fastcall* Start)(SessionImpl2* this_);
					void(__fastcall* EnableAccept)(SessionImpl2* this_, bool);
					void(__fastcall* DisconnectPreparingMembers)(SessionImpl2* this_);
					void* (__fastcall* GetP2pConnectionManager)(SessionImpl2* this_);
					void(__fastcall* SetP2pConnectionManager)(SessionImpl2* this_, void*);
					void(__fastcall* FixFrameTime)(SessionImpl2* this_);
					void(__fastcall* Update)(SessionImpl2* this_);
					unsigned __int64(__fastcall* GetResult)(SessionImpl2* this_);
					void(__fastcall* InitAcceptedMember)(SessionImpl2* this_, void*, void*);
					Group* (__fastcall* GetAllMembers)(SessionImpl2* this_);
				};

				struct SessionImpl2_vtbl_tpp
				{
					void* (__fastcall* __destructor)(SessionImpl2* this_);
					void(__fastcall* Release)(SessionImpl2* this_);
					unsigned int(__fastcall* GetState)(SessionImpl2* this_);
					Group* (__fastcall* GetPeerMembers)(SessionImpl2* this_);
					void(__fastcall* Initialize)(SessionImpl2* this_, void*);
					void(__fastcall* Terminate)(SessionImpl2* this_);
					Member* (__fastcall* GetLocalMember)(SessionImpl2* this_);
					Member* (__fastcall* GetHostMember)(SessionImpl2* this_);
					Member* (__fastcall* DeleteMember)(SessionImpl2* this_, Member* member);
					unsigned __int64(__fastcall* GetSppTimeInMsec)(SessionImpl2* this_);
					bool(__fastcall* IsEnabled)(SessionImpl2* this_);
					void(__fastcall* SetHeartBeat)(SessionImpl2* this_, bool, float, float);
					void(__fastcall* PrepareP2PConnection)(SessionImpl2* this_, void* sessionReserveManager);
					void(__fastcall* Start)(SessionImpl2* this_);
					void(__fastcall* EnableAccept)(SessionImpl2* this_, bool);
					void(__fastcall* DisconnectPreparingMembers)(SessionImpl2* this_);
					void(__fastcall* FixFrameTime)(SessionImpl2* this_);
					void(__fastcall* Update)(SessionImpl2* this_);
					void* (__fastcall* GetP2pConnectionManager)(SessionImpl2* this_);
					unsigned __int64(__fastcall* GetResult)(SessionImpl2* this_);
					void(__fastcall* InitAcceptedMember)(SessionImpl2* this_, void*, void*);
					Group* (__fastcall* GetAllMembers)(SessionImpl2* this_);
				};

				union SessionImpl2_vtbl
				{
					SessionImpl2_vtbl_tpp tpp;
					SessionImpl2_vtbl_mgo mgo;
				};

				struct SessionImpl2_SessionInterface;

				struct SessionImpl2_SessionInterface_vtbl
				{
					Member_SessionInterface* (__fastcall* GetLocalMemberInterface)(SessionImpl2_SessionInterface* this_);
					Member_SessionInterface* (__fastcall* GetHostMemberInterface)(SessionImpl2_SessionInterface* this_);
					Member_SessionInterface* (__fastcall* GetMemberInterfaceAtIndex)(SessionImpl2_SessionInterface* this_, int);
					int(__fastcall* GetMemberCount)(SessionImpl2_SessionInterface* this_);
					unsigned int(__fastcall* GetOriginalValueCount)(SessionImpl2_SessionInterface* this_);
					bool(__fastcall* IsHost)(SessionImpl2_SessionInterface* this_);
					unsigned int(__fastcall* GetInvisibleFlagOfDebugDraw)(SessionImpl2_SessionInterface* this_);
					bool(__fastcall* IsOnline)(SessionImpl2_SessionInterface* this_);
					void(__fastcall* PrepareP2PConnection)(SessionImpl2_SessionInterface* this_, void*);
					bool(__fastcall* IsConnecting)(SessionImpl2_SessionInterface* this_);
					unsigned int(__fastcall* GetLastError)(SessionImpl2_SessionInterface* this_);
					unsigned int(__fastcall* GetLastConnectionManagerError)(SessionImpl2_SessionInterface* this_);
					unsigned int(__fastcall* __destructor)(SessionImpl2_SessionInterface* this_);
				};

				struct SessionImpl2_SessionInterface
				{
					SessionImpl2_SessionInterface_vtbl* __vftable;
				};

				struct SessionImpl2_MpPortAllocator;

				struct SessionImpl2_MpPortAllocator_vtbl
				{

				};

				struct SessionImpl2_MpPortAllocator
				{
					SessionImpl2_MpPortAllocator_vtbl* __vftable;
				};

				struct SessionImpl2
				{
					fox::nt::impl::SessionImpl2_vtbl* __vftable /*VFT*/;
					void* a2;
					void* a3;
					fox::nt::impl::SessionImpl2_MpPortAllocator* mpPortAllocator;
					fox::nt::impl::SessionImpl2_SessionInterface sessionInterface;
					fox::nt::impl::SessionImpl2* session1;
					void* sPtr1;
					void* sPtr2;
					void* sPtr3;
					fox::nt::Member* hostMember;
					fox::nt::Group allMembers;
					fox::nt::Group peerMembers;
					unsigned int state;
					char sessionIndex;
					void* a4;
					int a5;
					int a6;
					void* a7;
					int a8;
					int a9;
					fox::nio::impl::SppSocketImpl* sppSocket;
					void* sPtr6;
					void* sPtr7;
					void* sPtr8;
					void* sPtr9;
					char a10_1;
					char started;
					char acceptEnabled;
					char a10_4;
					int lastError;
				};

				static_assert(sizeof(SessionImpl2) == 232);

				struct GameSocketImpl;
				struct GameSocketImpl
				{
					struct Peer
					{

					};

					struct vtable
					{
						void(__fastcall* RequestToSend)(GameSocketImpl*, int, const void*, unsigned int);
						void(__fastcall* RequestToSendToMember)(GameSocketImpl*, char, int, const void*, unsigned int);
						void(__fastcall* RequestToSendToMembers)(GameSocketImpl*, int, int, const void*, unsigned int);
						unsigned int(__fastcall* GetPacketCount)(GameSocketImpl*, unsigned int);
						unsigned int(__fastcall* GetPacketSize)(GameSocketImpl*, unsigned int, unsigned int);
						void* (__fastcall* GetPacket)(GameSocketImpl*, unsigned int, unsigned int);
						unsigned char(__fastcall* GetPacketSender)(GameSocketImpl*, unsigned int, unsigned int);
						void(__fastcall* SetInterval)(GameSocketImpl*, int, unsigned char, float);
						void(__fastcall* __destructor)(GameSocketImpl*, char);
					};

					vtable* __vftable;
					short a1;
					short a2;
					char a3_1;
					char a3_2;
					short a4;
					int a5;
					int a6;
					void* buf1;
					void* buf2;
				};
			}

			struct Session : impl::SessionImpl2
			{
			};

			template <typename T>
			struct ObserverContainer
			{

			};

			template <typename T>
			struct ObserverBase
			{
				struct vtable
				{
					void(__fastcall* NotifyImpl)(ObserverBase<T>*, fox::nt::impl::SessionImpl2*, int, unsigned __int8* a4);
					void(__fastcall* __destructor)(ObserverBase<T>*);
				};
				vtable* __vftable;
				ObserverContainer<T>* container;
			};
		}
	
		namespace gm
		{
			enum PeerType
			{
				PEER_LOCAL = 0,
				PEER_HOST = 1,
				PEER_AUTHORITY = 2,
				PEER_ALL = 3,
				PEER_DIRECT = 4,
			};

			enum NetType
			{

			};

			struct GameObjectInterface;
			struct GameObjectFactoryInterface;

			struct GameObjectMessageSystem
			{
				struct vtable
				{
					void(__fastcall* __destructor)(GameObjectMessageSystem*);
					void(__fastcall* SendSignal)(GameObjectMessageSystem*, int* result, __int64 objectId, int peerType, const void* buffer, unsigned __int16 size, int a7, char memberIndex);
					void(__fastcall* SendToSessionId)(GameObjectMessageSystem*);
					void(__fastcall* SerialProcessSignals)(GameObjectMessageSystem*);
				};

				vtable* __vftable;
			};

			namespace impl
			{
				struct GameObjectMessageSystemImpl
				{
					struct vtable
					{

					};

					vtable* __vftable;
					fox::nt::ObserverBase<fox::nt::Session> sessionObserver;
					GameObjectMessageSystem messageSystem;
					fox::nt::impl::GameSocketImpl* sockets[3];
					char __pad1[104];
					Mutex mutex;
					char __pad2[184];
				};

				struct GameObjectExecutionImpl;
				struct GameObjectExecutionImpl
				{
					struct vtable
					{
						void(__fastcall* SetExecutionPriority)(fox::gm::impl::GameObjectExecutionImpl*);
						void(__fastcall* GetExecutionPriority)(fox::gm::impl::GameObjectExecutionImpl*);
						void(__fastcall* ProcessSignal)(fox::gm::impl::GameObjectExecutionImpl*);
						void(__fastcall* IsParallel)(fox::gm::impl::GameObjectExecutionImpl*);
						void(__fastcall* __destructor)(fox::gm::impl::GameObjectExecutionImpl*);
					};

					vtable* __vftable;
					unsigned int a1;
					unsigned int a2;
					unsigned int a3;
					unsigned int a4;
					unsigned short a5_1;
					unsigned short a5_2;
					unsigned int a6;
					fox::gm::GameObjectInterface* gameObjectInterface;
					void* unk1;
					unsigned a7;
					char __pad1[4];
				};


				struct GameObjectFactoryControllerImpl;
				struct GameObjectFactoryControllerImpl
				{
					struct vtable
					{
						void(__fastcall* GetFactoryForTypeId)(fox::gm::impl::GameObjectFactoryControllerImpl*);
						void(__fastcall* GetFactoryWithTypeIndex)(fox::gm::impl::GameObjectFactoryControllerImpl*);
						void(__fastcall* __destructor)(fox::gm::impl::GameObjectFactoryControllerImpl*);
					};

					vtable* __vftable;
					fox::gm::GameObjectFactoryInterface* factories[128];
				};


				struct GameObjectImpl
				{
					struct vtable
					{

					};

					vtable* __vftable;
					char __pad0[24];
					fox::gm::GameObjectInterface* gameObjectInterface;
					char __pad1[24];
				};

				struct GameObjectTypeCollector
				{
					struct vtable
					{

					};

					vtable* __vftable;
					fox::gm::impl::GameObjectImpl* gameObject;
					char __pad0[56];
				};

				struct GameObjectControllerImpl;
				struct GameObjectControllerImpl
				{
					struct vtable
					{
						void(__fastcall* GameObjectTypeWasRegistered)(fox::gm::impl::GameObjectControllerImpl*, unsigned int, fox::StringId);
						void(__fastcall* AddGameObjectImpl)(fox::gm::impl::GameObjectControllerImpl*);
						void(__fastcall* AddGameObjectLocator)(fox::gm::impl::GameObjectControllerImpl*);
						void(__fastcall* DeleteGameObject)(fox::gm::impl::GameObjectControllerImpl*);
						fox::gm::GameObjectInterface* (__fastcall* GetGameObjectWithId)(fox::gm::impl::GameObjectControllerImpl*);
						void(__fastcall* GetGameObjectIdWithInstanceName)(fox::gm::impl::GameObjectControllerImpl*);
						void(__fastcall* Update)(fox::gm::impl::GameObjectControllerImpl*);
						void(__fastcall* GetGameObjectLocatorCountToBeAdded)(fox::gm::impl::GameObjectControllerImpl*);
						void(__fastcall* __destructor)(fox::gm::impl::GameObjectControllerImpl*);
					};

					vtable* __vftable;
					fox::MessageBox2* messageBox;
					fox::MessageBox2DelegateInterface* messageBox2DelegateInterface;
					fox::QuarkRecreationCallbackInterface* quarkRecreationCallbackInterface;
					__int64 unk1;
					__int64 unk2;
					fox::gm::impl::GameObjectTypeCollector* gameObjectTypeCollectors[128];
					fox::concurrent::impl::QueueBase gameObjectLocatorQueue;
				};

			}

			struct GameObjectFactoryInterface
			{
				struct vtable
				{
					void(__fastcall* __destructor)(fox::gm::GameObjectFactoryInterface*);
					void(__fastcall* GetGameObjectTypeId)(fox::gm::GameObjectFactoryInterface*);
					void(__fastcall* GetGameObjectTypeIndex)(fox::gm::GameObjectFactoryInterface*);
					fox::gm::GameObjectInterface* (__fastcall* CreateGameObject)(fox::gm::GameObjectFactoryInterface*);
					void(__fastcall* DeleteGameObject)(fox::gm::GameObjectFactoryInterface*);
				};

				vtable* __vftable;
			};

			struct GameObjectInterface
			{
				struct ExecutionDesc
				{
					unsigned short a1;
					unsigned int a2;
					unsigned int a3;
					unsigned int a4;
					char __pad0[8];
				};

				struct unk1
				{
					char __pad0[40];
					fox::gm::impl::GameObjectExecutionImpl* execution;
				};

				struct vtable
				{
					void(__fastcall* AllocResources)(fox::gm::GameObjectInterface*);
					void(__fastcall* FreeResources)(fox::gm::GameObjectInterface*);
					void(__fastcall* meth_003)(fox::gm::GameObjectInterface*);
					void(__fastcall* TerminateGameObject)(fox::gm::GameObjectInterface*);
					void(__fastcall* Activate)(fox::gm::GameObjectInterface*);
					void(__fastcall* Deactivate)(fox::gm::GameObjectInterface*);
					void(__fastcall* AddLocators)(fox::gm::GameObjectInterface*);
					void(__fastcall* RemoveLocators)(fox::gm::GameObjectInterface*);
					void(__fastcall* GetExecutionDesc)(fox::gm::GameObjectInterface*);
					void(__fastcall* ExecuteSerially)(fox::gm::GameObjectInterface*);
					void(__fastcall* ExecuteInParallel)(fox::gm::GameObjectInterface*);
					void(__fastcall* ExecuteLast)(fox::gm::GameObjectInterface*);
					void(__fastcall* ExecuteAtInactive)(fox::gm::GameObjectInterface*);
					void(__fastcall* meth_014)(fox::gm::GameObjectInterface*);
					void(__fastcall* ProcessSignal)(fox::gm::GameObjectInterface*);
					void(__fastcall* meth_0016)(fox::gm::GameObjectInterface*);
					void(__fastcall* ProcessLuaCommand)(fox::gm::GameObjectInterface*);
					void(__fastcall* GetGameObjectIdWithName)(fox::gm::GameObjectInterface*);
					void(__fastcall* GetNameIdWithGameObjectId)(fox::gm::GameObjectInterface*);
					void(__fastcall* GetNameStringIdWithGameObjectId)(fox::gm::GameObjectInterface*);
					void(__fastcall* meth_021)(fox::gm::GameObjectInterface*);
					void(__fastcall* meth_022)(fox::gm::GameObjectInterface*);
					void(__fastcall* meth_023)(fox::gm::GameObjectInterface*);
					void(__fastcall* __destructor)(fox::gm::GameObjectInterface*);
				};

				vtable* __vftable;
				unk1* unk1;
				char __pad0[184];
			};
		}

		namespace ncl
		{
#pragma pack(push, 8)
			struct NclMessageBase
			{
				struct vtable
				{
					void(__fastcall* __destructor)(fox::ncl::NclMessageBase*);
				};

				vtable* __vftable;
				fox::String msgid;
				int a1;
				int a2;
				fox::String crypto_type;
				fox::String str4;
			};

			struct NclJsonMessage
			{
				struct vtable
				{
					void(__fastcall* __destructor)(fox::ncl::NclJsonMessage*);
					__int64(__fastcall* Serialize)(fox::ncl::NclJsonMessage*, fox::Buffer*);
					__int64(__fastcall* Deserialize)(fox::ncl::NclJsonMessage*, fox::Buffer*);
					char(__fastcall* Pack)(fox::ncl::NclJsonMessage*);
					char(__fastcall* Unpack)(fox::ncl::NclJsonMessage*);
					int(__fastcall* meth_06)(fox::ncl::NclJsonMessage*);
				};

				vtable* __vftable;
				Json::Value json;
				fox::ncl::NclMessageBase base;
			};

			struct NclJsonMessageBase
			{
				struct vtable
				{
					void(__fastcall* __destructor)(fox::ncl::NclJsonMessageBase*);
				};

				vtable* __vftable;
				int rqid;
				NclJsonMessage jsonMessage;
			};

			struct NclJsonMessageResult : NclJsonMessageBase
			{
				struct vtable
				{

				};

				vtable* __vftable;
				fox::String result;
			};

			struct NclJsonMessageResult__shifted0x10
			{
				struct vtable
				{

				};

				NclJsonMessage jsonMessage;
				vtable* __vftable;
				fox::String result;
			};
#pragma pack(pop)

#pragma pack(push, 1)
			struct NclDaemon
			{
				char __pad0[56];
				char isLogin;
				char __pad1[7];
				fox::SharedString sessionKey;
				unsigned char key[16];
				char __pad2[96];
				fox::String ip1;
				fox::String ip2;
			};
#pragma pack(pop)

			static_assert(offsetof(NclDaemon, key) == 72);

			struct NclCryptBlowfish
			{
				struct vtable
				{

				};

				fox::ncl::NclCryptBlowfish::vtable* __vftable;
				char __pad0[4168];
			};

#pragma pack(push, 1)
			struct NclHttpCodec
			{
				struct vtable
				{
					void(__fastcall* __destructor)(fox::ncl::NclHttpCodec*);
					void(__fastcall* Release)(fox::ncl::NclHttpCodec*);
					void(__fastcall* SetMessage)(fox::ncl::NclHttpCodec*, fox::ncl::NclJsonMessageBase*, fox::ncl::NclJsonMessageResult*);
					void(__fastcall* Post)(fox::ncl::NclHttpCodec*);
				};

				fox::ncl::NclHttpCodec::vtable* __vftable;
				void* ptr1;
				fox::ncl::NclCryptBlowfish* blow;
				fox::ncl::NclJsonMessageBase* messageBase;
				fox::ncl::NclJsonMessageResult* messageResult;
				void* ptr5;
				void* callback;
				void* ptr7;
				void* ptr8;
				char a1;
				__int16 a2;
				char __pad0[5];
				void* ptr10;
				int a3;
				char __pad1[4];
				void* ptr12;
			};
#pragma pack(pop)
		}
	}

	union Session
	{
		fox::nt::impl::SessionImpl2* SessionImpl2;
		fox::nt::impl::SessionIdle* sessionIdle;
	};

	namespace tpp::ui
	{
		namespace menu
		{
			namespace impl
			{
				struct MbDvcSoundControllerImpl;

				struct MbDvcSoundControllerImpl
				{
					struct vtable
					{
						void(__fastcall* __destructor)(tpp::ui::menu::impl::MbDvcSoundControllerImpl*);
						void(__fastcall* Update)(tpp::ui::menu::impl::MbDvcSoundControllerImpl*);
						void(__fastcall* Stop)(tpp::ui::menu::impl::MbDvcSoundControllerImpl*);
						void(__fastcall* RequestVoice)(tpp::ui::menu::impl::MbDvcSoundControllerImpl*, unsigned __int8 a2);
						void(__fastcall* CancelRequestVoice)(tpp::ui::menu::impl::MbDvcSoundControllerImpl*);
						void(__fastcall* SetDelayTime)(tpp::ui::menu::impl::MbDvcSoundControllerImpl*);
						void(__fastcall* SetMbDvcOpen)(tpp::ui::menu::impl::MbDvcSoundControllerImpl*);
						void(__fastcall* SetVoiceInvalid)(tpp::ui::menu::impl::MbDvcSoundControllerImpl*);
						void(__fastcall* SetAllVoiceInvalid)(tpp::ui::menu::impl::MbDvcSoundControllerImpl*);
						void(__fastcall* GetVoiceTypeByName)(tpp::ui::menu::impl::MbDvcSoundControllerImpl*);
					};

					vtable* __vftable;
				};

				struct MenuSystemImpl
				{
					struct vtable
					{
						void(__fastcall* meth_000)(MenuSystemImpl*);
						void(__fastcall* meth_001)(MenuSystemImpl*);
						void(__fastcall* meth_002)(MenuSystemImpl*);
						void(__fastcall* meth_003)(MenuSystemImpl*);
						void(__fastcall* meth_004)(MenuSystemImpl*);
						void(__fastcall* meth_005)(MenuSystemImpl*);
						void(__fastcall* meth_006)(MenuSystemImpl*);
						void(__fastcall* meth_007)(MenuSystemImpl*);
						void(__fastcall* meth_008)(MenuSystemImpl*);
						void(__fastcall* meth_009)(MenuSystemImpl*);
						void(__fastcall* meth_010)(MenuSystemImpl*);
						void(__fastcall* meth_011)(MenuSystemImpl*);
						void(__fastcall* meth_012)(MenuSystemImpl*);
						void(__fastcall* meth_013)(MenuSystemImpl*);
						void(__fastcall* meth_014)(MenuSystemImpl*);
						void(__fastcall* meth_015)(MenuSystemImpl*);
						void(__fastcall* meth_016)(MenuSystemImpl*);
						void(__fastcall* meth_017)(MenuSystemImpl*);
						void(__fastcall* meth_018)(MenuSystemImpl*);
						void(__fastcall* meth_019)(MenuSystemImpl*);
						void(__fastcall* meth_020)(MenuSystemImpl*);
						void(__fastcall* meth_021)(MenuSystemImpl*);
						void(__fastcall* meth_022)(MenuSystemImpl*);
						void(__fastcall* meth_023)(MenuSystemImpl*);
						void(__fastcall* meth_024)(MenuSystemImpl*);
						void(__fastcall* meth_025)(MenuSystemImpl*);
						void(__fastcall* meth_026)(MenuSystemImpl*);
						void(__fastcall* meth_027)(MenuSystemImpl*);
						void(__fastcall* meth_028)(MenuSystemImpl*);
						void(__fastcall* meth_029)(MenuSystemImpl*);
						void(__fastcall* meth_030)(MenuSystemImpl*);
						void(__fastcall* meth_031)(MenuSystemImpl*);
						void(__fastcall* meth_032)(MenuSystemImpl*);
						void(__fastcall* meth_033)(MenuSystemImpl*);
						void(__fastcall* meth_034)(MenuSystemImpl*);
						void(__fastcall* meth_035)(MenuSystemImpl*);
						void(__fastcall* meth_036)(MenuSystemImpl*);
						void(__fastcall* meth_037)(MenuSystemImpl*);
						void(__fastcall* meth_038)(MenuSystemImpl*);
						void(__fastcall* meth_039)(MenuSystemImpl*);
						void(__fastcall* meth_040)(MenuSystemImpl*);
						void(__fastcall* meth_041)(MenuSystemImpl*);
						void(__fastcall* meth_042)(MenuSystemImpl*);
						void(__fastcall* meth_043)(MenuSystemImpl*);
						void(__fastcall* meth_044)(MenuSystemImpl*);
						void(__fastcall* meth_045)(MenuSystemImpl*);
						void(__fastcall* meth_046)(MenuSystemImpl*);
						void(__fastcall* meth_047)(MenuSystemImpl*);
						void(__fastcall* meth_048)(MenuSystemImpl*);
						void(__fastcall* meth_049)(MenuSystemImpl*);
						void(__fastcall* meth_050)(MenuSystemImpl*);
						void(__fastcall* meth_051)(MenuSystemImpl*);
						void(__fastcall* meth_052)(MenuSystemImpl*);
						void(__fastcall* meth_053)(MenuSystemImpl*);
						void(__fastcall* meth_054)(MenuSystemImpl*);
						void(__fastcall* meth_055)(MenuSystemImpl*);
						void(__fastcall* meth_056)(MenuSystemImpl*);
						void(__fastcall* meth_057)(MenuSystemImpl*);
						void(__fastcall* meth_058)(MenuSystemImpl*);
						void(__fastcall* meth_059)(MenuSystemImpl*);
						void(__fastcall* meth_060)(MenuSystemImpl*);
						void(__fastcall* meth_061)(MenuSystemImpl*);
						void(__fastcall* meth_062)(MenuSystemImpl*);
						void(__fastcall* meth_063)(MenuSystemImpl*);
						void(__fastcall* meth_064)(MenuSystemImpl*);
						void(__fastcall* meth_065)(MenuSystemImpl*);
						void(__fastcall* meth_066)(MenuSystemImpl*);
						void(__fastcall* meth_067)(MenuSystemImpl*);
						void(__fastcall* meth_068)(MenuSystemImpl*);
						void(__fastcall* meth_069)(MenuSystemImpl*);
						void(__fastcall* meth_070)(MenuSystemImpl*);
						void(__fastcall* meth_071)(MenuSystemImpl*);
						void(__fastcall* meth_072)(MenuSystemImpl*);
						void(__fastcall* meth_073)(MenuSystemImpl*);
						void(__fastcall* meth_074)(MenuSystemImpl*);
						void(__fastcall* meth_075)(MenuSystemImpl*);
						void(__fastcall* meth_076)(MenuSystemImpl*);
						void(__fastcall* meth_077)(MenuSystemImpl*);
						void(__fastcall* meth_078)(MenuSystemImpl*);
						void(__fastcall* meth_079)(MenuSystemImpl*);
						void(__fastcall* meth_080)(MenuSystemImpl*);
						void(__fastcall* meth_081)(MenuSystemImpl*);
						void(__fastcall* meth_082)(MenuSystemImpl*);
						void(__fastcall* meth_083)(MenuSystemImpl*);
						void(__fastcall* SetLangChange)(MenuSystemImpl*, char);
						void(__fastcall* meth_085)(MenuSystemImpl*);
						void(__fastcall* meth_086)(MenuSystemImpl*);
						void(__fastcall* meth_087)(MenuSystemImpl*);
						void(__fastcall* meth_088)(MenuSystemImpl*);
						void(__fastcall* meth_089)(MenuSystemImpl*);
						void(__fastcall* meth_090)(MenuSystemImpl*);
						void(__fastcall* meth_091)(MenuSystemImpl*);
						void(__fastcall* meth_092)(MenuSystemImpl*);
						void(__fastcall* meth_093)(MenuSystemImpl*);
						void(__fastcall* meth_094)(MenuSystemImpl*);
						void(__fastcall* meth_095)(MenuSystemImpl*);
						void(__fastcall* meth_096)(MenuSystemImpl*);
						void(__fastcall* meth_097)(MenuSystemImpl*);
						void(__fastcall* meth_098)(MenuSystemImpl*);
						void(__fastcall* meth_099)(MenuSystemImpl*);
						void(__fastcall* meth_100)(MenuSystemImpl*);
					};

					vtable* __vftable;
				};
			}

			struct UiCommonDataManager
			{
				struct vtable
				{

				};

				vtable* __vftable;
				char __pad0[128];
				impl::MbDvcSoundControllerImpl* soundController;

			};

			namespace mbm
			{
				namespace impl
				{
					struct FobMission2CallbackImpl
					{
						char __pad0[176];
						char state;
						char __pad1[11399];
						char currentTab;
						char hasLoadedTab[10];
						char __pad2[0x10000];
					};
				}
			}
		}

		namespace hud
		{
			namespace impl
			{
				struct HudSystemImpl_mgo_unk1
				{
					char __pad0[0x28BD];
					char showScores;
				};

				struct HudSystemImpl_mgo
				{
					struct vtable
					{

					};

					vtable* __vftable;
					char __pad0[0x92B0];
					HudSystemImpl_mgo_unk1 unk1;
				};

				struct HudSystemImpl_tpp
				{
					struct vtable
					{

					};

					vtable* __vftable;
				};

				union HudSystemImpl
				{
					HudSystemImpl_tpp tpp;
					HudSystemImpl_mgo mgo;
				};
			}
		}

		namespace impl
		{
			struct UiSystemImpl
			{
				char __pad0[24];
				tpp::ui::hud::impl::HudSystemImpl* hudSystem;
				char __pad1[48];
				tpp::ui::menu::impl::MenuSystemImpl* menuSystem;
			};
		}
	}

	namespace tpp::mbm
	{
		enum Const
		{
			SECURITY_SOLDIER_EQUIP_CLOSE_RANGE = 0,
			SECURITY_SOLDIER_EQUIP_MIDDLE_RANGE = 1,
			SECURITY_SOLDIER_EQUIP_LONG_RANGE = 2,
			SECURITY_SOLDIER_EQUIP_GRADE_MIN = 1,
			SECURITY_SOLDIER_EQUIP_GRADE_MAX = 15,
			SECURITY_QUANTITY_NONE = 0,
			SECURITY_QUANTITY_SMALL = 1,
			SECURITY_QUANTITY_MIDDLE = 2,
			SECURITY_QUANTITY_LARGE = 3,
			SECTION_WAIT = 0,
			SECTION_COMBAT = 1,
			SECTION_DEVELOP = 2,
			SECTION_BASE_DEV = 3,
			SECTION_SUPPORT = 4,
			SECTION_SPY = 5,
			SECTION_MEDICAL = 6,
			SECTION_SECURITY = 7,
			SECTION_HOSPITAL = 8,
			SECTION_PRISON = 9,
			SECTION_SEPARATION = 10,
			STAFF_SECTION_RANK_G = 0,
			STAFF_SECTION_RANK_F = 1,
			STAFF_SECTION_RANK_E = 2,
			STAFF_SECTION_RANK_D = 3,
			STAFF_SECTION_RANK_C = 4,
			STAFF_SECTION_RANK_B = 5,
			STAFF_SECTION_RANK_A = 6,
			STAFF_SECTION_RANK_S = 7,
			STAFF_SECTION_RANK_SP = 8,
			STAFF_SECTION_RANK_SPP = 9,
			STAFF_SECTION_RANK_COUNT_MAX = 10,
			STAFF_SKILL_ID_NONE = 0,
			STAFF_SKILL_ID_REFLEX = 1,
			STAFF_SKILL_ID_NINJA = 2,
			STAFF_SKILL_ID_ATHLETE = 3,
			STAFF_SKILL_ID_LOCKSMITH = 4,
			STAFF_SKILL_ID_FULTON_EXPERT = 5,
			STAFF_SKILL_ID_QUICK_RELOAD = 6,
			STAFF_SKILL_ID_STUDY = 7,
			STAFF_SKILL_ID_LUCKY = 8,
			STAFF_SKILL_ID_GRAPPLER = 9,
			STAFF_SKILL_ID_BIG_MOUTH = 10,
			STAFF_SKILL_ID_BOTANIST = 11,
			STAFF_SKILL_ID_SCOUT_SNIPER = 13,
			STAFF_SKILL_ID_PHYSICAL_LV1 = 14,
			STAFF_SKILL_ID_PHYSICAL_LV2 = 15,
			STAFF_SKILL_ID_PHYSICAL_LV3 = 16,
			STAFF_SKILL_ID_PRECISION_LV1 = 17,
			STAFF_SKILL_ID_PRECISION_LV2 = 18,
			STAFF_SKILL_ID_PRECISION_LV3 = 19,
			STAFF_SKILL_ID_MEDICAL_LV1 = 20,
			STAFF_SKILL_ID_MEDICAL_LV2 = 21,
			STAFF_SKILL_ID_MEDICAL_LV3 = 22,
			STAFF_SKILL_ID_QUICK_DRAW = 12,
			STAFF_SKILL_ID_GUNSMITH_HUND_GUN = 23,
			STAFF_SKILL_ID_GUNSMITH_SUBMACHINE_GUN = 24,
			STAFF_SKILL_ID_GUNSMITH_ASSULT_RIFLE = 25,
			STAFF_SKILL_ID_GUNSMITH_SHOT_GUN = 26,
			STAFF_SKILL_ID_GUNSMITH_GRENADE_LAUNCHER = 27,
			STAFF_SKILL_ID_GUNSMITH_SNIPER_RIFLE = 28,
			STAFF_SKILL_ID_GUNSMITH_MACHINE_GUN = 29,
			STAFF_SKILL_ID_GUNSMITH_MISSILE = 30,
			STAFF_SKILL_ID_MASTER_GUNSMITH = 31,
			STAFF_SKILL_ID_TRANQ_ENGINEER = 32,
			STAFF_SKILL_ID_SUPPRESSOR_ENGINEER = 33,
			STAFF_SKILL_ID_MISSILE_HOMING_ENGINEER = 34,
			STAFF_SKILL_ID_ZOOLOGIST = 35,
			STAFF_SKILL_ID_SLEEPING_GAS_ENGINEER = 36,
			STAFF_SKILL_ID_ELECTRIC_ENGINEER = 37,
			STAFF_SKILL_ID_ELECTROMAGNETIC_NET_ENGINEER = 38,
			STAFF_SKILL_ID_WATERPROOF_ENGINEER = 39,
			STAFF_SKILL_ID_RADAR_ENGINEER = 40,
			STAFF_SKILL_ID_METAMATERIAL_ENGINEER = 41,
			STAFF_SKILL_ID_DRUG_ENGINEER = 42,
			STAFF_SKILL_ID_MECHANICAL_ENGINEER = 43,
			STAFF_SKILL_ID_MECHATRONICS_ENGINEER = 44,
			STAFF_SKILL_ID_CYBERNETICS_ENGINEER = 45,
			STAFF_SKILL_ID_ROCKET_CONTROL_ENGINEER = 46,
			STAFF_SKILL_ID_ELECTRIC_SPINNING_ENGINEER = 47,
			STAFF_SKILL_ID_MATERIAL_ENGINEER = 48,
			STAFF_SKILL_ID_HAULAGE_ENGINEER = 49,
			STAFF_SKILL_ID_REMOTE_CONTROL_ENGINEER = 50,
			STAFF_SKILL_ID_MONITOR_ENGINEER = 51,
			STAFF_SKILL_ID_ACTIVE_PROTECT_ENGINEER = 52,
			STAFF_SKILL_ID_ATTITUDE_CONTROL_ENGINEER = 53,
			STAFF_SKILL_ID_BIPEDALISM_WEAPON_DEVELOPMENT = 54,
			STAFF_SKILL_ID_TRANSLATE_RUSSIAN = 55,
			STAFF_SKILL_ID_TRANSLATE_AFRIKAANS = 56,
			STAFF_SKILL_ID_TRANSLATE_KIKONGO = 57,
			STAFF_SKILL_ID_TRANSLATE_PASHTO = 58,
			STAFF_SKILL_ID_SURGEON = 59,
			STAFF_SKILL_ID_COUNSELOR = 61,
			STAFF_SKILL_ID_PHYSICIAN = 60,
			STAFF_SKILL_ID_PARASITE_RESEARCHER = 62,
			STAFF_SKILL_ID_TROUBLEMAKER_VIOLENCE = 63,
			STAFF_SKILL_ID_TROUBLEMAKER_INTEMPERATELY = 64,
			STAFF_SKILL_ID_TROUBLEMAKER_HARASSMENT = 65,
			STAFF_SKILL_ID_MOODMAKER = 66,
			STAFF_SKILL_ID_TACTICS_INSTRUCTOR = 67,
			STAFF_SKILL_ID_MB_VICE_COMMANDER = 68,
			STAFF_SKILL_ID_DEFENDER_LV1 = 69,
			STAFF_SKILL_ID_DEFENDER_LV2 = 70,
			STAFF_SKILL_ID_DEFENDER_LV3 = 71,
			STAFF_SKILL_ID_SENTRY_LV1 = 72,
			STAFF_SKILL_ID_SENTRY_LV2 = 73,
			STAFF_SKILL_ID_SENTRY_LV3 = 74,
			STAFF_SKILL_ID_RANGER_LV1 = 75,
			STAFF_SKILL_ID_RANGER_LV2 = 76,
			STAFF_SKILL_ID_RANGER_LV3 = 77,
			STAFF_SKILL_ID_MEDIC_LV1 = 78,
			STAFF_SKILL_ID_MEDIC_LV2 = 79,
			STAFF_SKILL_ID_MEDIC_LV3 = 80,
			STAFF_SKILL_ID_LIQUID_CARBON_MISSILE_ENGINEER_LV1 = 81,
			STAFF_SKILL_ID_LIQUID_CARBON_MISSILE_ENGINEER_LV2 = 82,
			STAFF_SKILL_ID_LIQUID_CARBON_MISSILE_ENGINEER_LV3 = 83,
			STAFF_SKILL_ID_INTERCEPTOR_MISSILE_ENGINEER_LV1 = 84,
			STAFF_SKILL_ID_INTERCEPTOR_MISSILE_ENGINEER_LV2 = 85,
			STAFF_SKILL_ID_INTERCEPTOR_MISSILE_ENGINEER_LV3 = 86,
			STAFF_UNIQUE_TYPE_ID_OCELOT = 249,
			STAFF_UNIQUE_TYPE_ID_MILLER = 250,
			STAFF_UNIQUE_TYPE_ID_QUIET = 251,
			STAFF_UNIQUE_TYPE_ID_HEUY = 252,
			STAFF_UNIQUE_TYPE_ID_CODE_TALKER = 253,
			BIRTHPLACE_NONE = 0,
			BIRTHPLACE_AFGHAN = 1,
			BIRTHPLACE_AFRICA = 2,
			RACE_WHITE = 0,
			RACE_BLOWN = 1,
			RACE_BLACK = 2,
			RACE_YELLOW = 3,
			LANG_NONE = 0,
			LANG_ENG = 1,
			LANG_RUS = 2,
			LANG_PUS = 3,
			LANG_KON = 4,
			LANG_AFR = 5,
			LANG_UZB = 6,
			LANG_KIR = 7,
			LANG_TUK = 8,
			LANG_UKR = 9,
			LANG_TGK = 10,
			LANG_EST = 11,
			LANG_LAV = 12,
			LANG_LIT = 13,
			LANG_KAZ = 14,
			LANG_DER = 15,
			LANG_XHO = 16,
			LANG_SNNA = 17,
			LANG_ZUL = 18,
			LANG_SPA = 19,
			LANG_SOT = 20,
			LANG_TSN = 21,
			LANG_DEU = 22,
			LANG_FRA = 23,
			LANG_POR = 24,
			LANG_LIN = 25,
			LANG_LUA = 26,
			LANG_NBL = 27,
			LANG_ITA = 28,
			LANG_KOR = 29,
			LANG_ZHO = 30,
			LANG_JPN = 31,
			LANG_ARA = 32,
			LANG_NAV = 33,
			REMOVER_REASON_UNKNOWN = 0,
			REMOVER_REASON_WAR = 1,
			REMOVER_REASON_CONDITION_DAMAGED = 2,
			REMOVER_REASON_CONDITION_SICK = 3,
			REMOVER_REASON_CONDITION_PTSD = 4,
			REMOVER_REASON_DEFECTION = 5,
			REMOVER_REASON_VIRUS = 6,
			REMOVER_REASON_BURN = 7,
			REMOVER_REASON_SUICIDE = 8,
			REMOVER_REASON_GUNDOWN = 9,
			REMOVER_REASON_BAN = 10,
			REMOVER_REASON_DISAPPEARANCE = 11,
			SECTION_FUNC_ID_COMBAT_DEPLOY = 0,
			SECTION_FUNC_ID_COMBAT_DEFENCE = 1,
			SECTION_FUNC_ID_DEVELOP_WEAPON = 5,
			SECTION_FUNC_ID_DEVELOP_ITEM = 5,
			SECTION_FUNC_ID_DEVELOP_HELI = 6,
			SECTION_FUNC_ID_DEVELOP_QUIET = 7,
			SECTION_FUNC_ID_DEVELOP_D_DOG = 8,
			SECTION_FUNC_ID_DEVELOP_D_HORSE = 9,
			SECTION_FUNC_ID_DEVELOP_D_WALKER = 10,
			SECTION_FUNC_ID_DEVELOP_BATTLE_GEAR = 11,
			SECTION_FUNC_ID_DEVELOP_SECURITY_DEVICE = 12,
			SECTION_FUNC_ID_BASE_DEV_RESOURCE_MINING = 13,
			SECTION_FUNC_ID_BASE_DEV_RESOURCE_PROCESSING = 14,
			SECTION_FUNC_ID_BASE_DEV_PLATFORM_EXTENTION = 15,
			SECTION_FUNC_ID_BASE_DEV_FOB_CONSTRUCT = 16,
			SECTION_FUNC_ID_BASE_DEV_NUCLEAR_DEVELOP = 17,
			SECTION_FUNC_ID_SUPPORT_FULTON = 18,
			SECTION_FUNC_ID_SUPPORT_SUPPLY = 19,
			SECTION_FUNC_ID_SUPPORT_BATTLE = 20,
			SECTION_FUNC_ID_SUPPORT_STRIKE = 21,
			SECTION_FUNC_ID_SUPPORT_SMOKE = 22,
			SECTION_FUNC_ID_SUPPORT_SLEEP_GAS = 23,
			SECTION_FUNC_ID_SUPPORT_CHAFF = 24,
			SECTION_FUNC_ID_SUPPORT_WEATHER = 25,
			SECTION_FUNC_ID_SUPPORT_RUSSIAN_TRANSLATE = 26,
			SECTION_FUNC_ID_SUPPORT_PASHTO_TRANSLATE = 27,
			SECTION_FUNC_ID_SUPPORT_AFRIKAANS_TRANSLATE = 28,
			SECTION_FUNC_ID_SUPPORT_KIKONGO_TRANSLATE = 29,
			SECTION_FUNC_ID_SPY_MISSION_INFO_COLLECTING = 30,
			SECTION_FUNC_ID_SPY_ENEMY_SEARCH = 31,
			SECTION_FUNC_ID_SPY_RESOURCE_SEARCH = 32,
			SECTION_FUNC_ID_SPY_WEATHER_INFO = 33,
			SECTION_FUNC_ID_SPY_SECURITY_INFO = 36,
			SECTION_FUNC_ID_MEDICAL_STAFF_EMERGENCY = 34,
			SECTION_FUNC_ID_MEDICAL_STAFF_TREATMENT = 35,
			SECTION_FUNC_ID_MEDICAL_ANTI_REFLEX = 37,
			SECTION_FUNC_ID_SECURITY_BASE_DEFENCE_STAFF = 2,
			SECTION_FUNC_ID_SECURITY_BASE_DEFENCE_MACHINE = 3,
			SECTION_FUNC_ID_SECURITY_BASE_BLOCKADE = 4,
			SECTION_FUNC_RANK_NONE = 0,
			SECTION_FUNC_RANK_F = 1,
			SECTION_FUNC_RANK_E = 2,
			SECTION_FUNC_RANK_D = 3,
			SECTION_FUNC_RANK_C = 4,
			SECTION_FUNC_RANK_B = 5,
			SECTION_FUNC_RANK_A = 6,
			SECTION_FUNC_RANK_S = 7,
			STAFF_INIT_ENMITY_LV_NONE = 0,
			STAFF_INIT_ENMITY_LV_10 = 1,
			STAFF_INIT_ENMITY_LV_20 = 2,
			STAFF_INIT_ENMITY_LV_30 = 3,
			STAFF_INIT_ENMITY_LV_40 = 4,
			STAFF_INIT_ENMITY_LV_50 = 5,
			STAFF_INIT_ENMITY_LV_60 = 6,
			STAFF_INIT_ENMITY_LV_70 = 7,
			STAFF_INIT_ENMITY_LV_80 = 8,
			STAFF_INIT_ENMITY_LV_90 = 9,
			STAFF_INIT_ENMITY_LV_100 = 10,
			STAFF_INIT_ENMITY_LV_MAX = 10,
			RESOURCE_ID_4WD_EAST = 13,
			RESOURCE_ID_4WD_WEST = 14,
			RESOURCE_ID_TRUCK_EAST = 15,
			RESOURCE_ID_TRUCK_WEST = 16,
			RESOURCE_ID_ARMORED_VEHICLE_EAST = 17,
			RESOURCE_ID_ARMORED_VEHICLE_WEST = 18,
			RESOURCE_ID_ARMORED_VEHICLE_EAST_ROCKET = 19,
			RESOURCE_ID_WHEELED_ARMORED_VEHICLE_WEST = 20,
			RESOURCE_ID_TANK_EAST = 21,
			RESOURCE_ID_TANK_WEST = 22,
			RESOURCE_ID_PARASITE_FOG = 30,
			RESOURCE_ID_PARASITE_CAMOFLA = 31,
			RESOURCE_ID_PARASITE_CURING = 32,
			RESOURCE_ID_EMPLACEMENT_GUN_EAST = 34,
			RESOURCE_ID_EMPLACEMENT_GUN_WEST = 35,
			RESOURCE_ID_MORTAR_NORMAL = 36,
			RESOURCE_ID_ANTI_AIR_GATLING_GUN_EAST = 37,
			RESOURCE_ID_ANTI_AIR_GATLING_GUN_WEST = 38,
			CONTAINER_VISUAL_WHITE = 0,
			CONTAINER_VISUAL_RED = 1,
			CONTAINER_VISUAL_YELLOW = 2,
			CONTAINER_VISUAL_INVALID = 255,
			PHOTO_NONE = 0xFFFF,
			PHOTO_1000 = 9,
			PHOTO_1001 = 16,
			PHOTO_1002 = 11,
			PHOTO_1003 = 12,
			PHOTO_1004 = 13,
			PHOTO_1005 = 14,
			PHOTO_1006 = 15,
			PHOTO_1007 = 10,
			PHOTO_1008 = 17,
			PHOTO_1009 = 18,
			PHOTO_1010 = 438,
			DESIGN_NONE = 0xFFFF,
			DESIGN_2000 = 19,
			DESIGN_2001 = 20,
			DESIGN_2002 = 21,
			DESIGN_2003 = 22,
			DESIGN_2027 = 439,
			DESIGN_2005 = 23,
			DESIGN_2006 = 24,
			DESIGN_2007 = 25,
			DESIGN_2008 = 26,
			DESIGN_2009 = 27,
			DESIGN_2010 = 28,
			DESIGN_2011 = 29,
			DESIGN_2012 = 30,
			DESIGN_2013 = 31,
			DESIGN_2014 = 32,
			DESIGN_2015 = 33,
			DESIGN_2016 = 34,
			DESIGN_2017 = 35,
			DESIGN_2018 = 36,
			DESIGN_2025 = 37,
			DESIGN_2019 = 38,
			DESIGN_2020 = 39,
			DESIGN_2026 = 40,
			DESIGN_2021 = 41,
			DESIGN_2022 = 42,
			DESIGN_2023 = 43,
			DESIGN_2024 = 44,
			DESIGN_3013 = 45,
			DESIGN_3001 = 46,
			DESIGN_3020 = 440,
			DESIGN_3011 = 47,
			DESIGN_3012 = 48,
			DESIGN_3006 = 49,
			DESIGN_3005 = 50,
			DESIGN_3000 = 51,
			DESIGN_3003 = 52,
			DESIGN_3009 = 53,
			DESIGN_3002 = 54,
			DESIGN_3014 = 441,
			DESIGN_3007 = 55,
			DESIGN_3015 = 442,
			DESIGN_3008 = 56,
			DESIGN_3016 = 443,
			DESIGN_3017 = 444,
			DESIGN_3018 = 445,
			DESIGN_3019 = 446,
			DESIGN_3010 = 57,
			EXTRA_NONE = 0xFFFF,
			EXTRA_4000 = 58,
			EXTRA_4001 = 59,
			EXTRA_4002 = 60,
			EXTRA_4003 = 61,
			EXTRA_4004 = 62,
			EXTRA_4005 = 63,
			EXTRA_4006 = 64,
			EXTRA_4007 = 65,
			EXTRA_4008 = 66,
			EXTRA_4009 = 67,
			EXTRA_4010 = 68,
			EXTRA_4011 = 69,
			EXTRA_4012 = 70,
			EXTRA_4013 = 71,
			EXTRA_4014 = 72,
			EXTRA_4015 = 73,
			EXTRA_4016 = 74,
			EXTRA_4017 = 75,
			EXTRA_4018 = 76,
			EXTRA_4019 = 77,
			EXTRA_4020 = 78,
			EXTRA_4021 = 79,
			EXTRA_4022 = 80,
			EXTRA_4023 = 81,
			EXTRA_4024 = 82,
			EXTRA_4025 = 83,
			EXTRA_4026 = 447,
			EXTRA_4027 = 448,
			EXTRA_4028 = 449,
			EXTRA_5000 = 450,
			EXTRA_5001 = 451,
			EXTRA_5002 = 452,
			EXTRA_6000 = 453,
			ANIMAL_NONE = 0xFFFF,
			ANIMAL_100 = 84,
			ANIMAL_110 = 85,
			ANIMAL_120 = 86,
			ANIMAL_130 = 87,
			ANIMAL_140 = 88,
			ANIMAL_200 = 89,
			ANIMAL_210 = 90,
			ANIMAL_220 = 91,
			ANIMAL_300 = 92,
			ANIMAL_400 = 93,
			ANIMAL_410 = 94,
			ANIMAL_500 = 95,
			ANIMAL_510 = 96,
			ANIMAL_600 = 97,
			ANIMAL_610 = 98,
			ANIMAL_620 = 99,
			ANIMAL_700 = 100,
			ANIMAL_720 = 101,
			ANIMAL_730 = 102,
			ANIMAL_800 = 103,
			ANIMAL_810 = 104,
			ANIMAL_900 = 105,
			ANIMAL_1000 = 106,
			ANIMAL_1100 = 107,
			ANIMAL_1200 = 108,
			ANIMAL_1210 = 109,
			ANIMAL_1220 = 110,
			ANIMAL_1300 = 111,
			ANIMAL_1310 = 112,
			ANIMAL_1400 = 113,
			ANIMAL_1401 = 114,
			ANIMAL_1402 = 115,
			ANIMAL_1403 = 116,
			ANIMAL_1410 = 117,
			ANIMAL_1420 = 118,
			ANIMAL_1430 = 119,
			ANIMAL_1500 = 120,
			ANIMAL_1600 = 121,
			ANIMAL_1700 = 122,
			ANIMAL_1710 = 123,
			ANIMAL_1800 = 124,
			ANIMAL_1900 = 125,
			ANIMAL_1901 = 126,
			ANIMAL_1902 = 127,
			ANIMAL_1903 = 128,
			ANIMAL_1910 = 129,
			ANIMAL_1911 = 130,
			ANIMAL_1912 = 131,
			ANIMAL_1913 = 132,
			ANIMAL_1920 = 133,
			ANIMAL_1921 = 134,
			ANIMAL_1922 = 135,
			ANIMAL_1923 = 136,
			ANIMAL_1930 = 137,
			ANIMAL_1931 = 138,
			ANIMAL_1932 = 139,
			ANIMAL_1933 = 140,
			ANIMAL_1940 = 141,
			ANIMAL_1941 = 142,
			ANIMAL_1942 = 143,
			ANIMAL_1943 = 144,
			ANIMAL_1944 = 145,
			ANIMAL_1945 = 146,
			ANIMAL_1946 = 147,
			ANIMAL_1947 = 148,
			ANIMAL_1950 = 149,
			ANIMAL_1951 = 150,
			ANIMAL_1952 = 151,
			ANIMAL_1953 = 152,
			ANIMAL_1954 = 153,
			ANIMAL_1955 = 154,
			ANIMAL_1956 = 155,
			ANIMAL_1957 = 156,
			ANIMAL_1960 = 157,
			ANIMAL_1961 = 158,
			ANIMAL_1962 = 159,
			ANIMAL_1963 = 160,
			ANIMAL_1964 = 161,
			ANIMAL_1965 = 162,
			ANIMAL_1966 = 163,
			ANIMAL_1967 = 164,
			ANIMAL_1970 = 165,
			ANIMAL_1971 = 166,
			ANIMAL_1972 = 167,
			ANIMAL_1973 = 168,
			ANIMAL_1974 = 169,
			ANIMAL_1975 = 170,
			ANIMAL_1976 = 171,
			ANIMAL_1977 = 172,
			ANIMAL_2000 = 173,
			ANIMAL_2010 = 174,
			ANIMAL_2100 = 175,
			ANIMAL_2200 = 176,
			ANIMAL_2210 = 177,
			ANIMAL_2220 = 178,
			ANIMAL_2230 = 179,
			ANIMAL_2240 = 180,
			ANIMAL_2241 = 181,
			ANIMAL_2250 = 182,
			ANIMAL_GROUP_1400 = 113,
			ANIMAL_GROUP_1900 = 125,
			ANIMAL_GROUP_1920 = 133,
			ANIMAL_GROUP_1940 = 141,
			ANIMAL_GROUP_1960 = 157,
			ANIMAL_TYPE_UNKNOWN = 0,
			ANIMAL_TYPE_DOG = 1,
			ANIMAL_TYPE_HORSE = 2,
			ANIMAL_TYPE_FROG = 3,
			ANIMAL_TYPE_TURTLE = 4,
			ANIMAL_TYPE_FOX = 5,
			ANIMAL_TYPE_BEAR = 6,
			ANIMAL_TYPE_BAT = 7,
			ANIMAL_TYPE_LIZARD = 8,
			ANIMAL_TYPE_SCORPION = 9,
			ANIMAL_TYPE_CAT = 10,
			ANIMAL_TYPE_PANGOLIN = 11,
			ANIMAL_TYPE_BIRD = 12,
			ANIMAL_TYPE_MOUSE = 13,
			ANIMAL_TYPE_HYRAX = 14,
			ANIMAL_TYPE_HUMAN = 15,
			ANIMAL_TYPE_SNAKE = 16,
			ANIMAL_TYPE_MONGOOSE = 17,
			ANIMAL_TYPE_GOAT = 18,
			ANIMAL_TYPE_GECKO = 19,
			ANIMAL_TYPE_RATERU = 20,
			CLUSTER_CATEGORY_COMMAND = 0,
			CLUSTER_CATEGORY_COMBAT = 1,
			CLUSTER_CATEGORY_DEVELOP = 2,
			CLUSTER_CATEGORY_BASE_DEV = 3,
			CLUSTER_CATEGORY_SUPPORT = 4,
			CLUSTER_CATEGORY_SPY = 5,
			CLUSTER_CATEGORY_MEDICAL = 6,
			ANIMAL_RARE_N = 0,
			ANIMAL_RARE_NR = 1,
			ANIMAL_RARE_R = 2,
			ANIMAL_RARE_SR = 3,
			ANIMAL_RARE_SSR = 4,
			DEPLOY_MISSION_CATEGORY_COMBAT1_PERSON_GUARD = 0,
			DEPLOY_MISSION_CATEGORY_COMBAT2_BASE_DEFENSE = 1,
			DEPLOY_MISSION_CATEGORY_COMBAT3_UNIT_EXCLUSION = 2,
			DEPLOY_MISSION_CATEGORY_COMBAT4_BASE_CONTROL = 3,
			DEPLOY_MISSION_CATEGORY_DEVELOP1_TECHNICAL_GRANT = 4,
			DEPLOY_MISSION_CATEGORY_DEVELOP2_JOINT_DEVELOP = 5,
			DEPLOY_MISSION_CATEGORY_BASE_DEV1_INSHORE_DEVELOP = 6,
			DEPLOY_MISSION_CATEGORY_BASE_DEV2_PELAGIC_DEVELOP = 7,
			DEPLOY_MISSION_CATEGORY_SUPPORT1_RECONSTRUCTION_AID = 8,
			DEPLOY_MISSION_CATEGORY_SUPPORT2_TRANSPORTATION = 9,
			DEPLOY_MISSION_CATEGORY_SPY1_RECRUIT = 10,
			DEPLOY_MISSION_CATEGORY_SPY2_SCOUT = 11,
			DEPLOY_MISSION_CATEGORY_MEDICAL1_MEDICAL_SUPPORT = 12,
			DEPLOY_MISSION_CATEGORY_MEDICAL2_PLANT_COLLECTION = 13,
			DEPLOY_MISSION_ID_SEQ_1001 = 1,
			DEPLOY_MISSION_ID_SEQ_1002 = 2,
			DEPLOY_MISSION_ID_SEQ_1003 = 3,
			DEPLOY_MISSION_ID_SEQ_1004 = 4,
			DEPLOY_MISSION_ID_SEQ_1005 = 5,
			DEPLOY_MISSION_ID_SEQ_1006 = 6,
			DEPLOY_MISSION_ID_SEQ_1007 = 7,
			DEPLOY_MISSION_ID_SEQ_1008 = 8,
			DEPLOY_MISSION_ID_SEQ_1009 = 9,
			DEPLOY_MISSION_ID_SEQ_1010 = 10,
			DEPLOY_MISSION_ID_SEQ_1011 = 11,
			DEPLOY_MISSION_ID_SEQ_1012 = 12,
			DEPLOY_MISSION_ID_SEQ_1013 = 13,
			DEPLOY_MISSION_ID_SEQ_1014 = 14,
			DEPLOY_MISSION_ID_SEQ_1015 = 15,
			DEPLOY_MISSION_ID_SEQ_1016 = 16,
			DEPLOY_MISSION_ID_SEQ_1017 = 17,
			DEPLOY_MISSION_ID_SEQ_1018 = 18,
			DEPLOY_MISSION_ID_SEQ_1019 = 19,
			DEPLOY_MISSION_ID_SEQ_1020 = 20,
			DEPLOY_MISSION_ID_RANDOM_10000 = 21,
			DEPLOY_MISSION_ID_RANDOM_10001 = 22,
			DEPLOY_MISSION_ID_RANDOM_10002 = 23,
			DEPLOY_MISSION_ID_RANDOM_10003 = 24,
			DEPLOY_MISSION_ID_RANDOM_10004 = 25,
			DEPLOY_MISSION_ID_RANDOM_10005 = 26,
			DEPLOY_MISSION_ID_RANDOM_10006 = 27,
			DEPLOY_MISSION_ID_RANDOM_10007 = 28,
			DEPLOY_MISSION_ID_RANDOM_10100 = 29,
			DEPLOY_MISSION_ID_RANDOM_10101 = 30,
			DEPLOY_MISSION_ID_RANDOM_11000 = 31,
			DEPLOY_MISSION_ID_RANDOM_11001 = 32,
			DEPLOY_MISSION_ID_RANDOM_11100 = 33,
			DEPLOY_MISSION_ID_RANDOM_11101 = 34,
			DEPLOY_MISSION_ID_RANDOM_12000 = 35,
			DEPLOY_MISSION_ID_RANDOM_12001 = 36,
			DEPLOY_MISSION_ID_RANDOM_12002 = 37,
			DEPLOY_MISSION_ID_RANDOM_12003 = 38,
			DEPLOY_MISSION_ID_RANDOM_12004 = 39,
			DEPLOY_MISSION_ID_RANDOM_12100 = 40,
			DEPLOY_MISSION_ID_RANDOM_12101 = 41,
			DEPLOY_MISSION_ID_RANDOM_12102 = 42,
			DEPLOY_MISSION_ID_RANDOM_13000 = 43,
			DEPLOY_MISSION_ID_RANDOM_13001 = 44,
			DEPLOY_MISSION_ID_RANDOM_13002 = 45,
			DEPLOY_MISSION_ID_RANDOM_13100 = 46,
			DEPLOY_MISSION_ID_RANDOM_13101 = 47,
			DEPLOY_MISSION_ID_RANDOM_13102 = 48,
			DEPLOY_MISSION_ID_RANDOM_13103 = 49,
			DEPLOY_MISSION_ID_RANDOM_13104 = 50,
			DEPLOY_MISSION_ID_RANDOM_13105 = 51,
			DEPLOY_MISSION_ID_RANDOM_13106 = 52,
			DEPLOY_MISSION_ID_RANDOM_13107 = 53,
			DEPLOY_MISSION_ID_RANDOM_13200 = 54,
			DEPLOY_MISSION_ID_RANDOM_13201 = 55,
			DEPLOY_MISSION_ID_RANDOM_13202 = 56,
			DEPLOY_MISSION_ID_RANDOM_13203 = 57,
			DEPLOY_MISSION_ID_RANDOM_13204 = 58,
			DEPLOY_MISSION_ID_RANDOM_13205 = 59,
			DEPLOY_MISSION_ID_RANDOM_13206 = 60,
			DEPLOY_MISSION_ID_RANDOM_13207 = 61,
			DEPLOY_MISSION_ID_RANDOM_13208 = 62,
			DEPLOY_MISSION_ID_RANDOM_13209 = 63,
			DEPLOY_MISSION_ID_RANDOM_14000 = 64,
			DEPLOY_MISSION_ID_RANDOM_14001 = 65,
			DEPLOY_MISSION_ID_RANDOM_14002 = 66,
			DEPLOY_MISSION_ID_RANDOM_14003 = 67,
			DEPLOY_MISSION_ID_RANDOM_14004 = 68,
			DEPLOY_MISSION_ID_RANDOM_14100 = 69,
			DEPLOY_MISSION_ID_RANDOM_14101 = 70,
			DEPLOY_MISSION_ID_RANDOM_15000 = 71,
			DEPLOY_MISSION_ID_RANDOM_15001 = 72,
			DEPLOY_MISSION_ID_RANDOM_15002 = 73,
			DEPLOY_MISSION_ID_RANDOM_15003 = 74,
			DEPLOY_MISSION_ID_RANDOM_15004 = 75,
			DEPLOY_MISSION_ID_RANDOM_15005 = 76,
			DEPLOY_MISSION_ID_RANDOM_15006 = 77,
			DEPLOY_MISSION_ID_RANDOM_15007 = 78,
			DEPLOY_MISSION_ID_RANDOM_15008 = 79,
			DEPLOY_MISSION_ID_RANDOM_15009 = 80,
			DEPLOY_MISSION_ID_RANDOM_15010 = 81,
			DEPLOY_MISSION_ID_RANDOM_15011 = 82,
			DEPLOY_MISSION_ID_RANDOM_15100 = 83,
			DEPLOY_MISSION_ID_RANDOM_15101 = 84,
			DEPLOY_MISSION_ID_RANDOM_15102 = 85,
			DEPLOY_MISSION_ID_RANDOM_15103 = 86,
			DEPLOY_MISSION_ID_RANDOM_15104 = 87,
			DEPLOY_MISSION_ID_RANDOM_15105 = 88,
			DEPLOY_MISSION_ID_RANDOM_15106 = 89,
			DEPLOY_MISSION_ID_RANDOM_15107 = 90,
			DEPLOY_MISSION_ID_RANDOM_15108 = 91,
			DEPLOY_MISSION_ID_RANDOM_15109 = 92,
			DEPLOY_MISSION_ID_RANDOM_15110 = 93,
			DEPLOY_MISSION_ID_RANDOM_16000 = 94,
			DEPLOY_MISSION_ID_RANDOM_16001 = 95,
			DEPLOY_MISSION_ID_RANDOM_16002 = 96,
			DEPLOY_MISSION_ID_RANDOM_16003 = 97,
			DEPLOY_MISSION_ID_RANDOM_16004 = 98,
			DEPLOY_MISSION_ID_RANDOM_16005 = 99,
			DEPLOY_MISSION_ID_RANDOM_16006 = 100,
			DEPLOY_MISSION_ID_RANDOM_16007 = 101,
			DEPLOY_MISSION_ID_RANDOM_16100 = 102,
			DEPLOY_MISSION_ID_RANDOM_16101 = 103,
			DEPLOY_MISSION_ID_RANDOM_16200 = 104,
			DEPLOY_MISSION_ID_RANDOM_16201 = 105,
			DEPLOY_MISSION_ID_RANDOM_16202 = 106,
			DEPLOY_MISSION_ID_RANDOM_16203 = 107,
			DEPLOY_MISSION_ID_RANDOM_16204 = 108,
			DEPLOY_MISSION_ID_RANDOM_16205 = 109,
			DEPLOY_MISSION_ID_RANDOM_16206 = 110,
			DEPLOY_MISSION_ID_RANDOM_16207 = 111,
			DEPLOY_MISSION_ID_RANDOM_16208 = 112,
			DEPLOY_MISSION_ID_RANDOM_16209 = 113,
			DEPLOY_MISSION_ID_RANDOM_16210 = 114,
			DEPLOY_MISSION_ID_RANDOM_16211 = 115,
			DEPLOY_MISSION_ID_RANDOM_16212 = 116,
			DEPLOY_MISSION_ID_RANDOM_16213 = 117,
			DEPLOY_MISSION_ID_RANDOM_16214 = 118,
			DEPLOY_MISSION_ID_RANDOM_16215 = 119,
			DEPLOY_MISSION_ID_REVENGE_START = 120,
			DEPLOY_MISSION_ID_REVENGE_SMOKE = 120,
			DEPLOY_MISSION_ID_REVENGE_HEAD_SHOT = 121,
			DEPLOY_MISSION_ID_REVENGE_STEALTH1 = 122,
			DEPLOY_MISSION_ID_REVENGE_STEALTH2 = 123,
			DEPLOY_MISSION_ID_REVENGE_STEALTH3 = 124,
			DEPLOY_MISSION_ID_REVENGE_NIGHT_STEALTH = 125,
			DEPLOY_MISSION_ID_REVENGE_COMBAT1 = 126,
			DEPLOY_MISSION_ID_REVENGE_COMBAT2 = 127,
			DEPLOY_MISSION_ID_REVENGE_COMBAT3 = 128,
			DEPLOY_MISSION_ID_REVENGE_COMBAT4 = 129,
			DEPLOY_MISSION_ID_REVENGE_COMBAT5 = 130,
			DEPLOY_MISSION_ID_REVENGE_NIGHT_COMBAT = 131,
			DEPLOY_MISSION_ID_REVENGE_LONG_RANGE = 132,
			DEPLOY_MISSION_ID_REVENGE_VEHICLE = 133,
			DEPLOY_MISSION_ID_REVENGE_END = 133,
			DEPLOY_MISSION_RARITY_NONE = 0,
			DEPLOY_MISSION_RARITY_N = 1,
			DEPLOY_MISSION_RARITY_R = 2,
			DEPLOY_MISSION_RARITY_SR = 3,
			POOL_REWARD_TYPE_GMP = 1,
			POOL_REWARD_TYPE_STAFF_SPP = 2,
			POOL_REWARD_TYPE_STAFF_SP = 3,
			POOL_REWARD_TYPE_STAFF_S = 4,
			POOL_REWARD_TYPE_STAFF_A = 5,
			POOL_REWARD_TYPE_STAFF_B = 6,
			POOL_REWARD_TYPE_STAFF_C = 7,
			POOL_REWARD_TYPE_STAFF_D = 8,
			POOL_REWARD_TYPE_STAFF_E = 9,
			POOL_REWARD_TYPE_STAFF_F = 10,
			POOL_REWARD_TYPE_STAFF_G = 11,
			POOL_REWARD_TYPE_COMMON_METAL = 12,
			POOL_REWARD_TYPE_MINOR_METAL = 13,
			POOL_REWARD_TYPE_PRECIOUS_METAL = 14,
			POOL_REWARD_TYPE_FUEL_RESOURCE = 15,
			POOL_REWARD_TYPE_BIOTIC_RESOURCE = 16,
			POOL_REWARD_TYPE_GOLDEN_CRESCENT = 17,
			POOL_REWARD_TYPE_AFRICAN_PEACH = 18,
			POOL_REWARD_TYPE_DIGITALIS_P = 19,
			POOL_REWARD_TYPE_DIGITALIS_L = 20,
			POOL_REWARD_TYPE_BLACK_CARROT = 21,
			POOL_REWARD_TYPE_WORM_WOOD = 22,
			POOL_REWARD_TYPE_TARRAGON = 23,
			POOL_REWARD_TYPE_HAOMA = 24,
			POOL_REWARD_TYPE_MB_COIN = 26,
			MAIN_REWARD_TYPE_STAFF = 2,
			MAIN_REWARD_TYPE_COMMON_METAL = 12,
			MAIN_REWARD_TYPE_MINOR_METAL = 13,
			MAIN_REWARD_TYPE_PRECIOUS_METAL = 14,
			MAIN_REWARD_TYPE_FUEL_RESOURCE = 15,
			MAIN_REWARD_TYPE_BIOTIC_RESOURCE = 16,
			MAIN_REWARD_TYPE_GOLDEN_CRESCENT = 17,
			MAIN_REWARD_TYPE_AFRICAN_PEACH = 18,
			MAIN_REWARD_TYPE_DIGITALIS_P = 19,
			MAIN_REWARD_TYPE_DIGITALIS_L = 20,
			MAIN_REWARD_TYPE_BLACK_CARROT = 21,
			MAIN_REWARD_TYPE_WORM_WOOD = 22,
			MAIN_REWARD_TYPE_TARRAGON = 23,
			MAIN_REWARD_TYPE_HAOMA = 24,
			MAIN_REWARD_TYPE_KEY_ITEM = 25,
			MAIN_REWARD_TYPE_GMP = 1,
			SECTION_LV_MAX = 162,
			SWIM_SUIT_TYPE_1 = 0,
			SWIM_SUIT_TYPE_2 = 1,
			SWIM_SUIT_TYPE_3 = 2,
		};

		struct PlayerBasicInfo
		{
			struct Emblem
			{
				unsigned int texture_tag[4];
				unsigned int base_color[4];
				unsigned int frame_color[4];
				char position_x[4];
				char position_y[4];
				char scale[4];
				char rotate[4];
			};

			struct Resource
			{
				int fuel_resource;
				int biotic_resource;
				int common_metal;
				int minor_metal;
				int precious_metal;
			};

			fox::gk::FirstPartyAccount owner_account;
			char owner_name[0x64];
			char __pad1[3];
			unsigned int owner_player_id;
			fox::gk::FirstPartyAccount attacker_account;
			char attacker_name[0x64];
			char __pad3[3];
			unsigned int attacker_player_id;
			unsigned int mother_base_id[4];
			int field4;
			int attack_count;
			int support_count2;
			short staff_num;
			char mother_base_num;
			char platform_count[4];
			char security_rank[4];
			char area_id[4];
			char construct_param2[4];
			int supported_count;
			int support_count1;
			int field7;
			int field8;
			int field9;
			int field10;
			int capture_staff;
			int capture_resource_count;
			int attack_gmp;
			int field14;
			int left_hour2;
			struct
			{
				char follow : 1;
				char follower : 1;
				char enemy : 1;
				char help : 1;
				char online : 1;
				char insurance : 1;
				char hero : 1;
				char unused : 1;
			} fields1;
			char sneak_rank_grade;
			char field18_1;
			char field19_1;
			int sneak_rank_rank;
			char attacker_sneak_rank_grade;
			char league_rank_grade;
			char field26_1;
			char field16_2;
			int league_rank_rank;
			char field21_2;
			char field22_2;
			char field23_2;
			char field24_2;
			char field25_2;
			char field26_2;
			short espionage_section;
			short espionage_win;
			short espionage_total;
			int espionage_score;
			short attacker_espionage_section;
			short attacker_espionage_win;
			short attacker_espionage_total;
			char field21_4;
			char field22_4;
			int attacker_espionage_score;
			char nuclear;
			char owner_ugc;
			char field27_3;
			char field27_4;
			Emblem owner_emblem;
			char attacker_ugc;
			char field43_2;
			char field43_3;
			char field43_4;
			Emblem attacker_emblem;
			__int64 field45;
			__int64 field47;
			int field49;
			char field50_1;
			char field50_2;
			char field50_3;
			char field50_4;
			Resource processing_resource;
			Resource usable_resource;
			Resource capture_resource;
			int field58;
			int field59;
			int field60;
			int field61;
			int field62;
			int field63;
			int field64;
			int field65;
			int field66;
			int field67;
			int field68;
			int field69;
			int field70;
			int field71;
			int field72;
			short staff_count[10];
			int date_time;
			char cluster;
			char field79_2;
			short capture_staff_count[10];
			short injury_staff_count[10];
			short field80;
			Resource capture_resource2;
			int capture_nuclear;
			char is_win;
			char field91_2;
			char nameplate_id;
			char field91_4;
		};

		static_assert(sizeof(PlayerBasicInfo) == 1000);
		static_assert(offsetof(PlayerBasicInfo, owner_name) == 0x89);
		static_assert(offsetof(PlayerBasicInfo, nameplate_id) == 0x3E6);

		namespace impl
		{
			struct FobMission2CallbackImpl
			{
				char __pad0[176];
				char state;
				char __pad1[0x1000];
			};

			struct StaffControllerImpl
			{
				enum Rank
				{
					RANK_E = 0,
					RANK_D = 1,
					RANK_C = 2,
					RANK_B = 3,
					RANK_A = 4,
					RANK_AP = 5,
					RANK_APP = 6,
					RANK_S = 7,
					RANK_SP = 8,
					RANK_SPP = 9,
				};

				enum Designation
				{
					DESIGNATION_NONE = 0,
					DESIGNATION_UNITS_START = 1,
					DESIGNATION_COMBAT = 1,
					DESIGNATION_RND = 2,
					DESIGNATION_BASE_DEV = 3,
					DESIGNATION_SUPPORT = 4,
					DESIGNATION_INTEL = 5,
					DESIGNATION_MEDICAL = 6,
					DESIGNATION_SECURITY = 7,
					DESIGNATION_SICKBAY = 8,
					DESIGNATION_UNITS_END = 8,
					DESIGNATION_BRIG = 9,
					DESIGNATION_QUARANTINE = 10,
					DESIGNATION_WAITING_ROOM_1 = 11,
					DESIGNATION_WAITING_ROOM_2 = 12,
					DESIGNATION_WAITING_ROOM_3 = 13,
					DESIGNATION_WAITING_ROOM_4 = 14,
					DESIGNATION_WAITING_ROOM_5 = 15,
					DESIGNATION_COUNT,
				};

				enum Skill
				{
					SKILL_GUNMAN = 1,
					SKILL_CLIMBER = 2,
					SKILL_ATHLETE = 3,
					SKILL_RESCUER = 5,
					SKILL_QUICK_RELOAD = 6,
					SKILL_TOUGH_GUY = 7,
					SKILL_FORTUNATE = 8,
					SKILL_SAVAGE = 9,
					SKILL_BOASTER = 10,
					SKILL_BOTANIST = 11,
					SKILL_QUICK_DRAW = 12,
					SKILL_VANGUARD_SHARPSHOOTER = 13,
					SKILL_PROSTHETIC_MOBILITY_1 = 14,
					SKILL_PROSTHETIC_MOBILITY_2 = 15,
					SKILL_PROSTHETIC_MOBILITY_3 = 16,
					SKILL_PROSTHETIC_PRECISION_MOVEMENT_1 = 17,
					SKILL_PROSTHETIC_PRECISION_MOVEMENT_2 = 18,
					SKILL_PROSTHETIC_PRECISION_MOVEMENT_3 = 19,
					SKILL_PROSTHETIC_FIRST_AID_FUNCTION_1 = 20,
					SKILL_PROSTHETIC_FIRST_AID_FUNCTION_2 = 21,
					SKILL_PROSTHETIC_FIRST_AID_FUNCTION_3 = 22,
					SKILL_GUNSMITH_HANDGUNS = 23,
					SKILL_GUNSMITH_SUBMACHINE_GUNS = 24,
					SKILL_GUNSMITH_ASSAULT_RIFLES = 25,
					SKILL_GUNSMITH_SHOTGUNS = 26,
					SKILL_GUNSMITH_GRENADE_LAUNCHERS = 27,
					SKILL_GUNSMITH_SNIPER_RIFLES = 28,
					SKILL_GUNSMITH_MACHINE_GUNS = 29,
					SKILL_GUNSMITH_MISSILE_LAUNCHERS = 30,
					SKILL_MASTER_GUNSMITH = 31,
					SKILL_ANESTHESIA_SPECIALIST = 32,
					SKILL_NOISE_SUPPRESSION_SPECIALIST = 33,
					SKILL_MISSILE_GUIDANCE_SPECIALIST = 34,
					SKILL_ZOOLOGIST = 35,
					SKILL_SLEEPING_GAS_SPECIALIST = 36,
					SKILL_TRAP_SPECIALIST = 37,
					SKILL_ELECTROMAGNETC_NET_SPECIALIST = 38,
					SKILL_RADAR_SPECIALIST = 40,
					SKILL_METAMATERIALS_SPECIALIST = 41,
					SKILL_DRUG_DEVELOPER = 42,
					SKILL_BIONICS_SPECIALIST = 43,
					SKILL_MECHATRONICS_SPECIALIST = 44,
					SKILL_CYBERNETICS_SPECIALIST = 45,
					SKILL_ROCKET_CONTROL_SPECIALIST = 46,
					SKILL_ELECTROSPINNING_SPECIALIST = 47,
					SKILL_MATERIALS_ENGINEER = 48,
					SKILL_TRANSPORTATION_SPECIALIST = 49,
					SKILL_VIDEO_SURVEILLANCE_SPECIALIST = 51,
					SKILL_BIPEDAL_WEAPONS_DEVELOPER = 54,
					SKILL_INTERPRETER_RUSSIAN = 55,
					SKILL_INTERPRETER_AFRIKAANS = 56,
					SKILL_INTERPRETER_KIKONGO = 57,
					SKILL_INTERPRETER_PASHTO = 58,
					SKILL_SURGEON = 59,
					SKILL_PHYSICIAN = 60,
					SKILL_COUNSELOR = 61,
					SKILL_PARASITOLOGIST = 62,
					SKILL_TROUBLEMAKER_VIOLENCE = 63,
					SKILL_TROUBLEMAKER_UNSANITARY = 64,
					SKILL_TROUBLEMAKER_HARASSMENT = 65,
					SKILL_DIPLOMAT = 66,
					SKILL_TACTICAL_INSTRUCTOR = 67,
					SKILL_MOTHER_BASE_XO = 68,
					SKILL_REQUIRED_END = 69,
					SKILL_DEFENDER_1 = 69,
					SKILL_DEFENDER_2 = 70,
					SKILL_DEFENDER_3 = 71,
					SKILL_SENTRY_1 = 72,
					SKILL_SENTRY_2 = 73,
					SKILL_SENTRY_3 = 74,
					SKILL_RANGER_1 = 75,
					SKILL_RANGER_2 = 76,
					SKILL_RANGER_3 = 77,
					SKILL_MEDIC_1 = 78,
					SKILL_MEDIC_2 = 79,
					SKILL_MEDIC_3 = 80,
					SKILL_LIQUID_CARBON_MISSILE_ENGINEER_1 = 81,
					SKILL_LIQUID_CARBON_MISSILE_ENGINEER_2 = 82,
					SKILL_LIQUID_CARBON_MISSILE_ENGINEER_3 = 83,
					SKILL_ANTI_BALLISTIC_MISSILE_ENGINEER_1 = 84,
					SKILL_ANTI_BALLISTIC_MISSILE_ENGINEER_2 = 85,
					SKILL_ANTI_BALLISTIC_MISSILE_ENGINEER_3 = 86,
				};

				enum StatDistribution
				{
					STAT_DIST_NONE_1 = 0b11010,
					STAT_DIST_NONE_2 = 0b11011,
					STAT_DIST_NONE_3 = 0b11100,
					STAT_DIST_NONE_4 = 0b11101,
					STAT_DIST_NONE_5 = 0b100100,
					STAT_DIST_NONE_6 = 0b100101,
					STAT_DIST_NONE_7 = 0b100110,
					STAT_DIST_NONE_8 = 0b100111,
					STAT_DIST_NONE_9 = 0b111011,
					STAT_DIST_SECURITY = 0b1,
					STAT_DIST_BASE_DEV_FOCUS = 0b100,
					STAT_DIST_BASE_DEV_AND_COMBAT = 0b110011,
					STAT_DIST_BASE_DEV_AND_INTEL = 0b10010,
					STAT_DIST_BASE_DEV_PLUS_AND_INTEL_PLUS = 0b110001,
					STAT_DIST_BASE_DEV_AND_MEDICAL = 0b10011,
					STAT_DIST_BASE_DEV_PLUS_AND_MEDICAL_PLUS = 0b110010,
					STAT_DIST_BASE_DEV_AND_RND = 0b101110,
					STAT_DIST_BASE_DEV_AND_SUPPORT_AND_INTEL_AND_MEDICAL = 0b100000,
					STAT_DIST_BASE_DEV_AND_SUPPORT = 0b10001,
					STAT_DIST_BASE_DEV_PLUS_AND_SUPPORT_PLUS = 0b110000,
					STAT_DIST_COMBAT_FOCUS = 0b10,
					STAT_DIST_COMBAT_AND_BASE_DEV = 0b101111,
					STAT_DIST_COMBAT_AND_INTEL = 0b1001,
					STAT_DIST_COMBAT_PLUS_AND_INTEL_PLUS = 0b101001,
					STAT_DIST_COMBAT_AND_MEDICAL = 0b1010,
					STAT_DIST_COMBAT_AND_RND = 0b1000,
					STAT_DIST_COMBAT_AND_SUPPORT_AND_INTEL_AND_MEDICAL = 0b11111,
					STAT_DIST_COMBAT_AND_SUPPORT = 0b101000,
					STAT_DIST_INTEL_FOCUS = 0b110,
					STAT_DIST_INTEL_AND_BASE_DEV = 0b10000,
					STAT_DIST_INTEL_PLUS_AND_BASE_DEV_PLUS = 0b110110,
					STAT_DIST_INTEL_AND_COMBAT_AND_SUPPORT_AND_MEDICAL = 0b100010,
					STAT_DIST_INTEL_AND_COMBAT = 0b1110,
					STAT_DIST_INTEL_PLUS_AND_COMBAT_PLUS = 0b111001,
					STAT_DIST_INTEL_AND_MEDICAL = 0b111000,
					STAT_DIST_INTEL_AND_RND = 0b1111,
					STAT_DIST_INTEL_AND_SUPPORT = 0b111010,
					STAT_DIST_MEDICAL_FOCUS = 0b111,
					STAT_DIST_MEDICAL_AND_BASE_DEV_AND_SUPPORT_AND_INTEL = 0b100011,
					STAT_DIST_MEDICAL_AND_BASE_DEV = 0b11000,
					STAT_DIST_MEDICAL_PLUS_AND_BASE_DEV_PLUS = 0b111110,
					STAT_DIST_MEDICAL_AND_COMBAT = 0b10111,
					STAT_DIST_MEDICAL_PLUS_AND_COMBAT_PLUS = 0b111100,
					STAT_DIST_MEDICAL_AND_INTEL = 0b110111,
					STAT_DIST_MEDICAL_AND_RND = 0b111101,
					STAT_DIST_MEDICAL_AND_SUPPORT = 0b11001,
					STAT_DIST_RND_FOCUS = 0b11,
					STAT_DIST_RND_AND_BASE_DEV = 0b1011,
					STAT_DIST_RND_PLUS_AND_BASE_DEV_PLUS = 0b101011,
					STAT_DIST_RND_AND_COMBAT = 0b101010,
					STAT_DIST_RND_AND_MEDICAL = 0b1101,
					STAT_DIST_RND_PLUS_AND_MEDICAL_PLUS = 0b101101,
					STAT_DIST_RND_AND_SUPPORT_AND_INTEL_AND_MEDICAL = 0b11110,
					STAT_DIST_RND_AND_SUPPORT = 0b1100,
					STAT_DIST_RND_PLUS_AND_SUPPORT_PLUS = 0b101100,
					STAT_DIST_SUPPORT_FOCUS = 0b101,
					STAT_DIST_SUPPORT_AND_COMBAT = 0b10100,
					STAT_DIST_SUPPORT_PLUS_AND_COMBAT_PLUS = 0b110100,
					STAT_DIST_SUPPORT_AND_INTEL = 0b10110,
					STAT_DIST_SUPPORT_PLUS_AND_INTEL_PLUS = 0b110101,
					STAT_DIST_SUPPORT_AND_RND_AND_INTEL_AND_MEDICAL = 0b100001,
					STAT_DIST_SPECIAL_CHARACTER = 0b111111,
				};

				union StaffHeader
				{
					struct
					{
						std::uint32_t suppress_stats : 1;
						std::uint32_t stat_bonus : 2;
						std::uint32_t peak_rank : 4;
						std::uint32_t stat_distribution : 6;
						std::uint32_t skill : 7;
						std::uint32_t face_gender : 10;
						std::uint32_t unk : 2;
					} fields;
					std::uint32_t data;
				};

				union StaffStatusSync
				{
					struct
					{
						std::uint32_t combat_deployment_team : 4;
						std::uint32_t player_selected : 3;
						std::uint32_t direct_contract : 1;
						std::uint32_t proficiency : 4;
						std::uint32_t ds_medal : 1;
						std::uint32_t ds_cross : 1;
						std::uint32_t honor_medal : 1;
						std::uint32_t unk : 1;
						std::uint32_t symptomatic : 1;
						std::uint32_t health_level : 3;
						std::uint32_t health_state : 2;
						std::uint32_t morale : 4;
						std::uint32_t enemy : 1;
						std::uint32_t designation : 4;
						std::uint32_t unselectable : 1;
					} fields;
					std::uint32_t data;
				};

				union StaffSeed
				{
					struct
					{
						std::uint32_t unk1 : 11;
						std::uint32_t unk2 : 21;
					} fields;
					std::uint32_t data;
				};

				union StaffStatusNoSync
				{
					struct
					{
						std::uint16_t locked : 1;
						std::uint16_t unk : 15;
					} fields;
					std::uint16_t data;
				};

				struct StaffUnk1
				{
					std::uint32_t data;
				};

				struct StaffUnk2
				{
					std::uint32_t data;
				};

				union Staff
				{
					struct
					{
						StaffUnk1 unk1;
						StaffUnk2 unk2;
						StaffHeader header;
						StaffSeed seed;
						StaffStatusSync status_sync;
						StaffStatusNoSync status_no_sync;
					} fields;
					std::uint32_t packed[6];
				};

				struct vtable
				{
					void(__fastcall* meth_000)(StaffControllerImpl*);
					void(__fastcall* meth_001)(StaffControllerImpl*);
					void(__fastcall* meth_002)(StaffControllerImpl*);
					void(__fastcall* meth_003)(StaffControllerImpl*);
					void(__fastcall* meth_004)(StaffControllerImpl*);
					void(__fastcall* meth_005)(StaffControllerImpl*);
					void(__fastcall* meth_006)(StaffControllerImpl*);
					void(__fastcall* meth_007)(StaffControllerImpl*);
					void(__fastcall* meth_008)(StaffControllerImpl*);
					void(__fastcall* meth_009)(StaffControllerImpl*);
					void(__fastcall* meth_010)(StaffControllerImpl*);
					void(__fastcall* meth_011)(StaffControllerImpl*);
					void(__fastcall* meth_012)(StaffControllerImpl*);
					void(__fastcall* meth_013)(StaffControllerImpl*);
					void(__fastcall* meth_014)(StaffControllerImpl*);
					void(__fastcall* meth_015)(StaffControllerImpl*);
					void(__fastcall* meth_016)(StaffControllerImpl*);
					void(__fastcall* meth_017)(StaffControllerImpl*);
					void(__fastcall* meth_018)(StaffControllerImpl*);
					void(__fastcall* meth_019)(StaffControllerImpl*);
					void(__fastcall* meth_020)(StaffControllerImpl*);
					void(__fastcall* meth_021)(StaffControllerImpl*);
					void(__fastcall* meth_022)(StaffControllerImpl*);
					void(__fastcall* meth_023)(StaffControllerImpl*);
					void(__fastcall* meth_024)(StaffControllerImpl*);
					void(__fastcall* meth_025)(StaffControllerImpl*);
					void(__fastcall* meth_026)(StaffControllerImpl*);
					void(__fastcall* meth_027)(StaffControllerImpl*);
					void(__fastcall* meth_028)(StaffControllerImpl*);
					void(__fastcall* meth_029)(StaffControllerImpl*);
					void(__fastcall* meth_030)(StaffControllerImpl*);
					void(__fastcall* meth_031)(StaffControllerImpl*);
					void(__fastcall* meth_032)(StaffControllerImpl*);
					void(__fastcall* meth_033)(StaffControllerImpl*);
					void(__fastcall* meth_034)(StaffControllerImpl*);
					void(__fastcall* meth_035)(StaffControllerImpl*);
					void(__fastcall* meth_036)(StaffControllerImpl*);
					void(__fastcall* meth_037)(StaffControllerImpl*);
					void(__fastcall* meth_038)(StaffControllerImpl*);
					void(__fastcall* meth_039)(StaffControllerImpl*);
					void(__fastcall* meth_040)(StaffControllerImpl*);
					void(__fastcall* meth_041)(StaffControllerImpl*);
					void(__fastcall* meth_042)(StaffControllerImpl*);
					void(__fastcall* meth_043)(StaffControllerImpl*);
					void(__fastcall* meth_044)(StaffControllerImpl*);
					void(__fastcall* meth_045)(StaffControllerImpl*);
					void(__fastcall* meth_046)(StaffControllerImpl*);
					void(__fastcall* meth_047)(StaffControllerImpl*);
					void(__fastcall* meth_048)(StaffControllerImpl*);
					void(__fastcall* meth_049)(StaffControllerImpl*);
					void(__fastcall* meth_050)(StaffControllerImpl*);
					void(__fastcall* meth_051)(StaffControllerImpl*);
					void(__fastcall* meth_052)(StaffControllerImpl*);
					void(__fastcall* meth_053)(StaffControllerImpl*);
					void(__fastcall* meth_054)(StaffControllerImpl*);
					void(__fastcall* meth_055)(StaffControllerImpl*);
					void(__fastcall* meth_056)(StaffControllerImpl*);
					void(__fastcall* meth_057)(StaffControllerImpl*);
					void(__fastcall* meth_058)(StaffControllerImpl*);
					void(__fastcall* meth_059)(StaffControllerImpl*);
					void(__fastcall* meth_060)(StaffControllerImpl*);
					void(__fastcall* meth_061)(StaffControllerImpl*);
					void(__fastcall* meth_062)(StaffControllerImpl*);
					void(__fastcall* meth_063)(StaffControllerImpl*);
					void(__fastcall* meth_064)(StaffControllerImpl*);
					void(__fastcall* meth_065)(StaffControllerImpl*);
					void(__fastcall* meth_066)(StaffControllerImpl*);
					void(__fastcall* meth_067)(StaffControllerImpl*);
					void(__fastcall* meth_068)(StaffControllerImpl*);
					void(__fastcall* meth_069)(StaffControllerImpl*);
					void(__fastcall* meth_070)(StaffControllerImpl*);
					void(__fastcall* meth_071)(StaffControllerImpl*);
					void(__fastcall* meth_072)(StaffControllerImpl*);
					void(__fastcall* meth_073)(StaffControllerImpl*);
					void(__fastcall* meth_074)(StaffControllerImpl*);
					void(__fastcall* meth_075)(StaffControllerImpl*);
					void(__fastcall* meth_076)(StaffControllerImpl*);
					void(__fastcall* meth_077)(StaffControllerImpl*);
					void(__fastcall* meth_078)(StaffControllerImpl*);
					void(__fastcall* meth_079)(StaffControllerImpl*);
					void(__fastcall* meth_080)(StaffControllerImpl*);
					void(__fastcall* meth_081)(StaffControllerImpl*);
					void(__fastcall* meth_082)(StaffControllerImpl*);
					void(__fastcall* meth_083)(StaffControllerImpl*);
					void(__fastcall* meth_084)(StaffControllerImpl*);
					void(__fastcall* meth_085)(StaffControllerImpl*);
					void(__fastcall* meth_086)(StaffControllerImpl*);
					void(__fastcall* meth_087)(StaffControllerImpl*);
					void(__fastcall* meth_088)(StaffControllerImpl*);
					void(__fastcall* meth_089)(StaffControllerImpl*);
					void(__fastcall* meth_090)(StaffControllerImpl*);
					void(__fastcall* meth_091)(StaffControllerImpl*);
					void(__fastcall* meth_092)(StaffControllerImpl*);
					void(__fastcall* meth_093)(StaffControllerImpl*);
					void(__fastcall* meth_094)(StaffControllerImpl*);
					void(__fastcall* meth_095)(StaffControllerImpl*);
					void(__fastcall* meth_096)(StaffControllerImpl*);
					void(__fastcall* meth_097)(StaffControllerImpl*);
					void(__fastcall* meth_098)(StaffControllerImpl*);
					void(__fastcall* meth_099)(StaffControllerImpl*);
					void(__fastcall* meth_100)(StaffControllerImpl*);
					void(__fastcall* meth_101)(StaffControllerImpl*);
					void(__fastcall* meth_102)(StaffControllerImpl*);
					void(__fastcall* meth_103)(StaffControllerImpl*);
					void(__fastcall* meth_104)(StaffControllerImpl*);
					void(__fastcall* meth_105)(StaffControllerImpl*);
					void(__fastcall* meth_106)(StaffControllerImpl*);
					void(__fastcall* meth_107)(StaffControllerImpl*);
					void(__fastcall* meth_108)(StaffControllerImpl*);
					void(__fastcall* meth_109)(StaffControllerImpl*);
					void(__fastcall* meth_110)(StaffControllerImpl*);
					void(__fastcall* meth_111)(StaffControllerImpl*);
					void(__fastcall* meth_112)(StaffControllerImpl*);
					void(__fastcall* meth_113)(StaffControllerImpl*);
					void(__fastcall* meth_114)(StaffControllerImpl*);
					void(__fastcall* meth_115)(StaffControllerImpl*);
					void(__fastcall* meth_116)(StaffControllerImpl*);
					void(__fastcall* meth_117)(StaffControllerImpl*);
					void(__fastcall* meth_118)(StaffControllerImpl*);
					void(__fastcall* meth_119)(StaffControllerImpl*);
					void(__fastcall* meth_120)(StaffControllerImpl*);
					void(__fastcall* meth_121)(StaffControllerImpl*);
					void(__fastcall* meth_122)(StaffControllerImpl*);
					void(__fastcall* meth_123)(StaffControllerImpl*);
					void(__fastcall* meth_124)(StaffControllerImpl*);
					void(__fastcall* meth_125)(StaffControllerImpl*);
					void(__fastcall* meth_126)(StaffControllerImpl*);
					void(__fastcall* meth_127)(StaffControllerImpl*);
					void(__fastcall* meth_128)(StaffControllerImpl*);
					void(__fastcall* meth_129)(StaffControllerImpl*);
					void(__fastcall* meth_130)(StaffControllerImpl*);
					void(__fastcall* meth_131)(StaffControllerImpl*);
					void(__fastcall* meth_132)(StaffControllerImpl*);
					void(__fastcall* meth_133)(StaffControllerImpl*);
					void(__fastcall* meth_134)(StaffControllerImpl*);
					void(__fastcall* meth_135)(StaffControllerImpl*);
					void(__fastcall* meth_136)(StaffControllerImpl*);
					void(__fastcall* meth_137)(StaffControllerImpl*);
					void(__fastcall* meth_138)(StaffControllerImpl*);
					void(__fastcall* meth_139)(StaffControllerImpl*);
					void(__fastcall* meth_140)(StaffControllerImpl*);
					bool(__fastcall* CanFobStaff)(StaffControllerImpl*, StaffHeader, StaffSeed, StaffStatusSync, StaffStatusNoSync);
					void(__fastcall* meth_142)(StaffControllerImpl*);
					void(__fastcall* meth_143)(StaffControllerImpl*);
					void(__fastcall* meth_144)(StaffControllerImpl*);
					void(__fastcall* meth_145)(StaffControllerImpl*);
					void(__fastcall* meth_146)(StaffControllerImpl*);
					void(__fastcall* meth_147)(StaffControllerImpl*);
					void(__fastcall* meth_148)(StaffControllerImpl*);
					void(__fastcall* meth_149)(StaffControllerImpl*);
					void(__fastcall* meth_150)(StaffControllerImpl*);
					void(__fastcall* meth_151)(StaffControllerImpl*);
					void(__fastcall* meth_152)(StaffControllerImpl*);
					void(__fastcall* meth_153)(StaffControllerImpl*);
					void(__fastcall* meth_154)(StaffControllerImpl*);
					void(__fastcall* meth_155)(StaffControllerImpl*);
					void(__fastcall* meth_156)(StaffControllerImpl*);
					void(__fastcall* meth_157)(StaffControllerImpl*);
					void(__fastcall* meth_158)(StaffControllerImpl*);
					void(__fastcall* meth_159)(StaffControllerImpl*);
					void(__fastcall* meth_160)(StaffControllerImpl*);
					void(__fastcall* meth_161)(StaffControllerImpl*);
					void(__fastcall* meth_162)(StaffControllerImpl*);
					void(__fastcall* meth_163)(StaffControllerImpl*);
					void(__fastcall* meth_164)(StaffControllerImpl*);
					void(__fastcall* meth_165)(StaffControllerImpl*);
					void(__fastcall* meth_166)(StaffControllerImpl*);
					void(__fastcall* meth_167)(StaffControllerImpl*);
					void(__fastcall* meth_168)(StaffControllerImpl*);
					void(__fastcall* meth_169)(StaffControllerImpl*);
					void(__fastcall* meth_170)(StaffControllerImpl*);
					void(__fastcall* meth_171)(StaffControllerImpl*);
					void(__fastcall* meth_172)(StaffControllerImpl*);
					void(__fastcall* meth_173)(StaffControllerImpl*);
					void(__fastcall* meth_174)(StaffControllerImpl*);
					void(__fastcall* meth_175)(StaffControllerImpl*);
					void(__fastcall* meth_176)(StaffControllerImpl*);
					void(__fastcall* meth_177)(StaffControllerImpl*);
					void(__fastcall* meth_178)(StaffControllerImpl*);
					void(__fastcall* meth_179)(StaffControllerImpl*);
					void(__fastcall* meth_180)(StaffControllerImpl*);
					void(__fastcall* meth_181)(StaffControllerImpl*);
					void(__fastcall* meth_182)(StaffControllerImpl*);
					void(__fastcall* meth_183)(StaffControllerImpl*);
					void(__fastcall* meth_184)(StaffControllerImpl*);
					void(__fastcall* meth_185)(StaffControllerImpl*);
					void(__fastcall* meth_186)(StaffControllerImpl*);
					void(__fastcall* meth_187)(StaffControllerImpl*);
					void(__fastcall* meth_188)(StaffControllerImpl*);
					void(__fastcall* meth_189)(StaffControllerImpl*);
					void(__fastcall* meth_190)(StaffControllerImpl*);
					void(__fastcall* meth_191)(StaffControllerImpl*);
					void(__fastcall* meth_192)(StaffControllerImpl*);
					void(__fastcall* meth_193)(StaffControllerImpl*);
					void(__fastcall* meth_194)(StaffControllerImpl*);
					void(__fastcall* meth_195)(StaffControllerImpl*);
					void(__fastcall* meth_196)(StaffControllerImpl*);
					void(__fastcall* meth_197)(StaffControllerImpl*);
					void(__fastcall* meth_198)(StaffControllerImpl*);
					void(__fastcall* meth_199)(StaffControllerImpl*);
					void(__fastcall* meth_200)(StaffControllerImpl*);
					void(__fastcall* meth_201)(StaffControllerImpl*);
					void(__fastcall* meth_202)(StaffControllerImpl*);
					void(__fastcall* meth_203)(StaffControllerImpl*);
					void(__fastcall* meth_204)(StaffControllerImpl*);
					void(__fastcall* meth_205)(StaffControllerImpl*);
					void(__fastcall* meth_206)(StaffControllerImpl*);
					void(__fastcall* meth_207)(StaffControllerImpl*);
					void(__fastcall* meth_208)(StaffControllerImpl*);
					void(__fastcall* meth_209)(StaffControllerImpl*);
					void(__fastcall* meth_210)(StaffControllerImpl*);
					void(__fastcall* meth_211)(StaffControllerImpl*);
					void(__fastcall* meth_212)(StaffControllerImpl*);
					void(__fastcall* meth_213)(StaffControllerImpl*);
					void(__fastcall* meth_214)(StaffControllerImpl*);
					void(__fastcall* meth_215)(StaffControllerImpl*);
					void(__fastcall* meth_216)(StaffControllerImpl*);
					void(__fastcall* meth_217)(StaffControllerImpl*);
					void(__fastcall* meth_218)(StaffControllerImpl*);
					void(__fastcall* meth_219)(StaffControllerImpl*);
					void(__fastcall* meth_220)(StaffControllerImpl*);
					void(__fastcall* meth_221)(StaffControllerImpl*);
					void(__fastcall* meth_222)(StaffControllerImpl*);
					void(__fastcall* meth_223)(StaffControllerImpl*);
					void(__fastcall* meth_224)(StaffControllerImpl*);
					void(__fastcall* meth_225)(StaffControllerImpl*);
					void(__fastcall* meth_226)(StaffControllerImpl*);
					void(__fastcall* meth_227)(StaffControllerImpl*);
					void(__fastcall* meth_228)(StaffControllerImpl*);
					void(__fastcall* meth_229)(StaffControllerImpl*);
					void(__fastcall* meth_230)(StaffControllerImpl*);
					void(__fastcall* meth_231)(StaffControllerImpl*);
					void(__fastcall* meth_232)(StaffControllerImpl*);
					void(__fastcall* meth_233)(StaffControllerImpl*);
					void(__fastcall* meth_234)(StaffControllerImpl*);
					void(__fastcall* meth_235)(StaffControllerImpl*);
					void(__fastcall* meth_236)(StaffControllerImpl*);
					void(__fastcall* meth_237)(StaffControllerImpl*);
					void(__fastcall* meth_238)(StaffControllerImpl*);
					void(__fastcall* meth_239)(StaffControllerImpl*);
					void(__fastcall* meth_240)(StaffControllerImpl*);
					void(__fastcall* meth_241)(StaffControllerImpl*);
					void(__fastcall* meth_242)(StaffControllerImpl*);
					void(__fastcall* meth_243)(StaffControllerImpl*);
					void(__fastcall* meth_244)(StaffControllerImpl*);
					void(__fastcall* meth_245)(StaffControllerImpl*);
					void(__fastcall* meth_246)(StaffControllerImpl*);
					void(__fastcall* meth_247)(StaffControllerImpl*);
					void(__fastcall* meth_248)(StaffControllerImpl*);
					void(__fastcall* meth_249)(StaffControllerImpl*);
					void(__fastcall* meth_250)(StaffControllerImpl*);
					void(__fastcall* meth_251)(StaffControllerImpl*);
					void(__fastcall* meth_252)(StaffControllerImpl*);
					void(__fastcall* meth_253)(StaffControllerImpl*);
					void(__fastcall* meth_254)(StaffControllerImpl*);
					void(__fastcall* meth_255)(StaffControllerImpl*);
				};

				vtable* __vftable;
				char __pad0[40048];
				StaffHeader* mbmStaffSvarsHeaders;
				StaffSeed* mbmStaffSvarsSeeds;
				StaffStatusSync* mbmStaffSvarsStatusesSync;
				StaffStatusNoSync* mbmStaffSvarsStatusesNoSync;
				char __pad1[392];
				StaffHeader* mbmRemoverSvarsHeaders;
				StaffSeed* mbmRemoverSvarsSeeds;
				StaffStatusSync* mbmRemoverSvarsStatusesSync;
				StaffStatusNoSync* mbmRemoverSvarsStatusesNoSync;
				char __pad2[44];
				short staffCount;
				char __pad3[4];
				__int16 sectionCounts[7];
				char __pad4[474];
				unsigned __int8 sectionLevels[7];
			};

			static_assert(offsetof(StaffControllerImpl, mbmStaffSvarsHeaders) == 0x9C78);
			static_assert(offsetof(StaffControllerImpl, mbmStaffSvarsStatusesSync) == 0x9C88);

			struct ResourceControllerImpl
			{
				struct UsableResource
				{
					std::uint32_t sign : 1;
					std::uint32_t count : 20;
					std::uint32_t unk : 11;
				};

				struct ProcessingResource
				{
					std::uint32_t unk1 : 3;
					std::uint32_t count : 20;
					std::uint32_t unk2 : 11;
				};

				struct vtable
				{
					void(__fastcall* meth_01)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_02)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_03)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_04)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* AddResource)(tpp::mbm::impl::ResourceControllerImpl*, unsigned char, int, char, bool, bool, bool);
					void(__fastcall* meth_06)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_07)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_08)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_09)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_10)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_11)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_12)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_13)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_14)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_15)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_16)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_17)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_18)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_19)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_20)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_21)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_22)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_23)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_24)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_25)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_26)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_27)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_28)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_29)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_30)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_31)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_32)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_33)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_34)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_35)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_36)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_37)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_38)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_39)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_40)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_41)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_42)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_43)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_44)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_45)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_46)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_47)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_48)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_49)(tpp::mbm::impl::ResourceControllerImpl*);
					void(__fastcall* meth_50)(tpp::mbm::impl::ResourceControllerImpl*);
				};

				vtable* __vftable;
				char __pad0[4584];
				UsableResource* usableResource;
				ProcessingResource* processingResource;
			};

			struct MotherBaseManagementSystemImpl
			{
				struct vtable
				{
					void(__fastcall* meth_01)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_02)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_03)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_04)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_05)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_06)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* AddTppGmp)(tpp::mbm::impl::MotherBaseManagementSystemImpl*, unsigned int);
					void(__fastcall* SubTppGmp)(tpp::mbm::impl::MotherBaseManagementSystemImpl*, unsigned int);
					void(__fastcall* meth_09)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_10)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_11)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_12)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_13)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_14)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_15)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_16)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_17)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_18)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_19)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_20)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_21)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_22)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_23)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_24)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_25)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_26)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_27)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_28)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_29)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_30)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_31)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_32)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_33)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_34)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_35)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_36)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_37)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_38)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_39)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_40)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_41)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_42)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_43)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_44)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_45)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* ReflectHeroicPointDiffToSvars)(tpp::mbm::impl::MotherBaseManagementSystemImpl*, int);
					void(__fastcall* meth_47)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_48)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_49)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
					void(__fastcall* meth_50)(tpp::mbm::impl::MotherBaseManagementSystemImpl*);
				};

				vtable* __vftable;
				char __pad0[2584];
				int* heroicPoint;
				int* heroicPointDiff;
				int* ogrePoint;
				int* ogrePointDiff;
				char __pad2[128];
				StaffControllerImpl* staffController;
				char __pad3[24];
				ResourceControllerImpl* resourceController;
			};

			static_assert(offsetof(MotherBaseManagementSystemImpl, staffController) == 2752);
			static_assert(offsetof(MotherBaseManagementSystemImpl, heroicPointDiff) == 2600);
			static_assert(offsetof(MotherBaseManagementSystemImpl, ogrePoint) == 0xA30);
		}
	}

	namespace tpp::net
	{
		struct ServerManager_vtbl
		{

		};

		struct ServerManager
		{
			ServerManager_vtbl* __vftable;
		};

		struct SessionConnectInfo
		{
			int a1;
			char __pad0[180];
			std::uint64_t hostParam;
		};

		struct RequestDisplayImpl
		{
			struct vtable
			{

			};

			vtable* __vftable;
			fox::gk::FirstPartyAccount* unkArray[12];
			char __pad1[704];
			int a1;
			int a2;
		};

		struct DisplayName
		{
			struct vtable
			{

			};

			vtable* __vftable;
			RequestDisplayImpl* requestDisplay;
		};

		struct ServerReceiveBase
		{
			struct vtable
			{
				void(__fastcall* __destructor)(tpp::net::ServerReceiveBase*);
				void(__fastcall* Release)(tpp::net::ServerReceiveBase*);
				void(__fastcall* Receive)(tpp::net::ServerReceiveBase*, void** ptr);
				void(__fastcall* Success)(tpp::net::ServerReceiveBase*, void** ptr);
				void(__fastcall* Failure)(tpp::net::ServerReceiveBase*, void** ptr);
			};

			vtable* __vftable;
			fox::pfimpl pf;
		};

		struct ServerRequestBase
		{
			struct vtable
			{

			};

			tpp::net::ServerRequestBase::vtable* __vftable;
			fox::ncl::NclHttpCodec* httpCodec;
			int state;
			int mode;
			fox::StringId error;
			int errorCount;
			char a6;
		};

		struct FobTarget : ServerRequestBase
		{
			mbm::PlayerBasicInfo* playerInfos;
			char __pad2[8];
			short maxPlayers;
			char __pad3[6];
			SessionConnectInfo* sessionConnectInfo;
			char __pad4[4];
			DisplayName* displayName1;
			DisplayName* displayName2;
		};

		struct SessionControl : tpp::net::ServerRequestBase
		{
		
		};

		struct PlayerPlatformInfo
		{
			struct Npid
			{
				struct
				{
					fox::String data;
					int term;
					struct
					{
						int value[3];
						int count;
					} dummy;
				} handler;
				struct
				{
					int value[8];
					int count;
				} opt;
				struct
				{
					int value[8];
					int count;
				} reserved;
			};

			int playerId;
			int playerPlatformInfoField0;
			fox::String name;
			Npid npid;
			unsigned __int64 xuid;
			int ugc;
			int playerPlatformInfo31;
		};

		struct ClusterSecurity
		{
			struct CoordParam
			{
				int placed_index;
				int position_x;
				int position_y;
				int position_z;
				int rotation_w;
				int rotation_x;
				int rotation_y;
				int rotation_z;
			};

			int soldier;
			int ir_sensor;
			int antitheft;
			int camera;
			int decoy;
			int mine;
			int uav;
			int caution_area;
			int voluntary_coord_camera_count;
			int voluntary_coord_mine_count;
			CoordParam voluntary_coord_camera_params[1];
			int camera_count;
			CoordParam voluntary_coord_mine_params[4];
			int mine_count;
		};

		struct ClusterParam
		{
			int build;
			int soldier_rank;
			int cluster_security;
			ClusterSecurity unique_security;
			ClusterSecurity common1_security;
			ClusterSecurity common2_security;
			ClusterSecurity common3_security;
		};

		struct MotherBaseParam
		{
			int mother_base_id;
			int fob_index;
			int construct_param;
			int security_rank;
			int platform_count;
			int price;
			ClusterParam cluster_param[7];
			int cluster_param_count;
			int area_id;
		};

		struct FobTargetInfo
		{
			struct Emblem
			{
				struct EmblemPart
				{
					int texture_tag;
					int base_color;
					int frame_color;
					int position_x;
					int position_y;
					int scale;
					int rotate;
				};

				EmblemPart parts[4];
				int part_count;
			};

			struct Espionage
			{
				int section;
				int win;
				int lose;
				int score;
			};

			struct Rank
			{
				int grade;
				int rank;
				int score;
			};

			struct DetailRecord
			{
				int follow;
				int follower;
				int hero;
				int enemy;
				int help;
				int online;
				int insurance;
				Emblem emblem;
				int staff_count;
				int nuclear;
				Rank league_rank;
				Rank sneak_rank;
				Espionage espionage;
				int name_plate_id;
				int is_security_challenge;
			};

			struct Resource
			{
				int common_metal;
				int minor_metal;
				int precious_metal;
				int fuel_resource;
				int biotic_resource;
			};

			struct StaffCount
			{
				int count[10];
				int num_count;
			};

			struct FobRecord
			{
				int attack_count;
				int support_count;
				int supported_count;
				Resource usable_resource;
				Resource processing_resource;
				StaffCount staff_count;
				StaffCount capture_staff_count;
				StaffCount injury_staff_count;
				int nuclear;
				int capture_staff;
				Resource capture_resource;
				int capture_resource_count;
				int attack_gmp;
				int capture_nuclear;
				int left_hour;
				int date_time;
				int name_plate_id;
			};

			PlayerPlatformInfo owner_info;
			DetailRecord owner_detail_record;
			FobRecord owner_fob_record;
			int sneak_mode;
			int is_win;
			int cluster;
			int is_sneak_restriction;
			MotherBaseParam mother_base_param[4];
			int num_param;
			PlayerPlatformInfo attacker_info;
			Emblem attacker_emblem;
			Espionage attacker_espionage;
			int attacker_sneak_rank_grade;
		};

		struct FobDeployDamageParam
		{
			int expiration_date;
			int damage_values[16];
			int damage_value_count;
			int mother_base_id;
			int cluster_index;
		};

		struct CmdGetFobTargetListOption
		{
			char __pad0[40];
			fox::String msgid;
			char __pad1[32];
			fox::String type;
			int index;
			int num;
		};

		struct CmdSetSecurityChallengeOption
		{
			char __pad0[40];
			fox::String msgid;
			fox::String rqid;
			char __pad1[24];
			fox::String status;
		};

#define SHIFT_CONDITIONAL std::conditional<Shift == 16, fox::ncl::NclJsonMessageResult__shifted0x10, fox::ncl::NclJsonMessageResult>::type
#define SHIFT_CONDITIONAL_ASSERT static_assert(Shift == 16 || Shift == 0);

		template <size_t Shift>
		struct CmdSetSecurityChallengeResult : SHIFT_CONDITIONAL
		{
			SHIFT_CONDITIONAL_ASSERT
		};

		template <size_t Shift>
		struct CmdGetFobTargetListResult : SHIFT_CONDITIONAL
		{
			SHIFT_CONDITIONAL_ASSERT

			fox::String type;
			int target_num;
			int fogTargetListField24;
			FobTargetInfo targets[32];
			int target_count;
			int unk;
			int win;
			int lose;
			int esp_point;
			int event_point;
			int shield_date;
			int enable_security_challenge;
			FobDeployDamageParam fob_deploy_damage_param;
		};

		template <size_t Shift>
		struct CmdGetOwnFobListResult : SHIFT_CONDITIONAL
		{
			SHIFT_CONDITIONAL_ASSERT

			MotherBaseParam fob[4];
			int fob_count;
		};

		template <size_t Shift>
		struct CmdSyncResourceResult : SHIFT_CONDITIONAL
		{
			SHIFT_CONDITIONAL_ASSERT

			int diff_resource1[59];
			int diff_resource1_count;
			int diff_resource2[59];
			int diff_resource2_count;
			int fix_resource1[59];
			int fix_resource1_count;
			int fix_resource2[59];
			int fix_resource2_count;
		};

		static_assert(offsetof(CmdGetFobTargetListResult<16>, target_count) == 0xC0668);
		static_assert(offsetof(CmdGetFobTargetListResult<16>, enable_security_challenge) == 0xC0684);

		struct CmdGetFobTargetDetailResult
		{
			char __pad0[7144];
			std::uint64_t steam_id;
			char __pad1[8];
			fox::SharedString* ip;
			std::uint32_t port;
		};

		struct PlayerData
		{
			fox::String name;
			int play_time;
			int point;
			int league_grade;
			int league_rank;
			int fob_grade;
			int fob_rank;
			int fob_point;
			int espionage_win;
			int espionage_lose;
			int is_insurance;
			int index;
		};

		template <size_t Shift>
		struct CmdGetPlayerlistResult : SHIFT_CONDITIONAL
		{
			SHIFT_CONDITIONAL_ASSERT

			int player_num;
			int pad;
			PlayerData player_data[1];
			int count;
		};

		template <size_t Shift>
		struct CmdSetCurrentplayerResult : SHIFT_CONDITIONAL
		{
			SHIFT_CONDITIONAL_ASSERT

			int player_id;
		};

		struct CmdSyncMotherBaseOption
		{
			char __pad0[148];
			int mother_base_num;
			MotherBaseParam mother_base_param[4];
			int mother_base_count;
			char __pad1[380];
			int name_plate_id;
		};

		struct CmdSyncSoldierBinOption
		{
			char __pad0[136];
			mbm::impl::StaffControllerImpl::Staff soldier_param[3500];
			int soldier_num;
		};

		struct Daemon_sub_1_sub_1_sub2
		{
			steam_id steam_id;
			char name[0x81];
			char __pad0[39];
			bool logged_on;
		};

		struct Daemon_sub_1_sub_1_sub1_vftable
		{
			void* meth_1;
			void* meth_2;
			void* meth_3;
			void* meth_4;
			void* meth_5;
			void* meth_6;
			void* meth_7;
			void* meth_8;
			void* meth_9;
			void* meth_10;
		};

		struct Daemon_sub_1_sub_1_sub1
		{
			Daemon_sub_1_sub_1_sub1_vftable* __vftable;
		};

		struct Daemon_sub_1_sub_1
		{
			Daemon_sub_1_sub_1_sub1 inst;
			Daemon_sub_1_sub_1_sub2* ptr2;
		};

#pragma pack(push, 1)
		struct Daemon_sub_1
		{
			Daemon_sub_1_sub_1* ptr1;
			char __pad1[24];
			int state;
			char __pad2[5];
			char flags;
			char __pad3[2566];
			int unk1;
		};
#pragma pack(pop)

		struct FobP2pNameResolver
		{

		};

		struct Daemon
		{
			char __pad0[16];
			Daemon_sub_1* ptr1;
			char __pad1[56];
			int flags;
			FobP2pNameResolver* nameResolver;
		};

		struct NetworkInfo
		{
			char __pad0[161];
			char opponentName[32];
		};
	}

	static_assert(offsetof(tpp::net::CmdGetFobTargetDetailResult, steam_id) == 7144);
	static_assert(offsetof(tpp::net::CmdGetFobTargetDetailResult, ip) == 7160);
	static_assert(offsetof(tpp::net::CmdGetFobTargetDetailResult, port) == 7168);

	namespace tpp::ui::hud
	{
		struct AnnounceLogViewer
		{
			struct LogModel
			{
				bool initialized;
				char __pad0[7];
				fox::ui::Model* model;
				fox::ui::ModelNodeText* modelNodeText1;
				fox::ui::ModelNodeText* modelNodeText2;
				fox::ui::ModelNode* modelNode;
				fox::ui::ModelNodeMesh* modelNodeMesh;
				char __pad3[8];
				fox::ui::TextUnit textUnit;
			};

			void* a1;
			LogModel logModels[5];
		};
	}

	namespace tpp::gm
	{
		struct ScriptVars_mgo
		{
			char utcTime[8]; // 0
			char totalPlayTime[8]; // 8
			short locationCode; // 16
			short missionCode; // 18
			short prevLocationCode; // 20
			short prevMissionCode; // 22
			int rulesetId; // 24
			int rulesetDynamicPackIndices; // 28
			short mbLayoutCode; // 32
			char mbClusterId; // 34
			char fobSneakMode; // 35
			char fobIsSneak; // 36
			char fobIsEvent; // 37
			char __pad1[2];
			int returnStaffHeader; // 40
			int returnStaffSeeds; // 44
			int eulaVersion; // 48
			int eulaVersion2; // 52
			int eulaVersion3; // 56
			int countryIndex; // 60
			int countryStateIndex; // 64
			int infoId; // 68
			int infoIdForMGO; // 72
			int inquiryId; // 76
			int fobPickup; // 80
			int padType; // 84
			int clock; // 88
			char __pad2[8];
			int weather; // 100
			int weatherNextTime; // 104
			int extraWeatherInterval; // 108
			int weatherAddDensity; // 112
			char requestWeatherType[5]; // 116
			char __pad3[3];
			int requestWeatherUserId[5]; // 124
			int requestWeatherInterp[5]; // 144
			int requestWeatherParam[5]; // 164
			char dominatedCpFlagsAfgh[16]; // 184
			char dominatedCpFlagsMafr[16]; // 200
			int totalAlertCount; // 216
			int initialPlayerPosX; // 220
			int initialPlayerPosY; // 224
			int initialPlayerPosZ; // 228
			int playerPosX; // 232
			int playerPosY; // 236
			int playerPosZ; // 240
			char playerPartsType; // 244
			char playerCamoType; // 245
			char playerHandType; // 246
			char playerType; // 247
			char __pad4[8];
			short playerLifeMax; // 256
			short initialPlayerRotY; // 258
			short playerRotY; // 260
			short initialPlayerFlag; // 262
			char initialPlayerAction; // 264
			char playerRetryFlag; // 265
			short initialPlayerPairGameObjectId; // 266
			short playerVehicleGameObjectId; // 268
			char __pad5[2];
			short vehicleGameObjectIdOnPlayerOnCargo; // 272
			char __pad6[2];
			int playerDisableActionFlag; // 276
			short playerLife; // 280
			short playerStamina; // 282
			int playerCameraPosition[3]; // 284
			char __pad7[16];
			int playerPlayFlag; // 312
			int playerMarkingCountInMission; // 316
			int playerShootCountInMission; // 320
			int shootHitCountInMission; // 324
			int shootHitCountEliminatedInMission; // 328
			int playerOgrePointOnStartMission; // 332
			int playerRecoveryCountFromOgre; // 336
			int totalChickenCapTime; // 340
			int totalChickCapTime; // 344
			int totalDiscoveredCount; // 348
			int totalTakeHitCount; // 352
			int totalMovingDistanceAtStand; // 356
			int totalMovingDistanceAtSquat; // 360
			int totalMovingDistanceAtCrawl; // 364
			int totalMovingDistanceOnHorse; // 368
			int totalMovingDistanceOnVehicle; // 372
			int totalMovingDistanceOnWalkerGear; // 376
			int totalMovingDistanceOnBattleGear; // 380
			int totalMarkingCount; // 384
			int totalReflexCount; // 388
			char useBlackDiamondEmblem; // 392
			char isAvatarPlayerEnable; // 393
			char avatarFaceRaceIndex; // 394
			char avatarFaceTypeIndex; // 395
			char avatarFaceVariationIndex; // 396
			char avatarFaceColorIndex; // 397
			char avatarHairStyleIndex; // 398
			char avatarRightEyeColorIndex; // 399
			char avatarRightEyeBrightnessIndex; // 400
			char avatarLeftEyeColorIndex; // 401
			char avatarLeftEyeBrightnessIndex; // 402
			char avatarHairColor; // 403
			char avatarBerdStyle; // 404
			char avatarBerdLength; // 405
			char avatarEbrwStyle; // 406
			char avatarEbrwWide; // 407
			char avatarGashOrTatoVariationIndex; // 408
			char avatarTatoColorIndex; // 409
			char avatarAcceFlag; // 410
			char avatarMotionFrame[60]; // 411
			char avatarVoiceIndex; // 471
			char avatarVoicePitch; // 472
			char avatarSaveIsValid; // 473
			char avatarEyePresetID; // 474
			char avatarNosePresetID; // 475
			char avatarMouthPresetID; // 476
			char avatarChinPresetID; // 477
			char avatarCheekPresetID; // 478
			char avatarEyebrowPresetID; // 479
			char avatarHeadPresetID; // 480
			char avatarHairPresetID; // 481
			char avatarHairColorPresetID; // 482
			char avatarSkinColorPresetID; // 483
			char avatarBeardPresetID; // 484
			char avatarAccessoryPresetID; // 485
			char avatarAllPartsPresetID; // 486
			char avatarReserve0; // 487
			char avatarReserve1; // 488
			char avatarReserve2; // 489
			char personalName[64]; // 490
			char personalBirthdayMonth; // 554
			char personalBirthdayDay; // 555
			int emblemTextureTag[4]; // 556
			int emblemColorL[4]; // 572
			int emblemColorH[4]; // 588
			char emblemX[4]; // 604
			char emblemY[4]; // 608
			char emblemScale[4]; // 612
			char emblemRotate[4]; // 616
			char emblemFlag[680]; // 620
			char emblemVersion; // 1300
			char emblemSyncFailed; // 1301
			char emblemSyncFailed2; // 1302
			char isDefaultDisconnected; // 1303
			char didCancelFobPatchDlcDownloadRequest; // 1304
			char isPersonalDirty; // 1305
			short initWeapons[3]; // 1306
			short initSupportWeapons[4]; // 1312
			short initItems[8]; // 1320
			short initHandEquip; // 1336
			short weapons[3]; // 1338
			short ammoInWeapons[3]; // 1344
			short ammoSubInWeapons[3]; // 1350
			char suppressorLives[3]; // 1356
			char isUseSuppressor[3]; // 1359
			char isUseFlashLight[3]; // 1362
			char isInitialWeapon[3]; // 1365
			short supportWeapons[4]; // 1368
			short items[8]; // 1376
			char cboxPosterIndices[8]; // 1392
			short handEquip; // 1400
			char upgrades[8]; // 1402
			char ammoStockIds[19]; // 1410
			char __pad8[1];
			short ammoStockCounts[19]; // 1430
			char __pad9[1124];
			short initCustomizedWeapon[3]; // 2592
			short customizedWeapon[3]; // 2598
			char initChimeraParts[36]; // 2604
			char chimeraParts[36]; // 2640
			char initChimeraPaintType[3]; // 2676
			char chimeraPaintType[3]; // 2679
			char initChimeraColorIndex[3]; // 2682
			char chimeraColorIndex[3]; // 2685
			char __pad10[80];
			char itemLevelsDeveloped[74]; // 2768
			char developedItemLevels[74]; // 2842
			short suitDevelopLevel[38]; // 2916
			char initLoadoutInfo; // 2992
			char loadoutInfoSaveFormVersion; // 2993
			char loadoutInfoU8buf[444]; // 2994
			short loadoutInfoU16buf[128]; // 3438
			char __pad11[2];
			int loadoutInfoU32buf[68]; // 3696
			char currentLoadoutSlot; // 3968
			char sortieLoadoutInfoSaveFormVersion; // 3969
			char __pad12[1951];
			char currentForRentLoadoutSlotIndex; // 5921
			char __pad13[2];
			int rentalLoadoutPlayerId; // 5924
			char rentalLoadoutPlayerName[100]; // 5928
			char sortiePrepPlayerSnakePartsType; // 6028
			char sortiePrepPlayerSnakeCamoType; // 6029
			short sortiePrepPlayerSnakeFaceEquipId; // 6030
			char sortiePrepPlayerSnakeSuitLevel; // 6032
			char sortiePrepPlayerDDPartsType; // 6033
			char sortiePrepPlayerDDCamoType; // 6034
			char __pad14[1];
			short sortiePrepPlayerDDFaceEquipId; // 6036
			char sortiePrepPlayerDDSuitLevel; // 6038
			char __pad15[1];
			short userPresetCustomizedWeapon[24]; // 6040
			char userPresetChimeraParts[288]; // 6088
			char userPresetChimeraColorIndex[24]; // 6376
			char userPresetChimeraPaintType[24]; // 6400
			char userPresetDWalkerParts[24]; // 6424
			char userPresetDWalkerPartsLevel[24]; // 6448
			char userPresetDWalkerAdditionalParts[12]; // 6472
			char userPresetDWalkerAdditionalPartsLevel[12]; // 6484
			char userPresetDWalkerCurrentSlot; // 6496
			char userPresetBattleGearParts[27]; // 6497
			char userPresetBattleGearPartsLevel[27]; // 6524
			char userPresetBattleGearCurrentSlot; // 6551
			char userPresetHeliParts[5]; // 6552
			char userPresetVehiclePaintType[10]; // 6557
			char userPresetBuddyDogParts[2]; // 6567
			char userPresetBuddyHorseParts[3]; // 6569
			char mbmUserPresetBaseColor; // 6572
			char __pad16[7];
			int droppedWeaponPos[9]; // 6580
			int droppedWeaponRot[12]; // 6616
			short droppedCount[6]; // 6664
			short droppedAmmo[6]; // 6676
			char droppedSuppressorLife[3]; // 6688
			char __pad17[1];
			short droppedCarrier[3]; // 6692
			char droppedCarrierPlaceNo[3]; // 6698
			char droppedWeaponStatus[3]; // 6701
			char __pad18[64];
			short importantItemNumber[32]; // 6768
			int importantItemPos[96]; // 6832
			int importantItemRot[128]; // 7216
			char importantItemStatus[32]; // 7728
			int markingsOfPickable[33]; // 7760
			short missionCodeMarkingsOfPickable; // 7892
			char __pad19[2];
			int markingsOfPlaced[33]; // 7896
			short missionCodeMarkingsOfPlaced; // 8028
			short placedWeapons[32]; // 8030
			short placedOwnerIds[32]; // 8094
			char __pad20[2];
			int placedPos[96]; // 8160
			int placedRot[128]; // 8544
			int placedAttachTargetNames[64]; // 9056
			short placedAttachTargetIndex[32]; // 9312
			char placedAttachTargetGameObjectType[32]; // 9376
			int placedAttachPoints[64]; // 9408
			short placedCorrelationId[32]; // 9664
			short placedTargetGroupBlast[32]; // 9728
			char placedStatus[32]; // 9792
			short playerDecoyInstanceIndicies[12]; // 9824
			int playerDecoyAngleBallon[12]; // 9848
			short caputreAnimalId[16]; // 9896
			int captureAreaName[16]; // 9928
			char captureAnimalIndex; // 9992
			char __pad21[585];
			char buddyTypeMissionStart; // 10578
			char obtainedBuddyTypeMissionStart; // 10579
			char sortieBuddyTypeMissionStart; // 10580
			char deadBuddyTypeMissionStart; // 10581
			char initialBuddyStatusMissionStart; // 10582
			char __pad22[1];
			int initialBuddyPosMissionStart[3]; // 10584
			int initialBuddyRotYMissionStart; // 10596
			int buddyCommonFlagMissionStart; // 10600
			short buddyFriendlyPointMissionStart[8]; // 10604
			char __pad23[52];
			char buddyType; // 10672
			char obtainedBuddyType; // 10673
			char sortieBuddyType; // 10674
			char disableCallBuddyType; // 10675
			char deadBuddyType; // 10676
			char initialBuddyStatus; // 10677
			char __pad24[2];
			int initialBuddyPos[3]; // 10680
			int initialBuddyRotY; // 10692
			int buddyCommonFlag; // 10696
			int buddyMissionFlag; // 10700
			int buddyCallCount[7]; // 10704
			int buddyEspionageCpNames[8]; // 10732
			int buddyFriendlyTimer[8]; // 10764
			short buddyFriendlyPoint[8]; // 10796
			short buddyFriendlyMaxPoint[8]; // 10812
			char buddyFriendlyCount[64]; // 10828
			short buddyQuietEquipType; // 10892
			char buddyQuietCostumeType; // 10894
			char buddyDogFlag; // 10895
			char buddyDogMode; // 10896
			char buddyDogEquipType; // 10897
			char buddyDogEyeType; // 10898
			char buddyDogColorType; // 10899
			short buddyGearAmmoCount[2]; // 10900
			short buddyGearStockAmmoCount[2]; // 10904
			char buddyGearBodyGrade; // 10908
			char buddyGearEquipGrade; // 10909
			char buddyGearReloadGrade; // 10910
			char buddyGearHeadType; // 10911
			char buddyGearHeadGrade; // 10912
			char buddyGearArmType; // 10913
			char buddyGearArmGrade; // 10914
			char buddyGearMainWPType; // 10915
			char buddyGearMainWPGrade; // 10916
			char buddyGearSubWPType; // 10917
			char buddyGearSubWPGrade; // 10918
			char buddyGearColoring0; // 10919
			char buddyGearColoring1; // 10920
			char buddyHorseType; // 10921
			char buddyHorseColorType; // 10922
			char buddyHorseHeadDesignType; // 10923
			char buddyHorseFootDesignType; // 10924
			char __pad25[1];
			short battleGearAmmoCountMain[2]; // 10926
			short battleGearAmmoCountSub; // 10930
			short battleGearStockAmmoCountMain[2]; // 10932
			short battleGearStockAmmoCountSub; // 10936
			char battleGearMainWPType[2]; // 10938
			char battleGearMainWPGrade[2]; // 10940
			char battleGearSubWPType; // 10942
			char battleGearSubWPGrade; // 10943
			char battleGearColoring0; // 10944
			char battleGearColoring1; // 10945
			char maxPhase; // 10946
			char playerPhase; // 10947
			short sahelanBrokenMesh; // 10948
			char __pad26[2];
			int supportHeliCustomize; // 10952
			int pfRatingPoint; // 10956
			int mbmTppGmp; // 10960
			int mbmTppHeroicPoint; // 10964
			char __pad27[4];
			int mbmTppOgrePoint; // 10972
			char __pad28[4];
			int mbmTppTotalIncomeGmp; // 10980
			char mbmIsOpenedCombatSection; // 10984
			char mbmIsOpenedBaseDevSection; // 10985
			char mbmIsOpenedSpySection; // 10986
			char mbmIsOpenedMedicalSection; // 10987
			char mbmIsOpenedSecuritySection; // 10988
			char mbmIsOpenedHospitalSection; // 10989
			char mbmIsOpenedPrisonSection; // 10990
			char mbmIsOpenedSeparationSection; // 10991
			char mbmSectionFuncSvars[42]; // 10992
			char mbmAutoGmpElapsedTimeMinuteSvars; // 11034
			char mbmAutoResourceElapsedTimeMinuteSvars; // 11035
			char mbmCommonMetalProcessingElapsedTimeMinute; // 11036
			char mbmMinorMetalProcessingElapsedTimeMinute; // 11037
			char mbmPreciousMetalProcessingElapsedTimeMinute; // 11038
			char mbmFuelResourceProcessingElapsedTimeMinute; // 11039
			char mbmBioticResourceProcessingElapsedTimeMinute; // 11040
			char isRussianTranslatable; // 11041
			char isPashtoTranslatable; // 11042
			char isAfrikaansTranslatable; // 11043
			char isKikongoTranslatable; // 11044
			char mbmTreatmentElapsedTimeMinute; // 11045
			char mbmBecomeCombatSectionLvMax; // 11046
			char mbmBecomeDevelopSectionLvMax; // 11047
			char mbmBecomeBaseDevSectionLvMax; // 11048
			char mbmBecomeSupportSectionLvMax; // 11049
			char mbmBecomeSpySectionLvMax; // 11050
			char mbmBecomeMedicalSectionLvMax; // 11051
			char mbmBecomeSecuritySectionLvMax; // 11052
			char mbmBecomeAllSectionLvMax; // 11053
			short mbmHighestTotalSectionLv; // 11054
			int mbmStaffSyncVersion; // 11056
			int mbmStaffSvarsHeaders[3500]; // 11060
			int mbmStaffSvarsSeeds[3500]; // 25060
			int mbmStaffSvarsStatusesSync[3500]; // 39060
			short mbmStaffSvarsStatusesNoSync[3500]; // 53060
			char mbmStaffSkillGots[69]; // 60060
			char mbmStaffSortKeysSvars[13]; // 60129
			char mbmStaffSortDirsSvars[13]; // 60142
			char mbmStaffListMainListModeSvars; // 60155
			char mbmStaffListAssignListModeSvars; // 60156
			char mbmEnmityElapsedTimeMinuteSvars; // 60157
			char mbmMoraleElapsedTimeMinuteSvars; // 60158
			char mbmLanguageElapsedTimeMinuteSvars; // 60159
			char mbmEnableStaffInitLangKikongo; // 60160
			char mbmPandemicEventMode; // 60161
			char mbmPandemicElapsedTimeMinuteSvars; // 60162
			char __pad29[1];
			short mbmPandemicSpreadAttackLimitCount; // 60164
			short mbmPandemicSpreadAttackCount; // 60166
			char mbmDisableKikongoFirst; // 60168
			char mbmMasterGunsmithSkill; // 60169
			char mbmBipedalismWeaponDevelopmentSkill; // 60170
			char mbmBanHeuy; // 60171
			char mbmAssignableMiller; // 60172
			char mbmLockedTanFlag; // 60173
			short mbmStaffInitEnmityLv; // 60174
			char mbmEquipParamStatus[768]; // 60176
			char mbmOnlineOffsetEquipParamStatus[768]; // 60944
			short mbmEquipDevelopIndices[10]; // 61712
			int mbmEquipDevelopTimes[10]; // 61732
			char mbmWeaponPartsReceiverStatus[100]; // 61772
			char mbmWeaponPartsBarrelStatus[32]; // 61872
			char mbmWeaponPartsMagazineStatus[46]; // 61904
			char mbmWeaponPartsStockStatus[35]; // 61950
			char mbmWeaponPartsMuzzleStatus[29]; // 61985
			char mbmWeaponPartsMuzzleOptionStatus[10]; // 62014
			char mbmWeaponPartsSightStatus[25]; // 62024
			char mbmWeaponPartsUnderBarrelStatus[7]; // 62049
			char mbmWeaponPartsLightStatus[5]; // 62056
			char __pad30[3];
			int mbmFobSvars[4]; // 62064
			short mbmClusterSvars[35]; // 62080
			char __pad31[2];
			int mbmClusterTimeMinutes[35]; // 62152
			char mbmCompMb; // 62292
			char __pad32[3];
			int mbmSecurityClusterSvars[35]; // 62296
			char mbmSecuritySpecialPlatformSoldierCounts[35]; // 62436
			char mbmSecuritySpecialPlatformIrSensorCounts[35]; // 62471
			char mbmSecuritySpecialPlatformAntitheftCounts[35]; // 62506
			char mbmSecuritySpecialPlatformCameraCounts[35]; // 62541
			char mbmSecuritySpecialPlatformDecoyCounts[35]; // 62576
			char mbmSecuritySpecialPlatformMineCounts[35]; // 62611
			char mbmSecuritySpecialPlatformUavCounts[35]; // 62646
			char __pad33[3];
			int mbmSecuritySpecialPlatformCautionAreaSvars[35]; // 62684
			char mbmSecurityCommonPlatform1SoldierCounts[35]; // 62824
			char mbmSecurityCommonPlatform1IrSensorCounts[35]; // 62859
			char mbmSecurityCommonPlatform1AntitheftCounts[35]; // 62894
			char mbmSecurityCommonPlatform1CameraCounts[35]; // 62929
			char mbmSecurityCommonPlatform1DecoyCounts[35]; // 62964
			char mbmSecurityCommonPlatform1MineCounts[35]; // 62999
			char mbmSecurityCommonPlatform1UavCounts[35]; // 63034
			char __pad34[3];
			int mbmSecurityCommonPlatform1CautionAreaSvars[35]; // 63072
			char mbmSecurityCommonPlatform2SoldierCounts[35]; // 63212
			char mbmSecurityCommonPlatform2IrSensorCounts[35]; // 63247
			char mbmSecurityCommonPlatform2AntitheftCounts[35]; // 63282
			char mbmSecurityCommonPlatform2CameraCounts[35]; // 63317
			char mbmSecurityCommonPlatform2DecoyCounts[35]; // 63352
			char mbmSecurityCommonPlatform2MineCounts[35]; // 63387
			char mbmSecurityCommonPlatform2UavCounts[35]; // 63422
			char __pad35[3];
			int mbmSecurityCommonPlatform2CautionAreaSvars[35]; // 63460
			char mbmSecurityCommonPlatform3SoldierCounts[35]; // 63600
			char mbmSecurityCommonPlatform3IrSensorCounts[35]; // 63635
			char mbmSecurityCommonPlatform3AntitheftCounts[35]; // 63670
			char mbmSecurityCommonPlatform3CameraCounts[35]; // 63705
			char mbmSecurityCommonPlatform3DecoyCounts[35]; // 63740
			char mbmSecurityCommonPlatform3MineCounts[35]; // 63775
			char mbmSecurityCommonPlatform3UavCounts[35]; // 63810
			char __pad36[3];
			int mbmSecurityCommonPlatform3CautionAreaSvars[35]; // 63848
			int mbmSecuritySpecialPlatformSvars[35]; // 63988
			int mbmSecurityCommonPlatform1Svars[35]; // 64128
			int mbmSecurityCommonPlatform2Svars[35]; // 64268
			int mbmSecurityCommonPlatform3Svars[35]; // 64408
			char mbmInitializedServerWalletFlagDay1; // 64548
			char __pad37[3];
			int mbmResourceSyncVersion; // 64552
			int mbmResourceSvars1Fix[59]; // 64556
			int mbmResourceSvars2Fix[59]; // 64792
			int mbmResourceSvars1Diff[59]; // 65028
			int mbmResourceSvars2Diff[59]; // 65264
			int mbmWhiteContainerCountsSvarsFix[5]; // 65500
			int mbmWhiteContainerCountsSvarsDiff[5]; // 65520
			int mbmRedContainerCountsSvarsFix[5]; // 65540
			int mbmRedContainerCountsSvarsDiff[5]; // 65560
			char mbmOpenedWalkerGear; // 65580
			char mbmDataBaseSvars[459]; // 65581
			char mbmCompAnimal; // 66040
			char mbmCompDesign; // 66041
			char mbmCompPhoto; // 66042
			char __pad38[1];
			int mbmDeployMissionsSvars[71]; // 66044
			char mbmDeployMissionAveRanksSvars[71]; // 66328
			char __pad39[1];
			short mbmDeployTeamCombatStaffCounts[11]; // 66400
			char mbmDeployTeamCombatStaffRankTops[11]; // 66422
			char mbmDeployTeamCombatStaffRankBottoms[11]; // 66433
			short mbmDeployTeamSubStaffCounts[11]; // 66444
			char mbmDeployTeamSubStaffRankTops[11]; // 66466
			char mbmDeployTeamSubStaffRankBottoms[11]; // 66477
			int mbmDeployTeamCombatPoints[11]; // 66488
			int mbmDeployTeamSubPoints[11]; // 66532
			int mbmDeployTeamSvarsTimeMinute[11]; // 66576
			char mbmDeployTeamSvars4wdEastCount[22]; // 66620
			char mbmDeployTeamSvars4wdWestCount[22]; // 66642
			char mbmDeployTeamSvarsTruckEastCount[22]; // 66664
			char mbmDeployTeamSvarsTruckWestCount[22]; // 66686
			char mbmDeployTeamSvarsArmoredVehicleEastCount[22]; // 66708
			char mbmDeployTeamSvarsArmoredVehicleWestCount[22]; // 66730
			char mbmDeployTeamSvarsArmoredVehicleEastRocketCount[22]; // 66752
			char mbmDeployTeamSvarsWheeledArmoredVehicleWestCount[22]; // 66774
			char mbmDeployTeamSvarsTankEastCount[22]; // 66796
			char mbmDeployTeamSvarsTankWestCount[22]; // 66818
			char mbmDeployTeamSvarsWalkerGearProtoHeuyCount[22]; // 66840
			char mbmDeployTeamSvarsWalkerGearSovietBattleCount[22]; // 66862
			char mbmDeployTeamSvarsWalkerGearSovietSupportCount[22]; // 66884
			char mbmDeployTeamSvarsWalkerGearCfaBattleCount[22]; // 66906
			char mbmDeployTeamSvarsWalkerGearCfaSupportCount[22]; // 66928
			char __pad40[2];
			int mbmDeployRandomMissionSeedSvars; // 66952
			short mbmDeploySequentialMissionIdSvars; // 66956
			short mbmDeploySequentialMissionIdLimitSvars; // 66958
			char mbmClearedDeployRevengeMissionFlags[14]; // 66960
			char mbmDeployableBattleGear; // 66974
			char mbmBattleGearDeployingTeam; // 66975
			char mbmAddDeployLogIndex; // 66976
			char mbmCompSequential; // 66977
			short mbmNewestNRandomMissionId; // 66978
			short mbmNewestRRandomMissionId; // 66980
			char mbmOpenedRRandomMission; // 66982
			char mbmNRandomMissionClearedCount; // 66983
			char mbmIsCancelAllDeploy; // 66984
			char __pad41[3];
			int mbmPoolRewardsSyncVersion; // 66988
			int mbmPoolRewardsSvars[200]; // 66992
			int mbmPoolRewardSourceIds[200]; // 67792
			short mbmPoolRewardSeeds[200]; // 68592
			int mbmPoolRewardValues[200]; // 68992
			int mbmS10081CaptiveStaffSvarsSeed; // 69792
			int mbmS10081CaptiveStaffSvarsHeader; // 69796
			int mbmS10081CaptiveStaffSvarsStatusSync; // 69800
			short mbmS10081CaptiveStaffSvarsStatusNoSync; // 69804
			char __pad42[2];
			int mbmRemoverSvarsHeaders[64]; // 69808
			int mbmRemoverSvarsSeeds[64]; // 70064
			int mbmRemoverSvarsStatusesSync[64]; // 70320
			short mbmRemoverSvarsStatusesNoSync[64]; // 70576
			char mbmRemoverReasons[64]; // 70704
			short mbmAddRemoverIndexSvars; // 70768
			char mbmSecuritySystemLevelsDeveloped[16]; // 70770
			char mbmAnimalHabitatAfghSvars[64]; // 70786
			char mbmAnimalHabitatMafrSvars[64]; // 70850
			char mbmIsOccupation; // 70914
			char mbmRequestDemoAttackedFromOtherPlayer; // 70915
			char mbmDemoAttackedFromOtherPlayerKnowWhereFrom; // 70916
			char __pad43[3];
			int mbmFobDefenseWinCount; // 70920
			int mbmFobDefenseLoseCount; // 70924
			char mbmIsContractOutDisplayed; // 70928
			char mbmIsEnableNuclearDevelop; // 70929
			char mbmIsNuclearDeveloped; // 70930
			char mbmIsNuclearDiscarded; // 70931
			char mbmIsNuclearDeveloping; // 70932
			char __pad44[3];
			int mbmNuclearDevelopTime; // 70936
			int mbmNuclearDevelopMaxTime; // 70940
			int mbmPdTotalAddedHostageStaffCount; // 70944
			int mbmPdTotalAddedSoldierStaffCount; // 70948
			int mbmPdTotalAddedVolunteerStaffCount; // 70952
			int mbmPdTotalDeadStaffCount; // 70956
			int mbmPdTotalDefectionCount; // 70960
			int mbmPdTotalFireCount; // 70964
			int mbmPdTotalAddedGmpT; // 70968
			int mbmPdTotalAddedGmp; // 70972
			int mbmPdTotalSubtractedGmpT; // 70976
			int mbmPdTotalSubtractedGmp; // 70980
			int mbmPdTotalAddedHeroicPoint; // 70984
			int mbmPdTotalSubtractedHeroicPoint; // 70988
			int mbmPdTotalAddedResourceCounts[59]; // 70992
			int mbmPdTotalAddedAnimalCounts[99]; // 71228
			char optionSelectedIndices[32]; // 71624
			char armRocketYIsReverse; // 71656
			char armRocketXIsReverse; // 71657
			char bgmVolume; // 71658
			char __pad45[1];
			int keyStr[120]; // 71660
			int buttonNo[120]; // 72140
			int keybordNo[120]; // 72620
			char pushToTalk; // 73100
			char checkPointGimmickIndelibleCount; // 73101
			char __pad46[2];
			int checkPointGimmickIndelibleName[64]; // 73104
			short checkPointGimmickIndelibleState[32]; // 73360
			char missionStartGimmickIndelibleCount; // 73424
			char __pad47[3];
			int missionStartGimmickIndelibleName[64]; // 73428
			short missionStartGimmickIndelibleState[32]; // 73684
			char checkPointGimmickPermanentGimmickSaveData[2592]; // 73748
			char missionStartGimmickPermanentGimmickSaveData[2592]; // 76340
			char __pad48[1];
			char cloakEnabled; // 78933
			char __pad49[360];
			char stealthAssistLeftCount; // 79294
			char __pad50[1];
			int tppTransferFlag; // 79296
			int tppTransferAcquiredFlag; // 79300
			int dlcPermittedFlag[2]; // 79304
			int dlcAcquiredFlag[2]; // 79312
			int mbDvcTutorialFlag; // 79320
			int mbDvcTopMenuFlag; // 79324
			char heliMusicType; // 79328
			char __pad51[3];
			int heliMusicTrackID; // 79332
			char __pad52[4];
			char numChar; // 79340
			char lastUsedChar; // 79341
			char selectedBGM; // 79342
			char __pad53[1];
			int savedPermanents[16]; // 79344
			char charName[512]; // 79408
			char charClass[8]; // 79920
			char __pad54[8];
			char savedPrestige[8]; // 79936
			int savedXP[8]; // 79944
			int savedPeakXP[8]; // 79976
			char playerTypes[8]; // 80008
			char lastLoadout[8]; // 80016
			char mgoAvatarFaceRaceIndex[8]; // 80024
			char mgoAvatarFaceTypeIndex[8]; // 80032
			char mgoAvatarFaceVariationIndex[8]; // 80040
			char mgoAvatarFaceColorIndex[8]; // 80048
			char mgoAvatarHairStyleIndex[8]; // 80056
			char mgoAvatarRightEyeColorIndex[8]; // 80064
			char mgoAvatarRightEyeBrightnessIndex[8]; // 80072
			char mgoAvatarLeftEyeColorIndex[8]; // 80080
			char mgoAvatarLeftEyeBrightnessIndex[8]; // 80088
			char mgoAvatarHairColor[8]; // 80096
			char mgoAvatarBerdStyle[8]; // 80104
			char mgoAvatarBerdLength[8]; // 80112
			char mgoAvatarEbrwStyle[8]; // 80120
			char mgoAvatarEbrwWide[8]; // 80128
			char mgoAvatarGashOrTatoVariationIndex[8]; // 80136
			char mgoAvatarTatoColorIndex[8]; // 80144
			char mgoAvatarAcceFlag[8]; // 80152
			char mgoAvatarMotionFrames[480]; // 80160
			char mgoAvatarVoiceIndex[8]; // 80640
			char mgoAvatarVoicePitch[8]; // 80648
			char numLoadout[8]; // 80656
			char loadoutName[3584]; // 80664
			int loadoutPrimary1[56]; // 84248
			int loadoutPrimary2[56]; // 84472
			int loadoutSecondary[56]; // 84696
			int loadoutParts[1008]; // 84920
			char loadoutColor[336]; // 88952
			int loadoutSupport[224]; // 89288
			int loadoutItem[448]; // 90184
			int loadoutSkill[224]; // 91976
			int gearData[120]; // 92872
			int appealActionSlots[40]; // 93352
			char appealActionSoundType[8]; // 93512
			char optionShowMissionInfo; // 93520
			char optionShowTmpInfo; // 93521
			char optionShowCombatInfo; // 93522
			char __pad55[1];
			short presetRadio[50]; // 93524
			short presetRadioMarkers[15]; // 93624
			char __pad56[2];
			int gearState[15]; // 93656
			char isNight; // 93716
			char roundTickets; // 93717
			char isAvatarEditMode; // 93718
			char cancelAvatarEdit; // 93719
			int roundTimeLimit; // 93720
			int briefingTime; // 93724
			char walkerGear; // 93728
			char __pad57[3];
			int memberMin; // 93732
			int specialRole; // 93736
			int uniqueCharacterSelect; // 93740
			char isGameplayHost; // 93744
			char hasDisplayedTitleScreen; // 93745
			char isCustomizeMenu; // 93746
			char isPlayerInCustomizeMenu; // 93747
			char doesNeedToRespawnOnceInCustomize; // 93748
			char isInitialTutorialFinished; // 93749
			char gamePlayTutorialCount; // 93750
			char isNoContest; // 93751
			char didSurvivalTicketNotice; // 93752
			char __pad58[3];
			int dlcFlags; // 93756
			int mgoEulaVersion; // 93760
			char searchSettingsMapIndex; // 93764
			char searchSettingsRuleIndex; // 93765
			char __pad59[2];
			int searchSettingsOthers; // 93768
			int searchSettingsSurvival; // 93772
			char isSavedHostCommentV110; // 93776
			char __pad60[3];
			int survivalHostCommentIndex; // 93780
			int survivalRoomId; // 93784
			char __pad61[1480];
			short userMarkerLocationId; // 95268
			char userMarkerSaveCount; // 95270
		};

		static_assert(sizeof(ScriptVars_mgo) == 95272);
		static_assert(offsetof(ScriptVars_mgo, utcTime) == 0);
		static_assert(offsetof(ScriptVars_mgo, totalPlayTime) == 8);
		static_assert(offsetof(ScriptVars_mgo, locationCode) == 16);
		static_assert(offsetof(ScriptVars_mgo, missionCode) == 18);
		static_assert(offsetof(ScriptVars_mgo, prevLocationCode) == 20);
		static_assert(offsetof(ScriptVars_mgo, prevMissionCode) == 22);
		static_assert(offsetof(ScriptVars_mgo, rulesetDynamicPackIndices) == 28);
		static_assert(offsetof(ScriptVars_mgo, mbLayoutCode) == 32);
		static_assert(offsetof(ScriptVars_mgo, mbClusterId) == 34);
		static_assert(offsetof(ScriptVars_mgo, fobSneakMode) == 35);
		static_assert(offsetof(ScriptVars_mgo, fobIsSneak) == 36);
		static_assert(offsetof(ScriptVars_mgo, fobIsEvent) == 37);
		static_assert(offsetof(ScriptVars_mgo, returnStaffHeader) == 40);
		static_assert(offsetof(ScriptVars_mgo, returnStaffSeeds) == 44);
		static_assert(offsetof(ScriptVars_mgo, eulaVersion) == 48);
		static_assert(offsetof(ScriptVars_mgo, eulaVersion2) == 52);
		static_assert(offsetof(ScriptVars_mgo, eulaVersion3) == 56);
		static_assert(offsetof(ScriptVars_mgo, countryIndex) == 60);
		static_assert(offsetof(ScriptVars_mgo, countryStateIndex) == 64);
		static_assert(offsetof(ScriptVars_mgo, infoId) == 68);
		static_assert(offsetof(ScriptVars_mgo, infoIdForMGO) == 72);
		static_assert(offsetof(ScriptVars_mgo, inquiryId) == 76);
		static_assert(offsetof(ScriptVars_mgo, fobPickup) == 80);
		static_assert(offsetof(ScriptVars_mgo, padType) == 84);
		static_assert(offsetof(ScriptVars_mgo, clock) == 88);
		static_assert(offsetof(ScriptVars_mgo, weather) == 100);
		static_assert(offsetof(ScriptVars_mgo, weatherNextTime) == 104);
		static_assert(offsetof(ScriptVars_mgo, extraWeatherInterval) == 108);
		static_assert(offsetof(ScriptVars_mgo, weatherAddDensity) == 112);
		static_assert(offsetof(ScriptVars_mgo, requestWeatherType) == 116);
		static_assert(offsetof(ScriptVars_mgo, requestWeatherUserId) == 124);
		static_assert(offsetof(ScriptVars_mgo, requestWeatherInterp) == 144);
		static_assert(offsetof(ScriptVars_mgo, requestWeatherParam) == 164);
		static_assert(offsetof(ScriptVars_mgo, dominatedCpFlagsAfgh) == 184);
		static_assert(offsetof(ScriptVars_mgo, dominatedCpFlagsMafr) == 200);
		static_assert(offsetof(ScriptVars_mgo, totalAlertCount) == 216);
		static_assert(offsetof(ScriptVars_mgo, initialPlayerPosX) == 220);
		static_assert(offsetof(ScriptVars_mgo, initialPlayerPosY) == 224);
		static_assert(offsetof(ScriptVars_mgo, initialPlayerPosZ) == 228);
		static_assert(offsetof(ScriptVars_mgo, playerPosX) == 232);
		static_assert(offsetof(ScriptVars_mgo, playerPosY) == 236);
		static_assert(offsetof(ScriptVars_mgo, playerPosZ) == 240);
		static_assert(offsetof(ScriptVars_mgo, playerPartsType) == 244);
		static_assert(offsetof(ScriptVars_mgo, playerCamoType) == 245);
		static_assert(offsetof(ScriptVars_mgo, playerHandType) == 246);
		static_assert(offsetof(ScriptVars_mgo, playerType) == 247);
		static_assert(offsetof(ScriptVars_mgo, playerLifeMax) == 256);
		static_assert(offsetof(ScriptVars_mgo, initialPlayerRotY) == 258);
		static_assert(offsetof(ScriptVars_mgo, playerRotY) == 260);
		static_assert(offsetof(ScriptVars_mgo, initialPlayerFlag) == 262);
		static_assert(offsetof(ScriptVars_mgo, initialPlayerAction) == 264);
		static_assert(offsetof(ScriptVars_mgo, playerRetryFlag) == 265);
		static_assert(offsetof(ScriptVars_mgo, initialPlayerPairGameObjectId) == 266);
		static_assert(offsetof(ScriptVars_mgo, playerVehicleGameObjectId) == 268);
		static_assert(offsetof(ScriptVars_mgo, vehicleGameObjectIdOnPlayerOnCargo) == 272);
		static_assert(offsetof(ScriptVars_mgo, playerDisableActionFlag) == 276);
		static_assert(offsetof(ScriptVars_mgo, playerLife) == 280);
		static_assert(offsetof(ScriptVars_mgo, playerStamina) == 282);
		static_assert(offsetof(ScriptVars_mgo, playerCameraPosition) == 284);
		static_assert(offsetof(ScriptVars_mgo, playerPlayFlag) == 312);
		static_assert(offsetof(ScriptVars_mgo, playerMarkingCountInMission) == 316);
		static_assert(offsetof(ScriptVars_mgo, playerShootCountInMission) == 320);
		static_assert(offsetof(ScriptVars_mgo, shootHitCountInMission) == 324);
		static_assert(offsetof(ScriptVars_mgo, shootHitCountEliminatedInMission) == 328);
		static_assert(offsetof(ScriptVars_mgo, playerOgrePointOnStartMission) == 332);
		static_assert(offsetof(ScriptVars_mgo, playerRecoveryCountFromOgre) == 336);
		static_assert(offsetof(ScriptVars_mgo, totalChickenCapTime) == 340);
		static_assert(offsetof(ScriptVars_mgo, totalChickCapTime) == 344);
		static_assert(offsetof(ScriptVars_mgo, totalDiscoveredCount) == 348);
		static_assert(offsetof(ScriptVars_mgo, totalTakeHitCount) == 352);
		static_assert(offsetof(ScriptVars_mgo, totalMovingDistanceAtStand) == 356);
		static_assert(offsetof(ScriptVars_mgo, totalMovingDistanceAtSquat) == 360);
		static_assert(offsetof(ScriptVars_mgo, totalMovingDistanceAtCrawl) == 364);
		static_assert(offsetof(ScriptVars_mgo, totalMovingDistanceOnHorse) == 368);
		static_assert(offsetof(ScriptVars_mgo, totalMovingDistanceOnVehicle) == 372);
		static_assert(offsetof(ScriptVars_mgo, totalMovingDistanceOnWalkerGear) == 376);
		static_assert(offsetof(ScriptVars_mgo, totalMovingDistanceOnBattleGear) == 380);
		static_assert(offsetof(ScriptVars_mgo, totalMarkingCount) == 384);
		static_assert(offsetof(ScriptVars_mgo, totalReflexCount) == 388);
		static_assert(offsetof(ScriptVars_mgo, useBlackDiamondEmblem) == 392);
		static_assert(offsetof(ScriptVars_mgo, isAvatarPlayerEnable) == 393);
		static_assert(offsetof(ScriptVars_mgo, avatarFaceRaceIndex) == 394);
		static_assert(offsetof(ScriptVars_mgo, avatarFaceTypeIndex) == 395);
		static_assert(offsetof(ScriptVars_mgo, avatarFaceVariationIndex) == 396);
		static_assert(offsetof(ScriptVars_mgo, avatarFaceColorIndex) == 397);
		static_assert(offsetof(ScriptVars_mgo, avatarHairStyleIndex) == 398);
		static_assert(offsetof(ScriptVars_mgo, avatarRightEyeColorIndex) == 399);
		static_assert(offsetof(ScriptVars_mgo, avatarRightEyeBrightnessIndex) == 400);
		static_assert(offsetof(ScriptVars_mgo, avatarLeftEyeColorIndex) == 401);
		static_assert(offsetof(ScriptVars_mgo, avatarLeftEyeBrightnessIndex) == 402);
		static_assert(offsetof(ScriptVars_mgo, avatarHairColor) == 403);
		static_assert(offsetof(ScriptVars_mgo, avatarBerdStyle) == 404);
		static_assert(offsetof(ScriptVars_mgo, avatarBerdLength) == 405);
		static_assert(offsetof(ScriptVars_mgo, avatarEbrwStyle) == 406);
		static_assert(offsetof(ScriptVars_mgo, avatarEbrwWide) == 407);
		static_assert(offsetof(ScriptVars_mgo, avatarGashOrTatoVariationIndex) == 408);
		static_assert(offsetof(ScriptVars_mgo, avatarTatoColorIndex) == 409);
		static_assert(offsetof(ScriptVars_mgo, avatarAcceFlag) == 410);
		static_assert(offsetof(ScriptVars_mgo, avatarMotionFrame) == 411);
		static_assert(offsetof(ScriptVars_mgo, avatarVoiceIndex) == 471);
		static_assert(offsetof(ScriptVars_mgo, avatarVoicePitch) == 472);
		static_assert(offsetof(ScriptVars_mgo, avatarSaveIsValid) == 473);
		static_assert(offsetof(ScriptVars_mgo, avatarEyePresetID) == 474);
		static_assert(offsetof(ScriptVars_mgo, avatarNosePresetID) == 475);
		static_assert(offsetof(ScriptVars_mgo, avatarMouthPresetID) == 476);
		static_assert(offsetof(ScriptVars_mgo, avatarChinPresetID) == 477);
		static_assert(offsetof(ScriptVars_mgo, avatarCheekPresetID) == 478);
		static_assert(offsetof(ScriptVars_mgo, avatarEyebrowPresetID) == 479);
		static_assert(offsetof(ScriptVars_mgo, avatarHeadPresetID) == 480);
		static_assert(offsetof(ScriptVars_mgo, avatarHairPresetID) == 481);
		static_assert(offsetof(ScriptVars_mgo, avatarHairColorPresetID) == 482);
		static_assert(offsetof(ScriptVars_mgo, avatarSkinColorPresetID) == 483);
		static_assert(offsetof(ScriptVars_mgo, avatarBeardPresetID) == 484);
		static_assert(offsetof(ScriptVars_mgo, avatarAccessoryPresetID) == 485);
		static_assert(offsetof(ScriptVars_mgo, avatarAllPartsPresetID) == 486);
		static_assert(offsetof(ScriptVars_mgo, avatarReserve0) == 487);
		static_assert(offsetof(ScriptVars_mgo, avatarReserve1) == 488);
		static_assert(offsetof(ScriptVars_mgo, avatarReserve2) == 489);
		static_assert(offsetof(ScriptVars_mgo, personalName) == 490);
		static_assert(offsetof(ScriptVars_mgo, personalBirthdayMonth) == 554);
		static_assert(offsetof(ScriptVars_mgo, personalBirthdayDay) == 555);
		static_assert(offsetof(ScriptVars_mgo, emblemTextureTag) == 556);
		static_assert(offsetof(ScriptVars_mgo, emblemColorL) == 572);
		static_assert(offsetof(ScriptVars_mgo, emblemColorH) == 588);
		static_assert(offsetof(ScriptVars_mgo, emblemX) == 604);
		static_assert(offsetof(ScriptVars_mgo, emblemY) == 608);
		static_assert(offsetof(ScriptVars_mgo, emblemScale) == 612);
		static_assert(offsetof(ScriptVars_mgo, emblemRotate) == 616);
		static_assert(offsetof(ScriptVars_mgo, emblemFlag) == 620);
		static_assert(offsetof(ScriptVars_mgo, emblemVersion) == 1300);
		static_assert(offsetof(ScriptVars_mgo, emblemSyncFailed) == 1301);
		static_assert(offsetof(ScriptVars_mgo, emblemSyncFailed2) == 1302);
		static_assert(offsetof(ScriptVars_mgo, isDefaultDisconnected) == 1303);
		static_assert(offsetof(ScriptVars_mgo, didCancelFobPatchDlcDownloadRequest) == 1304);
		static_assert(offsetof(ScriptVars_mgo, isPersonalDirty) == 1305);
		static_assert(offsetof(ScriptVars_mgo, initWeapons) == 1306);
		static_assert(offsetof(ScriptVars_mgo, initSupportWeapons) == 1312);
		static_assert(offsetof(ScriptVars_mgo, initItems) == 1320);
		static_assert(offsetof(ScriptVars_mgo, initHandEquip) == 1336);
		static_assert(offsetof(ScriptVars_mgo, weapons) == 1338);
		static_assert(offsetof(ScriptVars_mgo, ammoInWeapons) == 1344);
		static_assert(offsetof(ScriptVars_mgo, ammoSubInWeapons) == 1350);
		static_assert(offsetof(ScriptVars_mgo, suppressorLives) == 1356);
		static_assert(offsetof(ScriptVars_mgo, isUseSuppressor) == 1359);
		static_assert(offsetof(ScriptVars_mgo, isUseFlashLight) == 1362);
		static_assert(offsetof(ScriptVars_mgo, isInitialWeapon) == 1365);
		static_assert(offsetof(ScriptVars_mgo, supportWeapons) == 1368);
		static_assert(offsetof(ScriptVars_mgo, items) == 1376);
		static_assert(offsetof(ScriptVars_mgo, cboxPosterIndices) == 1392);
		static_assert(offsetof(ScriptVars_mgo, handEquip) == 1400);
		static_assert(offsetof(ScriptVars_mgo, upgrades) == 1402);
		static_assert(offsetof(ScriptVars_mgo, ammoStockIds) == 1410);
		static_assert(offsetof(ScriptVars_mgo, ammoStockCounts) == 1430);
		static_assert(offsetof(ScriptVars_mgo, initCustomizedWeapon) == 2592);
		static_assert(offsetof(ScriptVars_mgo, customizedWeapon) == 2598);
		static_assert(offsetof(ScriptVars_mgo, initChimeraParts) == 2604);
		static_assert(offsetof(ScriptVars_mgo, chimeraParts) == 2640);
		static_assert(offsetof(ScriptVars_mgo, initChimeraPaintType) == 2676);
		static_assert(offsetof(ScriptVars_mgo, chimeraPaintType) == 2679);
		static_assert(offsetof(ScriptVars_mgo, initChimeraColorIndex) == 2682);
		static_assert(offsetof(ScriptVars_mgo, chimeraColorIndex) == 2685);
		static_assert(offsetof(ScriptVars_mgo, itemLevelsDeveloped) == 2768);
		static_assert(offsetof(ScriptVars_mgo, developedItemLevels) == 2842);
		static_assert(offsetof(ScriptVars_mgo, suitDevelopLevel) == 2916);
		static_assert(offsetof(ScriptVars_mgo, initLoadoutInfo) == 2992);
		static_assert(offsetof(ScriptVars_mgo, loadoutInfoSaveFormVersion) == 2993);
		static_assert(offsetof(ScriptVars_mgo, loadoutInfoU8buf) == 2994);
		static_assert(offsetof(ScriptVars_mgo, loadoutInfoU16buf) == 3438);
		static_assert(offsetof(ScriptVars_mgo, loadoutInfoU32buf) == 3696);
		static_assert(offsetof(ScriptVars_mgo, currentLoadoutSlot) == 3968);
		static_assert(offsetof(ScriptVars_mgo, sortieLoadoutInfoSaveFormVersion) == 3969);
		static_assert(offsetof(ScriptVars_mgo, currentForRentLoadoutSlotIndex) == 5921);
		static_assert(offsetof(ScriptVars_mgo, rentalLoadoutPlayerId) == 5924);
		static_assert(offsetof(ScriptVars_mgo, rentalLoadoutPlayerName) == 5928);
		static_assert(offsetof(ScriptVars_mgo, sortiePrepPlayerSnakePartsType) == 6028);
		static_assert(offsetof(ScriptVars_mgo, sortiePrepPlayerSnakeCamoType) == 6029);
		static_assert(offsetof(ScriptVars_mgo, sortiePrepPlayerSnakeFaceEquipId) == 6030);
		static_assert(offsetof(ScriptVars_mgo, sortiePrepPlayerSnakeSuitLevel) == 6032);
		static_assert(offsetof(ScriptVars_mgo, sortiePrepPlayerDDPartsType) == 6033);
		static_assert(offsetof(ScriptVars_mgo, sortiePrepPlayerDDCamoType) == 6034);
		static_assert(offsetof(ScriptVars_mgo, sortiePrepPlayerDDFaceEquipId) == 6036);
		static_assert(offsetof(ScriptVars_mgo, sortiePrepPlayerDDSuitLevel) == 6038);
		static_assert(offsetof(ScriptVars_mgo, userPresetCustomizedWeapon) == 6040);
		static_assert(offsetof(ScriptVars_mgo, userPresetChimeraParts) == 6088);
		static_assert(offsetof(ScriptVars_mgo, userPresetChimeraColorIndex) == 6376);
		static_assert(offsetof(ScriptVars_mgo, userPresetChimeraPaintType) == 6400);
		static_assert(offsetof(ScriptVars_mgo, userPresetDWalkerParts) == 6424);
		static_assert(offsetof(ScriptVars_mgo, userPresetDWalkerPartsLevel) == 6448);
		static_assert(offsetof(ScriptVars_mgo, userPresetDWalkerAdditionalParts) == 6472);
		static_assert(offsetof(ScriptVars_mgo, userPresetDWalkerAdditionalPartsLevel) == 6484);
		static_assert(offsetof(ScriptVars_mgo, userPresetDWalkerCurrentSlot) == 6496);
		static_assert(offsetof(ScriptVars_mgo, userPresetBattleGearParts) == 6497);
		static_assert(offsetof(ScriptVars_mgo, userPresetBattleGearPartsLevel) == 6524);
		static_assert(offsetof(ScriptVars_mgo, userPresetBattleGearCurrentSlot) == 6551);
		static_assert(offsetof(ScriptVars_mgo, userPresetHeliParts) == 6552);
		static_assert(offsetof(ScriptVars_mgo, userPresetVehiclePaintType) == 6557);
		static_assert(offsetof(ScriptVars_mgo, userPresetBuddyDogParts) == 6567);
		static_assert(offsetof(ScriptVars_mgo, userPresetBuddyHorseParts) == 6569);
		static_assert(offsetof(ScriptVars_mgo, mbmUserPresetBaseColor) == 6572);
		static_assert(offsetof(ScriptVars_mgo, droppedWeaponPos) == 6580);
		static_assert(offsetof(ScriptVars_mgo, droppedWeaponRot) == 6616);
		static_assert(offsetof(ScriptVars_mgo, droppedCount) == 6664);
		static_assert(offsetof(ScriptVars_mgo, droppedAmmo) == 6676);
		static_assert(offsetof(ScriptVars_mgo, droppedSuppressorLife) == 6688);
		static_assert(offsetof(ScriptVars_mgo, droppedCarrier) == 6692);
		static_assert(offsetof(ScriptVars_mgo, droppedCarrierPlaceNo) == 6698);
		static_assert(offsetof(ScriptVars_mgo, droppedWeaponStatus) == 6701);
		static_assert(offsetof(ScriptVars_mgo, importantItemNumber) == 6768);
		static_assert(offsetof(ScriptVars_mgo, importantItemPos) == 6832);
		static_assert(offsetof(ScriptVars_mgo, importantItemRot) == 7216);
		static_assert(offsetof(ScriptVars_mgo, importantItemStatus) == 7728);
		static_assert(offsetof(ScriptVars_mgo, markingsOfPickable) == 7760);
		static_assert(offsetof(ScriptVars_mgo, missionCodeMarkingsOfPickable) == 7892);
		static_assert(offsetof(ScriptVars_mgo, markingsOfPlaced) == 7896);
		static_assert(offsetof(ScriptVars_mgo, missionCodeMarkingsOfPlaced) == 8028);
		static_assert(offsetof(ScriptVars_mgo, placedWeapons) == 8030);
		static_assert(offsetof(ScriptVars_mgo, placedOwnerIds) == 8094);
		static_assert(offsetof(ScriptVars_mgo, placedPos) == 8160);
		static_assert(offsetof(ScriptVars_mgo, placedRot) == 8544);
		static_assert(offsetof(ScriptVars_mgo, placedAttachTargetNames) == 9056);
		static_assert(offsetof(ScriptVars_mgo, placedAttachTargetIndex) == 9312);
		static_assert(offsetof(ScriptVars_mgo, placedAttachTargetGameObjectType) == 9376);
		static_assert(offsetof(ScriptVars_mgo, placedAttachPoints) == 9408);
		static_assert(offsetof(ScriptVars_mgo, placedCorrelationId) == 9664);
		static_assert(offsetof(ScriptVars_mgo, placedTargetGroupBlast) == 9728);
		static_assert(offsetof(ScriptVars_mgo, placedStatus) == 9792);
		static_assert(offsetof(ScriptVars_mgo, playerDecoyInstanceIndicies) == 9824);
		static_assert(offsetof(ScriptVars_mgo, playerDecoyAngleBallon) == 9848);
		static_assert(offsetof(ScriptVars_mgo, caputreAnimalId) == 9896);
		static_assert(offsetof(ScriptVars_mgo, captureAreaName) == 9928);
		static_assert(offsetof(ScriptVars_mgo, captureAnimalIndex) == 9992);
		static_assert(offsetof(ScriptVars_mgo, buddyTypeMissionStart) == 10578);
		static_assert(offsetof(ScriptVars_mgo, obtainedBuddyTypeMissionStart) == 10579);
		static_assert(offsetof(ScriptVars_mgo, sortieBuddyTypeMissionStart) == 10580);
		static_assert(offsetof(ScriptVars_mgo, deadBuddyTypeMissionStart) == 10581);
		static_assert(offsetof(ScriptVars_mgo, initialBuddyStatusMissionStart) == 10582);
		static_assert(offsetof(ScriptVars_mgo, initialBuddyPosMissionStart) == 10584);
		static_assert(offsetof(ScriptVars_mgo, initialBuddyRotYMissionStart) == 10596);
		static_assert(offsetof(ScriptVars_mgo, buddyCommonFlagMissionStart) == 10600);
		static_assert(offsetof(ScriptVars_mgo, buddyFriendlyPointMissionStart) == 10604);
		static_assert(offsetof(ScriptVars_mgo, buddyType) == 10672);
		static_assert(offsetof(ScriptVars_mgo, obtainedBuddyType) == 10673);
		static_assert(offsetof(ScriptVars_mgo, sortieBuddyType) == 10674);
		static_assert(offsetof(ScriptVars_mgo, disableCallBuddyType) == 10675);
		static_assert(offsetof(ScriptVars_mgo, deadBuddyType) == 10676);
		static_assert(offsetof(ScriptVars_mgo, initialBuddyStatus) == 10677);
		static_assert(offsetof(ScriptVars_mgo, initialBuddyPos) == 10680);
		static_assert(offsetof(ScriptVars_mgo, initialBuddyRotY) == 10692);
		static_assert(offsetof(ScriptVars_mgo, buddyCommonFlag) == 10696);
		static_assert(offsetof(ScriptVars_mgo, buddyMissionFlag) == 10700);
		static_assert(offsetof(ScriptVars_mgo, buddyCallCount) == 10704);
		static_assert(offsetof(ScriptVars_mgo, buddyEspionageCpNames) == 10732);
		static_assert(offsetof(ScriptVars_mgo, buddyFriendlyTimer) == 10764);
		static_assert(offsetof(ScriptVars_mgo, buddyFriendlyPoint) == 10796);
		static_assert(offsetof(ScriptVars_mgo, buddyFriendlyMaxPoint) == 10812);
		static_assert(offsetof(ScriptVars_mgo, buddyFriendlyCount) == 10828);
		static_assert(offsetof(ScriptVars_mgo, buddyQuietEquipType) == 10892);
		static_assert(offsetof(ScriptVars_mgo, buddyQuietCostumeType) == 10894);
		static_assert(offsetof(ScriptVars_mgo, buddyDogFlag) == 10895);
		static_assert(offsetof(ScriptVars_mgo, buddyDogMode) == 10896);
		static_assert(offsetof(ScriptVars_mgo, buddyDogEquipType) == 10897);
		static_assert(offsetof(ScriptVars_mgo, buddyDogEyeType) == 10898);
		static_assert(offsetof(ScriptVars_mgo, buddyDogColorType) == 10899);
		static_assert(offsetof(ScriptVars_mgo, buddyGearAmmoCount) == 10900);
		static_assert(offsetof(ScriptVars_mgo, buddyGearStockAmmoCount) == 10904);
		static_assert(offsetof(ScriptVars_mgo, buddyGearBodyGrade) == 10908);
		static_assert(offsetof(ScriptVars_mgo, buddyGearEquipGrade) == 10909);
		static_assert(offsetof(ScriptVars_mgo, buddyGearReloadGrade) == 10910);
		static_assert(offsetof(ScriptVars_mgo, buddyGearHeadType) == 10911);
		static_assert(offsetof(ScriptVars_mgo, buddyGearHeadGrade) == 10912);
		static_assert(offsetof(ScriptVars_mgo, buddyGearArmType) == 10913);
		static_assert(offsetof(ScriptVars_mgo, buddyGearArmGrade) == 10914);
		static_assert(offsetof(ScriptVars_mgo, buddyGearMainWPType) == 10915);
		static_assert(offsetof(ScriptVars_mgo, buddyGearMainWPGrade) == 10916);
		static_assert(offsetof(ScriptVars_mgo, buddyGearSubWPType) == 10917);
		static_assert(offsetof(ScriptVars_mgo, buddyGearSubWPGrade) == 10918);
		static_assert(offsetof(ScriptVars_mgo, buddyGearColoring0) == 10919);
		static_assert(offsetof(ScriptVars_mgo, buddyGearColoring1) == 10920);
		static_assert(offsetof(ScriptVars_mgo, buddyHorseType) == 10921);
		static_assert(offsetof(ScriptVars_mgo, buddyHorseColorType) == 10922);
		static_assert(offsetof(ScriptVars_mgo, buddyHorseHeadDesignType) == 10923);
		static_assert(offsetof(ScriptVars_mgo, buddyHorseFootDesignType) == 10924);
		static_assert(offsetof(ScriptVars_mgo, battleGearAmmoCountMain) == 10926);
		static_assert(offsetof(ScriptVars_mgo, battleGearAmmoCountSub) == 10930);
		static_assert(offsetof(ScriptVars_mgo, battleGearStockAmmoCountMain) == 10932);
		static_assert(offsetof(ScriptVars_mgo, battleGearStockAmmoCountSub) == 10936);
		static_assert(offsetof(ScriptVars_mgo, battleGearMainWPType) == 10938);
		static_assert(offsetof(ScriptVars_mgo, battleGearMainWPGrade) == 10940);
		static_assert(offsetof(ScriptVars_mgo, battleGearSubWPType) == 10942);
		static_assert(offsetof(ScriptVars_mgo, battleGearSubWPGrade) == 10943);
		static_assert(offsetof(ScriptVars_mgo, battleGearColoring0) == 10944);
		static_assert(offsetof(ScriptVars_mgo, battleGearColoring1) == 10945);
		static_assert(offsetof(ScriptVars_mgo, maxPhase) == 10946);
		static_assert(offsetof(ScriptVars_mgo, playerPhase) == 10947);
		static_assert(offsetof(ScriptVars_mgo, sahelanBrokenMesh) == 10948);
		static_assert(offsetof(ScriptVars_mgo, supportHeliCustomize) == 10952);
		static_assert(offsetof(ScriptVars_mgo, pfRatingPoint) == 10956);
		static_assert(offsetof(ScriptVars_mgo, mbmTppGmp) == 10960);
		static_assert(offsetof(ScriptVars_mgo, mbmTppHeroicPoint) == 10964);
		static_assert(offsetof(ScriptVars_mgo, mbmTppOgrePoint) == 10972);
		static_assert(offsetof(ScriptVars_mgo, mbmTppTotalIncomeGmp) == 10980);
		static_assert(offsetof(ScriptVars_mgo, mbmIsOpenedCombatSection) == 10984);
		static_assert(offsetof(ScriptVars_mgo, mbmIsOpenedBaseDevSection) == 10985);
		static_assert(offsetof(ScriptVars_mgo, mbmIsOpenedSpySection) == 10986);
		static_assert(offsetof(ScriptVars_mgo, mbmIsOpenedMedicalSection) == 10987);
		static_assert(offsetof(ScriptVars_mgo, mbmIsOpenedSecuritySection) == 10988);
		static_assert(offsetof(ScriptVars_mgo, mbmIsOpenedHospitalSection) == 10989);
		static_assert(offsetof(ScriptVars_mgo, mbmIsOpenedPrisonSection) == 10990);
		static_assert(offsetof(ScriptVars_mgo, mbmIsOpenedSeparationSection) == 10991);
		static_assert(offsetof(ScriptVars_mgo, mbmSectionFuncSvars) == 10992);
		static_assert(offsetof(ScriptVars_mgo, mbmAutoGmpElapsedTimeMinuteSvars) == 11034);
		static_assert(offsetof(ScriptVars_mgo, mbmAutoResourceElapsedTimeMinuteSvars) == 11035);
		static_assert(offsetof(ScriptVars_mgo, mbmCommonMetalProcessingElapsedTimeMinute) == 11036);
		static_assert(offsetof(ScriptVars_mgo, mbmMinorMetalProcessingElapsedTimeMinute) == 11037);
		static_assert(offsetof(ScriptVars_mgo, mbmPreciousMetalProcessingElapsedTimeMinute) == 11038);
		static_assert(offsetof(ScriptVars_mgo, mbmFuelResourceProcessingElapsedTimeMinute) == 11039);
		static_assert(offsetof(ScriptVars_mgo, mbmBioticResourceProcessingElapsedTimeMinute) == 11040);
		static_assert(offsetof(ScriptVars_mgo, isRussianTranslatable) == 11041);
		static_assert(offsetof(ScriptVars_mgo, isPashtoTranslatable) == 11042);
		static_assert(offsetof(ScriptVars_mgo, isAfrikaansTranslatable) == 11043);
		static_assert(offsetof(ScriptVars_mgo, isKikongoTranslatable) == 11044);
		static_assert(offsetof(ScriptVars_mgo, mbmTreatmentElapsedTimeMinute) == 11045);
		static_assert(offsetof(ScriptVars_mgo, mbmBecomeCombatSectionLvMax) == 11046);
		static_assert(offsetof(ScriptVars_mgo, mbmBecomeDevelopSectionLvMax) == 11047);
		static_assert(offsetof(ScriptVars_mgo, mbmBecomeBaseDevSectionLvMax) == 11048);
		static_assert(offsetof(ScriptVars_mgo, mbmBecomeSupportSectionLvMax) == 11049);
		static_assert(offsetof(ScriptVars_mgo, mbmBecomeSpySectionLvMax) == 11050);
		static_assert(offsetof(ScriptVars_mgo, mbmBecomeMedicalSectionLvMax) == 11051);
		static_assert(offsetof(ScriptVars_mgo, mbmBecomeSecuritySectionLvMax) == 11052);
		static_assert(offsetof(ScriptVars_mgo, mbmBecomeAllSectionLvMax) == 11053);
		static_assert(offsetof(ScriptVars_mgo, mbmHighestTotalSectionLv) == 11054);
		static_assert(offsetof(ScriptVars_mgo, mbmStaffSyncVersion) == 11056);
		static_assert(offsetof(ScriptVars_mgo, mbmStaffSvarsHeaders) == 11060);
		static_assert(offsetof(ScriptVars_mgo, mbmStaffSvarsSeeds) == 25060);
		static_assert(offsetof(ScriptVars_mgo, mbmStaffSvarsStatusesSync) == 39060);
		static_assert(offsetof(ScriptVars_mgo, mbmStaffSvarsStatusesNoSync) == 53060);
		static_assert(offsetof(ScriptVars_mgo, mbmStaffSkillGots) == 60060);
		static_assert(offsetof(ScriptVars_mgo, mbmStaffSortKeysSvars) == 60129);
		static_assert(offsetof(ScriptVars_mgo, mbmStaffSortDirsSvars) == 60142);
		static_assert(offsetof(ScriptVars_mgo, mbmStaffListMainListModeSvars) == 60155);
		static_assert(offsetof(ScriptVars_mgo, mbmStaffListAssignListModeSvars) == 60156);
		static_assert(offsetof(ScriptVars_mgo, mbmEnmityElapsedTimeMinuteSvars) == 60157);
		static_assert(offsetof(ScriptVars_mgo, mbmMoraleElapsedTimeMinuteSvars) == 60158);
		static_assert(offsetof(ScriptVars_mgo, mbmLanguageElapsedTimeMinuteSvars) == 60159);
		static_assert(offsetof(ScriptVars_mgo, mbmEnableStaffInitLangKikongo) == 60160);
		static_assert(offsetof(ScriptVars_mgo, mbmPandemicEventMode) == 60161);
		static_assert(offsetof(ScriptVars_mgo, mbmPandemicElapsedTimeMinuteSvars) == 60162);
		static_assert(offsetof(ScriptVars_mgo, mbmPandemicSpreadAttackLimitCount) == 60164);
		static_assert(offsetof(ScriptVars_mgo, mbmPandemicSpreadAttackCount) == 60166);
		static_assert(offsetof(ScriptVars_mgo, mbmDisableKikongoFirst) == 60168);
		static_assert(offsetof(ScriptVars_mgo, mbmMasterGunsmithSkill) == 60169);
		static_assert(offsetof(ScriptVars_mgo, mbmBipedalismWeaponDevelopmentSkill) == 60170);
		static_assert(offsetof(ScriptVars_mgo, mbmBanHeuy) == 60171);
		static_assert(offsetof(ScriptVars_mgo, mbmAssignableMiller) == 60172);
		static_assert(offsetof(ScriptVars_mgo, mbmLockedTanFlag) == 60173);
		static_assert(offsetof(ScriptVars_mgo, mbmStaffInitEnmityLv) == 60174);
		static_assert(offsetof(ScriptVars_mgo, mbmEquipParamStatus) == 60176);
		static_assert(offsetof(ScriptVars_mgo, mbmOnlineOffsetEquipParamStatus) == 60944);
		static_assert(offsetof(ScriptVars_mgo, mbmEquipDevelopIndices) == 61712);
		static_assert(offsetof(ScriptVars_mgo, mbmEquipDevelopTimes) == 61732);
		static_assert(offsetof(ScriptVars_mgo, mbmWeaponPartsReceiverStatus) == 61772);
		static_assert(offsetof(ScriptVars_mgo, mbmWeaponPartsBarrelStatus) == 61872);
		static_assert(offsetof(ScriptVars_mgo, mbmWeaponPartsMagazineStatus) == 61904);
		static_assert(offsetof(ScriptVars_mgo, mbmWeaponPartsStockStatus) == 61950);
		static_assert(offsetof(ScriptVars_mgo, mbmWeaponPartsMuzzleStatus) == 61985);
		static_assert(offsetof(ScriptVars_mgo, mbmWeaponPartsMuzzleOptionStatus) == 62014);
		static_assert(offsetof(ScriptVars_mgo, mbmWeaponPartsSightStatus) == 62024);
		static_assert(offsetof(ScriptVars_mgo, mbmWeaponPartsUnderBarrelStatus) == 62049);
		static_assert(offsetof(ScriptVars_mgo, mbmWeaponPartsLightStatus) == 62056);
		static_assert(offsetof(ScriptVars_mgo, mbmFobSvars) == 62064);
		static_assert(offsetof(ScriptVars_mgo, mbmClusterSvars) == 62080);
		static_assert(offsetof(ScriptVars_mgo, mbmClusterTimeMinutes) == 62152);
		static_assert(offsetof(ScriptVars_mgo, mbmCompMb) == 62292);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityClusterSvars) == 62296);
		static_assert(offsetof(ScriptVars_mgo, mbmSecuritySpecialPlatformSoldierCounts) == 62436);
		static_assert(offsetof(ScriptVars_mgo, mbmSecuritySpecialPlatformIrSensorCounts) == 62471);
		static_assert(offsetof(ScriptVars_mgo, mbmSecuritySpecialPlatformAntitheftCounts) == 62506);
		static_assert(offsetof(ScriptVars_mgo, mbmSecuritySpecialPlatformCameraCounts) == 62541);
		static_assert(offsetof(ScriptVars_mgo, mbmSecuritySpecialPlatformDecoyCounts) == 62576);
		static_assert(offsetof(ScriptVars_mgo, mbmSecuritySpecialPlatformMineCounts) == 62611);
		static_assert(offsetof(ScriptVars_mgo, mbmSecuritySpecialPlatformUavCounts) == 62646);
		static_assert(offsetof(ScriptVars_mgo, mbmSecuritySpecialPlatformCautionAreaSvars) == 62684);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform1SoldierCounts) == 62824);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform1IrSensorCounts) == 62859);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform1AntitheftCounts) == 62894);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform1CameraCounts) == 62929);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform1DecoyCounts) == 62964);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform1MineCounts) == 62999);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform1UavCounts) == 63034);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform1CautionAreaSvars) == 63072);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform2SoldierCounts) == 63212);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform2IrSensorCounts) == 63247);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform2AntitheftCounts) == 63282);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform2CameraCounts) == 63317);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform2DecoyCounts) == 63352);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform2MineCounts) == 63387);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform2UavCounts) == 63422);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform2CautionAreaSvars) == 63460);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform3SoldierCounts) == 63600);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform3IrSensorCounts) == 63635);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform3AntitheftCounts) == 63670);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform3CameraCounts) == 63705);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform3DecoyCounts) == 63740);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform3MineCounts) == 63775);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform3UavCounts) == 63810);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform3CautionAreaSvars) == 63848);
		static_assert(offsetof(ScriptVars_mgo, mbmSecuritySpecialPlatformSvars) == 63988);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform1Svars) == 64128);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform2Svars) == 64268);
		static_assert(offsetof(ScriptVars_mgo, mbmSecurityCommonPlatform3Svars) == 64408);
		static_assert(offsetof(ScriptVars_mgo, mbmInitializedServerWalletFlagDay1) == 64548);
		static_assert(offsetof(ScriptVars_mgo, mbmResourceSyncVersion) == 64552);
		static_assert(offsetof(ScriptVars_mgo, mbmResourceSvars1Fix) == 64556);
		static_assert(offsetof(ScriptVars_mgo, mbmResourceSvars2Fix) == 64792);
		static_assert(offsetof(ScriptVars_mgo, mbmResourceSvars1Diff) == 65028);
		static_assert(offsetof(ScriptVars_mgo, mbmResourceSvars2Diff) == 65264);
		static_assert(offsetof(ScriptVars_mgo, mbmWhiteContainerCountsSvarsFix) == 65500);
		static_assert(offsetof(ScriptVars_mgo, mbmWhiteContainerCountsSvarsDiff) == 65520);
		static_assert(offsetof(ScriptVars_mgo, mbmRedContainerCountsSvarsFix) == 65540);
		static_assert(offsetof(ScriptVars_mgo, mbmRedContainerCountsSvarsDiff) == 65560);
		static_assert(offsetof(ScriptVars_mgo, mbmOpenedWalkerGear) == 65580);
		static_assert(offsetof(ScriptVars_mgo, mbmDataBaseSvars) == 65581);
		static_assert(offsetof(ScriptVars_mgo, mbmCompAnimal) == 66040);
		static_assert(offsetof(ScriptVars_mgo, mbmCompDesign) == 66041);
		static_assert(offsetof(ScriptVars_mgo, mbmCompPhoto) == 66042);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployMissionsSvars) == 66044);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployMissionAveRanksSvars) == 66328);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamCombatStaffCounts) == 66400);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamCombatStaffRankTops) == 66422);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamCombatStaffRankBottoms) == 66433);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSubStaffCounts) == 66444);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSubStaffRankTops) == 66466);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSubStaffRankBottoms) == 66477);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamCombatPoints) == 66488);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSubPoints) == 66532);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsTimeMinute) == 66576);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvars4wdEastCount) == 66620);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvars4wdWestCount) == 66642);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsTruckEastCount) == 66664);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsTruckWestCount) == 66686);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsArmoredVehicleEastCount) == 66708);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsArmoredVehicleWestCount) == 66730);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsArmoredVehicleEastRocketCount) == 66752);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsWheeledArmoredVehicleWestCount) == 66774);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsTankEastCount) == 66796);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsTankWestCount) == 66818);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsWalkerGearProtoHeuyCount) == 66840);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsWalkerGearSovietBattleCount) == 66862);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsWalkerGearSovietSupportCount) == 66884);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsWalkerGearCfaBattleCount) == 66906);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployTeamSvarsWalkerGearCfaSupportCount) == 66928);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployRandomMissionSeedSvars) == 66952);
		static_assert(offsetof(ScriptVars_mgo, mbmDeploySequentialMissionIdSvars) == 66956);
		static_assert(offsetof(ScriptVars_mgo, mbmDeploySequentialMissionIdLimitSvars) == 66958);
		static_assert(offsetof(ScriptVars_mgo, mbmClearedDeployRevengeMissionFlags) == 66960);
		static_assert(offsetof(ScriptVars_mgo, mbmDeployableBattleGear) == 66974);
		static_assert(offsetof(ScriptVars_mgo, mbmBattleGearDeployingTeam) == 66975);
		static_assert(offsetof(ScriptVars_mgo, mbmAddDeployLogIndex) == 66976);
		static_assert(offsetof(ScriptVars_mgo, mbmCompSequential) == 66977);
		static_assert(offsetof(ScriptVars_mgo, mbmNewestNRandomMissionId) == 66978);
		static_assert(offsetof(ScriptVars_mgo, mbmNewestRRandomMissionId) == 66980);
		static_assert(offsetof(ScriptVars_mgo, mbmOpenedRRandomMission) == 66982);
		static_assert(offsetof(ScriptVars_mgo, mbmNRandomMissionClearedCount) == 66983);
		static_assert(offsetof(ScriptVars_mgo, mbmIsCancelAllDeploy) == 66984);
		static_assert(offsetof(ScriptVars_mgo, mbmPoolRewardsSyncVersion) == 66988);
		static_assert(offsetof(ScriptVars_mgo, mbmPoolRewardsSvars) == 66992);
		static_assert(offsetof(ScriptVars_mgo, mbmPoolRewardSourceIds) == 67792);
		static_assert(offsetof(ScriptVars_mgo, mbmPoolRewardSeeds) == 68592);
		static_assert(offsetof(ScriptVars_mgo, mbmPoolRewardValues) == 68992);
		static_assert(offsetof(ScriptVars_mgo, mbmS10081CaptiveStaffSvarsSeed) == 69792);
		static_assert(offsetof(ScriptVars_mgo, mbmS10081CaptiveStaffSvarsHeader) == 69796);
		static_assert(offsetof(ScriptVars_mgo, mbmS10081CaptiveStaffSvarsStatusSync) == 69800);
		static_assert(offsetof(ScriptVars_mgo, mbmS10081CaptiveStaffSvarsStatusNoSync) == 69804);
		static_assert(offsetof(ScriptVars_mgo, mbmRemoverSvarsHeaders) == 69808);
		static_assert(offsetof(ScriptVars_mgo, mbmRemoverSvarsSeeds) == 70064);
		static_assert(offsetof(ScriptVars_mgo, mbmRemoverSvarsStatusesSync) == 70320);
		static_assert(offsetof(ScriptVars_mgo, mbmRemoverSvarsStatusesNoSync) == 70576);
		static_assert(offsetof(ScriptVars_mgo, mbmRemoverReasons) == 70704);
		static_assert(offsetof(ScriptVars_mgo, mbmAddRemoverIndexSvars) == 70768);
		static_assert(offsetof(ScriptVars_mgo, mbmSecuritySystemLevelsDeveloped) == 70770);
		static_assert(offsetof(ScriptVars_mgo, mbmAnimalHabitatAfghSvars) == 70786);
		static_assert(offsetof(ScriptVars_mgo, mbmAnimalHabitatMafrSvars) == 70850);
		static_assert(offsetof(ScriptVars_mgo, mbmIsOccupation) == 70914);
		static_assert(offsetof(ScriptVars_mgo, mbmRequestDemoAttackedFromOtherPlayer) == 70915);
		static_assert(offsetof(ScriptVars_mgo, mbmDemoAttackedFromOtherPlayerKnowWhereFrom) == 70916);
		static_assert(offsetof(ScriptVars_mgo, mbmFobDefenseWinCount) == 70920);
		static_assert(offsetof(ScriptVars_mgo, mbmFobDefenseLoseCount) == 70924);
		static_assert(offsetof(ScriptVars_mgo, mbmIsContractOutDisplayed) == 70928);
		static_assert(offsetof(ScriptVars_mgo, mbmIsEnableNuclearDevelop) == 70929);
		static_assert(offsetof(ScriptVars_mgo, mbmIsNuclearDeveloped) == 70930);
		static_assert(offsetof(ScriptVars_mgo, mbmIsNuclearDiscarded) == 70931);
		static_assert(offsetof(ScriptVars_mgo, mbmIsNuclearDeveloping) == 70932);
		static_assert(offsetof(ScriptVars_mgo, mbmNuclearDevelopTime) == 70936);
		static_assert(offsetof(ScriptVars_mgo, mbmNuclearDevelopMaxTime) == 70940);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalAddedHostageStaffCount) == 70944);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalAddedSoldierStaffCount) == 70948);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalAddedVolunteerStaffCount) == 70952);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalDeadStaffCount) == 70956);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalDefectionCount) == 70960);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalFireCount) == 70964);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalAddedGmpT) == 70968);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalAddedGmp) == 70972);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalSubtractedGmpT) == 70976);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalSubtractedGmp) == 70980);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalAddedHeroicPoint) == 70984);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalSubtractedHeroicPoint) == 70988);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalAddedResourceCounts) == 70992);
		static_assert(offsetof(ScriptVars_mgo, mbmPdTotalAddedAnimalCounts) == 71228);
		static_assert(offsetof(ScriptVars_mgo, optionSelectedIndices) == 71624);
		static_assert(offsetof(ScriptVars_mgo, armRocketYIsReverse) == 71656);
		static_assert(offsetof(ScriptVars_mgo, armRocketXIsReverse) == 71657);
		static_assert(offsetof(ScriptVars_mgo, bgmVolume) == 71658);
		static_assert(offsetof(ScriptVars_mgo, keyStr) == 71660);
		static_assert(offsetof(ScriptVars_mgo, buttonNo) == 72140);
		static_assert(offsetof(ScriptVars_mgo, keybordNo) == 72620);
		static_assert(offsetof(ScriptVars_mgo, pushToTalk) == 73100);
		static_assert(offsetof(ScriptVars_mgo, checkPointGimmickIndelibleCount) == 73101);
		static_assert(offsetof(ScriptVars_mgo, checkPointGimmickIndelibleName) == 73104);
		static_assert(offsetof(ScriptVars_mgo, checkPointGimmickIndelibleState) == 73360);
		static_assert(offsetof(ScriptVars_mgo, missionStartGimmickIndelibleCount) == 73424);
		static_assert(offsetof(ScriptVars_mgo, missionStartGimmickIndelibleName) == 73428);
		static_assert(offsetof(ScriptVars_mgo, missionStartGimmickIndelibleState) == 73684);
		static_assert(offsetof(ScriptVars_mgo, checkPointGimmickPermanentGimmickSaveData) == 73748);
		static_assert(offsetof(ScriptVars_mgo, missionStartGimmickPermanentGimmickSaveData) == 76340);
		static_assert(offsetof(ScriptVars_mgo, cloakEnabled) == 78933);
		static_assert(offsetof(ScriptVars_mgo, stealthAssistLeftCount) == 79294);
		static_assert(offsetof(ScriptVars_mgo, tppTransferFlag) == 79296);
		static_assert(offsetof(ScriptVars_mgo, tppTransferAcquiredFlag) == 79300);
		static_assert(offsetof(ScriptVars_mgo, dlcPermittedFlag) == 79304);
		static_assert(offsetof(ScriptVars_mgo, dlcAcquiredFlag) == 79312);
		static_assert(offsetof(ScriptVars_mgo, mbDvcTutorialFlag) == 79320);
		static_assert(offsetof(ScriptVars_mgo, mbDvcTopMenuFlag) == 79324);
		static_assert(offsetof(ScriptVars_mgo, heliMusicType) == 79328);
		static_assert(offsetof(ScriptVars_mgo, heliMusicTrackID) == 79332);
		static_assert(offsetof(ScriptVars_mgo, numChar) == 79340);
		static_assert(offsetof(ScriptVars_mgo, lastUsedChar) == 79341);
		static_assert(offsetof(ScriptVars_mgo, selectedBGM) == 79342);
		static_assert(offsetof(ScriptVars_mgo, savedPermanents) == 79344);
		static_assert(offsetof(ScriptVars_mgo, charName) == 79408);
		static_assert(offsetof(ScriptVars_mgo, charClass) == 79920);
		static_assert(offsetof(ScriptVars_mgo, savedPrestige) == 79936);
		static_assert(offsetof(ScriptVars_mgo, savedXP) == 79944);
		static_assert(offsetof(ScriptVars_mgo, savedPeakXP) == 79976);
		static_assert(offsetof(ScriptVars_mgo, playerTypes) == 80008);
		static_assert(offsetof(ScriptVars_mgo, lastLoadout) == 80016);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarFaceRaceIndex) == 80024);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarFaceTypeIndex) == 80032);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarFaceVariationIndex) == 80040);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarFaceColorIndex) == 80048);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarHairStyleIndex) == 80056);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarRightEyeColorIndex) == 80064);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarRightEyeBrightnessIndex) == 80072);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarLeftEyeColorIndex) == 80080);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarLeftEyeBrightnessIndex) == 80088);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarHairColor) == 80096);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarBerdStyle) == 80104);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarBerdLength) == 80112);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarEbrwStyle) == 80120);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarEbrwWide) == 80128);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarGashOrTatoVariationIndex) == 80136);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarTatoColorIndex) == 80144);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarAcceFlag) == 80152);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarMotionFrames) == 80160);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarVoiceIndex) == 80640);
		static_assert(offsetof(ScriptVars_mgo, mgoAvatarVoicePitch) == 80648);
		static_assert(offsetof(ScriptVars_mgo, numLoadout) == 80656);
		static_assert(offsetof(ScriptVars_mgo, loadoutName) == 80664);
		static_assert(offsetof(ScriptVars_mgo, loadoutPrimary1) == 84248);
		static_assert(offsetof(ScriptVars_mgo, loadoutPrimary2) == 84472);
		static_assert(offsetof(ScriptVars_mgo, loadoutSecondary) == 84696);
		static_assert(offsetof(ScriptVars_mgo, loadoutParts) == 84920);
		static_assert(offsetof(ScriptVars_mgo, loadoutColor) == 88952);
		static_assert(offsetof(ScriptVars_mgo, loadoutSupport) == 89288);
		static_assert(offsetof(ScriptVars_mgo, loadoutItem) == 90184);
		static_assert(offsetof(ScriptVars_mgo, loadoutSkill) == 91976);
		static_assert(offsetof(ScriptVars_mgo, gearData) == 92872);
		static_assert(offsetof(ScriptVars_mgo, appealActionSlots) == 93352);
		static_assert(offsetof(ScriptVars_mgo, appealActionSoundType) == 93512);
		static_assert(offsetof(ScriptVars_mgo, optionShowMissionInfo) == 93520);
		static_assert(offsetof(ScriptVars_mgo, optionShowTmpInfo) == 93521);
		static_assert(offsetof(ScriptVars_mgo, optionShowCombatInfo) == 93522);
		static_assert(offsetof(ScriptVars_mgo, presetRadio) == 93524);
		static_assert(offsetof(ScriptVars_mgo, presetRadioMarkers) == 93624);
		static_assert(offsetof(ScriptVars_mgo, gearState) == 93656);
		static_assert(offsetof(ScriptVars_mgo, isNight) == 93716);
		static_assert(offsetof(ScriptVars_mgo, roundTickets) == 93717);
		static_assert(offsetof(ScriptVars_mgo, isAvatarEditMode) == 93718);
		static_assert(offsetof(ScriptVars_mgo, cancelAvatarEdit) == 93719);
		static_assert(offsetof(ScriptVars_mgo, roundTimeLimit) == 93720);
		static_assert(offsetof(ScriptVars_mgo, briefingTime) == 93724);
		static_assert(offsetof(ScriptVars_mgo, walkerGear) == 93728);
		static_assert(offsetof(ScriptVars_mgo, memberMin) == 93732);
		static_assert(offsetof(ScriptVars_mgo, specialRole) == 93736);
		static_assert(offsetof(ScriptVars_mgo, uniqueCharacterSelect) == 93740);
		static_assert(offsetof(ScriptVars_mgo, isGameplayHost) == 93744);
		static_assert(offsetof(ScriptVars_mgo, hasDisplayedTitleScreen) == 93745);
		static_assert(offsetof(ScriptVars_mgo, isCustomizeMenu) == 93746);
		static_assert(offsetof(ScriptVars_mgo, isPlayerInCustomizeMenu) == 93747);
		static_assert(offsetof(ScriptVars_mgo, doesNeedToRespawnOnceInCustomize) == 93748);
		static_assert(offsetof(ScriptVars_mgo, isInitialTutorialFinished) == 93749);
		static_assert(offsetof(ScriptVars_mgo, gamePlayTutorialCount) == 93750);
		static_assert(offsetof(ScriptVars_mgo, isNoContest) == 93751);
		static_assert(offsetof(ScriptVars_mgo, didSurvivalTicketNotice) == 93752);
		static_assert(offsetof(ScriptVars_mgo, dlcFlags) == 93756);
		static_assert(offsetof(ScriptVars_mgo, mgoEulaVersion) == 93760);
		static_assert(offsetof(ScriptVars_mgo, searchSettingsMapIndex) == 93764);
		static_assert(offsetof(ScriptVars_mgo, searchSettingsRuleIndex) == 93765);
		static_assert(offsetof(ScriptVars_mgo, searchSettingsOthers) == 93768);
		static_assert(offsetof(ScriptVars_mgo, searchSettingsSurvival) == 93772);
		static_assert(offsetof(ScriptVars_mgo, isSavedHostCommentV110) == 93776);
		static_assert(offsetof(ScriptVars_mgo, survivalHostCommentIndex) == 93780);
		static_assert(offsetof(ScriptVars_mgo, survivalRoomId) == 93784);
		static_assert(offsetof(ScriptVars_mgo, userMarkerLocationId) == 95268);
		static_assert(offsetof(ScriptVars_mgo, userMarkerSaveCount) == 95270);

		struct ScriptVars_tpp
		{
			char utcTime[8]; // 0
			char totalPlayTime[8]; // 8
			short locationCode; // 16
			short missionCode; // 18
			short prevLocationCode; // 20
			short prevMissionCode; // 22
			char __pad0[4];
			int rulesetDynamicPackIndices; // 28
			short mbLayoutCode; // 32
			char mbClusterId; // 34
			char fobSneakMode; // 35
			char fobIsSneak; // 36
			char fobIsEvent; // 37
			char fobIsSecurity; // 38
			char fobIsPlaceMode; // 39
			char fobIsInitializedFreePositionItem; // 40
			char __pad1[3];
			int returnStaffHeader; // 44
			int returnStaffSeeds; // 48
			int eulaVersion; // 52
			int eulaVersion2; // 56
			int eulaVersion3; // 60
			int countryIndex; // 64
			int countryStateIndex; // 68
			int infoId; // 72
			int infoIdForMGO; // 76
			int inquiryId; // 80
			int fobPickup; // 84
			int padType; // 88
			int clock; // 92
			char __pad2[4];
			int passageSecondsSinceOutMB; // 100
			int weather; // 104
			int weatherNextTime; // 108
			int extraWeatherInterval; // 112
			int weatherAddDensity; // 116
			char requestWeatherType[5]; // 120
			char __pad3[3];
			int requestWeatherUserId[5]; // 128
			int requestWeatherInterp[5]; // 148
			int requestWeatherParam[5]; // 168
			char dominatedCpFlagsAfgh[16]; // 188
			char dominatedCpFlagsMafr[16]; // 204
			int totalAlertCount; // 220
			int initialPlayerPosX; // 224
			int initialPlayerPosY; // 228
			int initialPlayerPosZ; // 232
			int playerPosX; // 236
			int playerPosY; // 240
			int playerPosZ; // 244
			char playerPartsType; // 248
			char playerCamoType; // 249
			char playerHandType; // 250
			char playerType; // 251
			short playerFaceId; // 252
			short playerFaceEquipId; // 254
			char playerSkillId; // 256
			char playerInjuryCount; // 257
			char playerBodyInjuryCount; // 258
			char __pad4[1];
			short playerLifeMax; // 260
			short initialPlayerRotY; // 262
			short playerRotY; // 264
			short initialPlayerFlag; // 266
			char initialPlayerAction; // 268
			char playerRetryFlag; // 269
			short initialPlayerPairGameObjectId; // 270
			short playerVehicleGameObjectId; // 272
			short initialPlayerVehicleGameObjectId; // 274
			short vehicleGameObjectIdOnPlayerOnCargo; // 276
			char __pad5[2];
			int playerDisableActionFlag; // 280
			short playerLife; // 284
			short playerStamina; // 286
			int playerCameraPosition[3]; // 288
			int playerCameraRotation[2]; // 300
			int playerStaffHeader; // 308
			int playerStaffSeed; // 312
			int playerPlayFlag; // 316
			int playerMarkingCountInMission; // 320
			int playerShootCountInMission; // 324
			int shootHitCountInMission; // 328
			int shootHitCountEliminatedInMission; // 332
			int playerOgrePointOnStartMission; // 336
			int playerRecoveryCountFromOgre; // 340
			int totalChickenCapTime; // 344
			int totalChickCapTime; // 348
			int totalDiscoveredCount; // 352
			int totalTakeHitCount; // 356
			int totalMovingDistanceAtStand; // 360
			int totalMovingDistanceAtSquat; // 364
			int totalMovingDistanceAtCrawl; // 368
			int totalMovingDistanceOnHorse; // 372
			int totalMovingDistanceOnVehicle; // 376
			int totalMovingDistanceOnWalkerGear; // 380
			int totalMovingDistanceOnBattleGear; // 384
			int totalMarkingCount; // 388
			int totalReflexCount; // 392
			char useBlackDiamondEmblem; // 396
			char isAvatarPlayerEnable; // 397
			char avatarFaceRaceIndex; // 398
			char avatarFaceTypeIndex; // 399
			char avatarFaceVariationIndex; // 400
			char avatarFaceColorIndex; // 401
			char avatarHairStyleIndex; // 402
			char avatarRightEyeColorIndex; // 403
			char avatarRightEyeBrightnessIndex; // 404
			char avatarLeftEyeColorIndex; // 405
			char avatarLeftEyeBrightnessIndex; // 406
			char avatarHairColor; // 407
			char avatarBerdStyle; // 408
			char avatarBerdLength; // 409
			char avatarEbrwStyle; // 410
			char avatarEbrwWide; // 411
			char avatarGashOrTatoVariationIndex; // 412
			char avatarTatoColorIndex; // 413
			char avatarAcceFlag; // 414
			char avatarMotionFrame[60]; // 415
			char avatarSaveIsValid; // 475
			char avatarEyePresetID; // 476
			char avatarNosePresetID; // 477
			char avatarMouthPresetID; // 478
			char avatarChinPresetID; // 479
			char avatarCheekPresetID; // 480
			char avatarEyebrowPresetID; // 481
			char avatarHeadPresetID; // 482
			char avatarHairPresetID; // 483
			char avatarHairColorPresetID; // 484
			char avatarSkinColorPresetID; // 485
			char avatarBeardPresetID; // 486
			char avatarAccessoryPresetID; // 487
			char avatarAllPartsPresetID; // 488
			char avatarReserve0; // 489
			char avatarReserve1; // 490
			char avatarReserve2; // 491
			char personalName[64]; // 492
			char personalBirthdayMonth; // 556
			char personalBirthdayDay; // 557
			char __pad6[2];
			int emblemTextureTag[4]; // 560
			int emblemColorL[4]; // 576
			int emblemColorH[4]; // 592
			char emblemX[4]; // 608
			char emblemY[4]; // 612
			char emblemScale[4]; // 616
			char emblemRotate[4]; // 620
			char emblemFlag[680]; // 624
			char emblemVersion; // 1304
			char emblemSyncFailed; // 1305
			char emblemSyncFailed2; // 1306
			char isServerDataCleared; // 1307
			char isDefaultDisconnected; // 1308
			char didCancelPatchDlcDownloadRequest; // 1309
			char didCancelFobPatchDlcDownloadRequest; // 1310
			char isPersonalDirty; // 1311
			short initWeapons[3]; // 1312
			short initSupportWeapons[8]; // 1318
			short initItems[8]; // 1334
			short initHandEquip; // 1350
			short weapons[6]; // 1352
			short ammoInWeapons[6]; // 1364
			short ammoSubInWeapons[6]; // 1376
			char suppressorLives[6]; // 1388
			char isUseSuppressor[6]; // 1394
			char isUseFlashLight[6]; // 1400
			char isInitialWeapon[6]; // 1406
			short supportWeapons[16]; // 1412
			short items[16]; // 1444
			char cboxPosterIndices[16]; // 1476
			short handEquip; // 1492
			char upgrades[16]; // 1494
			char ammoStockIds[46]; // 1510
			short ammoStockCounts[46]; // 1556
			int totalBatteryPowerAsGmp; // 1648
			int currentInventorySlot; // 1652
			int currentSupportWeaponIndex; // 1656
			int currentItemIndex; // 1660
			int fpsAttackMode; // 1664
			char currentZoomStep[5]; // 1668
			char requestFlagsAboutEquip; // 1673
			short weaponOriginalOwners[3]; // 1674
			char partsType2; // 1680
			char camoType2; // 1681
			char faceEquipId2; // 1682
			char __pad7[1];
			short weapons2[3]; // 1684
			short supportWeapons2[8]; // 1690
			short items2[8]; // 1706
			short handEquip2; // 1722
			char dirtyEffectCount[2]; // 1724
			char dirtyEffectBoneIndex[64]; // 1726
			char __pad8[2];
			int dirtyEffectPosition[192]; // 1792
			int dirtyEffectRadius[64]; // 2560
			short initCustomizedWeapon[3]; // 2816
			short customizedWeapon[3]; // 2822
			char initChimeraParts[36]; // 2828
			char chimeraParts[36]; // 2864
			char initChimeraPaintType[3]; // 2900
			char chimeraPaintType[3]; // 2903
			char initChimeraColorIndex[3]; // 2906
			char chimeraColorIndex[3]; // 2909
			char isUseCustomizedWeapon[3]; // 2912
			char customizedWeaponSlotIndex[3]; // 2915
			char itemLevels[72]; // 2918
			char itemLevelsDeveloped[72]; // 2990
			char developedItemLevels[72]; // 3062
			short suitDevelopLevel[117]; // 3134
			char initLoadoutInfo; // 3368
			char loadoutInfoSaveFormVersion; // 3369
			char loadoutInfoU8buf[460]; // 3370
			short loadoutInfoU16buf[144]; // 3830
			char __pad9[2];
			int loadoutInfoU32buf[68]; // 4120
			char currentLoadoutSlot; // 4392
			char sortieLoadoutInfoSaveFormVersion; // 4393
			char sortieLoadoutInfoU8buf[460]; // 4394
			short sortieLoadoutInfoU16buf[144]; // 4854
			char __pad10[2];
			int sortieLoadoutInfoU32buf[68]; // 5144
			char currentSortieLoadoutSlot; // 5416
			char currentSortieLoadoutSlotNonRental; // 5417
			char returnHeliLoadoutInfoSaveFormVersion; // 5418
			char returnHeliLoadoutInfoU8buf[460]; // 5419
			char __pad11[1];
			short returnHeliLoadoutInfoU16buf[144]; // 5880
			int returnHeliLoadoutInfoU32buf[68]; // 6168
			char currentReturnHeliLoadoutSlot; // 6440
			char currentForRentLoadoutSlotIndex; // 6441
			char __pad12[2];
			int rentalLoadoutPlayerId; // 6444
			char rentalLoadoutPlayerName[100]; // 6448
			char sortiePrepPlayerSnakePartsType; // 6548
			char sortiePrepPlayerSnakeCamoType; // 6549
			short sortiePrepPlayerSnakeFaceEquipId; // 6550
			char sortiePrepPlayerSnakeSuitLevel; // 6552
			char sortiePrepPlayerDDPartsType; // 6553
			char sortiePrepPlayerDDCamoType; // 6554
			char __pad13[1];
			short sortiePrepPlayerDDFaceEquipId; // 6556
			char sortiePrepPlayerDDSuitLevel; // 6558
			char sortiePrepPlayerType; // 6559
			int sortiePrepPlayerStaffHeader; // 6560
			int sortiePrepPlayerStaffSeed; // 6564
			int sortiePrepPlayerFaceId; // 6568
			short userPresetCustomizedWeapon[24]; // 6572
			char userPresetChimeraParts[288]; // 6620
			char userPresetChimeraColorIndex[24]; // 6908
			char userPresetChimeraPaintType[24]; // 6932
			char userPresetDWalkerParts[24]; // 6956
			char userPresetDWalkerPartsLevel[24]; // 6980
			char userPresetDWalkerAdditionalParts[12]; // 7004
			char userPresetDWalkerAdditionalPartsLevel[12]; // 7016
			char userPresetDWalkerCurrentSlot; // 7028
			char userPresetBattleGearParts[27]; // 7029
			char userPresetBattleGearPartsLevel[27]; // 7056
			char userPresetBattleGearCurrentSlot; // 7083
			char userPresetHeliParts[5]; // 7084
			char userPresetVehiclePaintType[10]; // 7089
			char userPresetBuddyDogParts[2]; // 7099
			char userPresetBuddyHorseParts[3]; // 7101
			char mbmUserPresetBaseColor; // 7104
			char __pad14[1];
			short droppedWeapons[3]; // 7106
			int droppedWeaponPos[9]; // 7112
			int droppedWeaponRot[12]; // 7148
			short droppedCount[6]; // 7196
			short droppedAmmo[6]; // 7208
			char droppedSuppressorLife[3]; // 7220
			char __pad15[1];
			short droppedCarrier[3]; // 7224
			char droppedCarrierPlaceNo[3]; // 7230
			char droppedWeaponStatus[3]; // 7233
			short importantItems[32]; // 7236
			short importantItemNumber[32]; // 7300
			int importantItemPos[96]; // 7364
			int importantItemRot[128]; // 7748
			char importantItemStatus[32]; // 8260
			int markingsOfPickable[33]; // 8292
			short missionCodeMarkingsOfPickable; // 8424
			char __pad16[2];
			int markingsOfPlaced[33]; // 8428
			short missionCodeMarkingsOfPlaced; // 8560
			short placedWeapons[32]; // 8562
			short placedOwnerIds[32]; // 8626
			char __pad17[2];
			int placedPos[96]; // 8692
			int placedRot[128]; // 9076
			int placedAttachTargetNames[64]; // 9588
			short placedAttachTargetIndex[32]; // 9844
			char placedAttachTargetGameObjectType[32]; // 9908
			int placedAttachPoints[64]; // 9940
			short placedCorrelationId[32]; // 10196
			short placedTargetGroupBlast[32]; // 10260
			char placedStatus[32]; // 10324
			short playerDecoyInstanceIndicies[12]; // 10356
			int playerDecoyAngleBallon[12]; // 10380
			short caputreAnimalId[16]; // 10428
			int captureAreaName[16]; // 10460
			char captureAnimalIndex; // 10524
			char __pad18[3];
			int collectionPositionsMoved[96]; // 10528
			int collectionRotationsMoved[32]; // 10912
			short collectionLocatorIndicesMoved[32]; // 11040
			int collectionMovedIndexEnd; // 11104
			short locationCodeOfCollectionMoved; // 11108
			char cboxIsValid[16]; // 11110
			char __pad19[2];
			int cboxLife[16]; // 11128
			short cboxFlag[16]; // 11192
			char cboxPosterType[16]; // 11224
			short cboxEquipId[16]; // 11240
			int cboxLocation[64]; // 11272
			char buddyTypeMissionStart; // 11528
			char obtainedBuddyTypeMissionStart; // 11529
			char sortieBuddyTypeMissionStart; // 11530
			char deadBuddyTypeMissionStart; // 11531
			char initialBuddyStatusMissionStart; // 11532
			char __pad20[3];
			int initialBuddyPosMissionStart[3]; // 11536
			int initialBuddyRotYMissionStart; // 11548
			int buddyCommonFlagMissionStart; // 11552
			short buddyFriendlyPointMissionStart[8]; // 11556
			int reliefVehicleCarryType; // 11572
			int reliefVehicleCarryAttitude[4]; // 11576
			short reliefVehicleCarryLife[12]; // 11592
			short reliefVehicleCarryAmmo[3]; // 11616
			short reliefVehicleCarryState; // 11622
			char buddyType; // 11624
			char obtainedBuddyType; // 11625
			char sortieBuddyType; // 11626
			char disableCallBuddyType; // 11627
			char deadBuddyType; // 11628
			char initialBuddyStatus; // 11629
			char __pad21[2];
			int initialBuddyPos[3]; // 11632
			int initialBuddyRotY; // 11644
			int buddyCommonFlag; // 11648
			int buddyMissionFlag; // 11652
			int buddyCallCount[7]; // 11656
			int buddyEspionageCpNames[8]; // 11684
			int buddyFriendlyTimer[8]; // 11716
			short buddyFriendlyPoint[8]; // 11748
			short buddyFriendlyMaxPoint[8]; // 11764
			char buddyFriendlyCount[64]; // 11780
			short buddyQuietEquipType; // 11844
			char buddyQuietCostumeType; // 11846
			char buddyDogFlag; // 11847
			char buddyDogMode; // 11848
			char buddyDogEquipType; // 11849
			char buddyDogEyeType; // 11850
			char buddyDogColorType; // 11851
			short buddyGearAmmoCount[2]; // 11852
			short buddyGearStockAmmoCount[2]; // 11856
			char buddyGearBodyGrade; // 11860
			char buddyGearEquipGrade; // 11861
			char buddyGearReloadGrade; // 11862
			char buddyGearHeadType; // 11863
			char buddyGearHeadGrade; // 11864
			char buddyGearArmType; // 11865
			char buddyGearArmGrade; // 11866
			char buddyGearMainWPType; // 11867
			char buddyGearMainWPGrade; // 11868
			char buddyGearSubWPType; // 11869
			char buddyGearSubWPGrade; // 11870
			char buddyGearColoring0; // 11871
			char buddyGearColoring1; // 11872
			char buddyHorseType; // 11873
			char buddyHorseColorType; // 11874
			char buddyHorseHeadDesignType; // 11875
			char buddyHorseFootDesignType; // 11876
			char __pad22[1];
			short battleGearAmmoCountMain[2]; // 11878
			short battleGearAmmoCountSub; // 11882
			short battleGearStockAmmoCountMain[2]; // 11884
			short battleGearStockAmmoCountSub; // 11888
			char battleGearMainWPType[2]; // 11890
			char battleGearMainWPGrade[2]; // 11892
			char battleGearSubWPType; // 11894
			char battleGearSubWPGrade; // 11895
			char battleGearColoring0; // 11896
			char battleGearColoring1; // 11897
			char maxPhase; // 11898
			char playerPhase; // 11899
			short sahelanBrokenMesh; // 11900
			char __pad23[2];
			int supportHeliCustomize; // 11904
			int pfRatingPoint; // 11908
			int mbmTppGmp; // 11912
			int mbmServerWalletGmp; // 11916
			int mbmTppHeroicPoint; // 11920
			int missionHeroicPoint; // 11924
			int mbmTppOgrePoint; // 11928
			int missionOgrePoint; // 11932
			char mbmRequestMisogi; // 11936
			char __pad24[3];
			int mbmTppTotalIncomeGmp; // 11940
			char mbmIsOpenedCombatSection; // 11944
			char mbmIsOpenedBaseDevSection; // 11945
			char mbmIsOpenedSpySection; // 11946
			char mbmIsOpenedMedicalSection; // 11947
			char mbmIsOpenedSecuritySection; // 11948
			char mbmIsOpenedHospitalSection; // 11949
			char mbmIsOpenedPrisonSection; // 11950
			char mbmIsOpenedSeparationSection; // 11951
			char mbmSectionFuncSvars[42]; // 11952
			char mbmAutoGmpElapsedTimeMinuteSvars; // 11994
			char mbmAutoResourceElapsedTimeMinuteSvars; // 11995
			char mbmCommonMetalProcessingElapsedTimeMinute; // 11996
			char mbmMinorMetalProcessingElapsedTimeMinute; // 11997
			char mbmPreciousMetalProcessingElapsedTimeMinute; // 11998
			char mbmFuelResourceProcessingElapsedTimeMinute; // 11999
			char mbmBioticResourceProcessingElapsedTimeMinute; // 12000
			char isRussianTranslatable; // 12001
			char isPashtoTranslatable; // 12002
			char isAfrikaansTranslatable; // 12003
			char isKikongoTranslatable; // 12004
			char mbmTreatmentElapsedTimeMinute; // 12005
			char mbmBecomeCombatSectionLvMax; // 12006
			char mbmBecomeDevelopSectionLvMax; // 12007
			char mbmBecomeBaseDevSectionLvMax; // 12008
			char mbmBecomeSupportSectionLvMax; // 12009
			char mbmBecomeSpySectionLvMax; // 12010
			char mbmBecomeMedicalSectionLvMax; // 12011
			char mbmBecomeSecuritySectionLvMax; // 12012
			char mbmBecomeAllSectionLvMax; // 12013
			short mbmHighestTotalSectionLv; // 12014
			int mbmStaffSyncVersion; // 12016
			int mbmStaffSvarsHeaders[3500]; // 12020
			int mbmStaffSvarsSeeds[3500]; // 26020
			int mbmStaffSvarsStatusesSync[3500]; // 40020
			short mbmStaffSvarsStatusesNoSync[3500]; // 54020
			char mbmStaffSkillGots[87]; // 61020
			char mbmStaffSortKeysSvars[13]; // 61107
			char mbmStaffSortDirsSvars[13]; // 61120
			char mbmStaffListMainListModeSvars; // 61133
			char mbmStaffListAssignListModeSvars; // 61134
			char mbmEnmityElapsedTimeMinuteSvars; // 61135
			char mbmMoraleElapsedTimeMinuteSvars; // 61136
			char mbmLanguageElapsedTimeMinuteSvars; // 61137
			char mbmEnableStaffInitLangKikongo; // 61138
			char mbmPandemicEventMode; // 61139
			char mbmPandemicElapsedTimeMinuteSvars; // 61140
			char __pad25[1];
			short mbmPandemicSpreadAttackLimitCount; // 61142
			short mbmPandemicSpreadAttackCount; // 61144
			char mbmDisableKikongoFirst; // 61146
			char mbmMasterGunsmithSkill; // 61147
			char mbmBipedalismWeaponDevelopmentSkill; // 61148
			char mbmBanHeuy; // 61149
			char mbmAssignableMiller; // 61150
			char mbmLockedTanFlag; // 61151
			short mbmStaffInitEnmityLv; // 61152
			char mbmEquipParamStatus[1024]; // 61154
			char mbmOnlineOffsetEquipParamStatus[1024]; // 62178
			short mbmEquipDevelopIndices[10]; // 63202
			char __pad26[2];
			int mbmEquipDevelopTimes[10]; // 63224
			char mbmWeaponPartsReceiverStatus[234]; // 63264
			char mbmWeaponPartsBarrelStatus[115]; // 63498
			char mbmWeaponPartsMagazineStatus[192]; // 63613
			char mbmWeaponPartsStockStatus[43]; // 63805
			char mbmWeaponPartsMuzzleStatus[29]; // 63848
			char mbmWeaponPartsMuzzleOptionStatus[40]; // 63877
			char mbmWeaponPartsSightStatus[25]; // 63917
			char mbmWeaponPartsUnderBarrelStatus[23]; // 63942
			char mbmWeaponPartsLightStatus[10]; // 63965
			char __pad27[1];
			short mbmServerDevelopCount; // 63976
			short mbmServerDevelopIds[320]; // 63978
			short mbmServerDevelopUsageGmpCosts[320]; // 64618
			char mbmServerDevelopUsageResourceType1s[320]; // 65258
			short mbmServerDevelopUsageResourceType1Counts[320]; // 65578
			char mbmServerDevelopUsageResourceType2s[320]; // 66218
			short mbmServerDevelopUsageResourceType2Counts[320]; // 66538
			char __pad28[2];
			int mbmFobSvars[4]; // 67180
			short mbmClusterSvars[35]; // 67196
			char __pad29[2];
			int mbmClusterTimeMinutes[35]; // 67268
			char mbmCompMb; // 67408
			char doesNotRestoreCommandClusterGradeFromLayoutCode; // 67409
			char __pad30[2];
			int mbmSecurityClusterSvars[35]; // 67412
			char mbmSecuritySpecialPlatformSoldierCounts[35]; // 67552
			char mbmSecuritySpecialPlatformIrSensorCounts[35]; // 67587
			char mbmSecuritySpecialPlatformAntitheftCounts[35]; // 67622
			char mbmSecuritySpecialPlatformCameraCounts[35]; // 67657
			char mbmSecuritySpecialPlatformDecoyCounts[35]; // 67692
			char mbmSecuritySpecialPlatformMineCounts[35]; // 67727
			char mbmSecuritySpecialPlatformUavCounts[35]; // 67762
			char __pad31[3];
			int mbmSecuritySpecialPlatformCautionAreaSvars[35]; // 67800
			char mbmSecuritySpecialPlatformPlacedFreeMineCounts[35]; // 67940
			char mbmSecuritySpecialPlatformPlacedFreeCameraCounts[35]; // 67975
			char mbmSecurityCommonPlatform1SoldierCounts[35]; // 68010
			char mbmSecurityCommonPlatform1IrSensorCounts[35]; // 68045
			char mbmSecurityCommonPlatform1AntitheftCounts[35]; // 68080
			char mbmSecurityCommonPlatform1CameraCounts[35]; // 68115
			char mbmSecurityCommonPlatform1DecoyCounts[35]; // 68150
			char mbmSecurityCommonPlatform1MineCounts[35]; // 68185
			char mbmSecurityCommonPlatform1UavCounts[35]; // 68220
			char __pad32[1];
			int mbmSecurityCommonPlatform1CautionAreaSvars[35]; // 68256
			char mbmSecurityCommonPlatform1PlacedFreeMineCounts[35]; // 68396
			char mbmSecurityCommonPlatform1PlacedFreeCameraCounts[35]; // 68431
			char mbmSecurityCommonPlatform2SoldierCounts[35]; // 68466
			char mbmSecurityCommonPlatform2IrSensorCounts[35]; // 68501
			char mbmSecurityCommonPlatform2AntitheftCounts[35]; // 68536
			char mbmSecurityCommonPlatform2CameraCounts[35]; // 68571
			char mbmSecurityCommonPlatform2DecoyCounts[35]; // 68606
			char mbmSecurityCommonPlatform2MineCounts[35]; // 68641
			char mbmSecurityCommonPlatform2UavCounts[35]; // 68676
			char __pad33[1];
			int mbmSecurityCommonPlatform2CautionAreaSvars[35]; // 68712
			char mbmSecurityCommonPlatform2PlacedFreeMineCounts[35]; // 68852
			char mbmSecurityCommonPlatform2PlacedFreeCameraCounts[35]; // 68887
			char mbmSecurityCommonPlatform3SoldierCounts[35]; // 68922
			char mbmSecurityCommonPlatform3IrSensorCounts[35]; // 68957
			char mbmSecurityCommonPlatform3AntitheftCounts[35]; // 68992
			char mbmSecurityCommonPlatform3CameraCounts[35]; // 69027
			char mbmSecurityCommonPlatform3DecoyCounts[35]; // 69062
			char mbmSecurityCommonPlatform3MineCounts[35]; // 69097
			char mbmSecurityCommonPlatform3UavCounts[35]; // 69132
			char __pad34[1];
			int mbmSecurityCommonPlatform3CautionAreaSvars[35]; // 69168
			char mbmSecurityCommonPlatform3PlacedFreeMineCounts[35]; // 69308
			char mbmSecurityCommonPlatform3PlacedFreeCameraCounts[35]; // 69343
			char __pad35[2];
			int mbmSecuritySpecialPlatformSvars[35]; // 69380
			int mbmSecurityCommonPlatform1Svars[35]; // 69520
			int mbmSecurityCommonPlatform2Svars[35]; // 69660
			int mbmSecurityCommonPlatform3Svars[35]; // 69800
			char mbmInitializedServerWalletFlagDay1; // 69940
			char mbmIsInitializedDay50; // 69941
			char __pad36[2];
			int mbmNuclearWeaponLocalCountPatchingDay50; // 69944
			char mbmNuclearSynchronizedDay50; // 69948
			char __pad37[3];
			int mbmResourceSyncVersion; // 69952
			int mbmResourceSvars1Fix[59]; // 69956
			int mbmResourceSvars2Fix[59]; // 70192
			int mbmResourceSvars1Diff[59]; // 70428
			int mbmResourceSvars2Diff[59]; // 70664
			int mbmResourceCompensateCounts[59]; // 70900
			int mbmWhiteContainerCountsSvarsFix[5]; // 71136
			int mbmWhiteContainerCountsSvarsDiff[5]; // 71156
			int mbmRedContainerCountsSvarsFix[5]; // 71176
			int mbmRedContainerCountsSvarsDiff[5]; // 71196
			char mbmOpenedWalkerGear; // 71216
			char mbmDataBaseSvars[459]; // 71217
			char mbmCompAnimal; // 71676
			char mbmCompDesign; // 71677
			char mbmCompPhoto; // 71678
			char __pad38[1];
			int mbmDeployMissionsSvars[71]; // 71680
			char mbmDeployMissionAveRanksSvars[71]; // 71964
			char __pad39[1];
			short mbmDeployTeamCombatStaffCounts[11]; // 72036
			char mbmDeployTeamCombatStaffRankTops[11]; // 72058
			char mbmDeployTeamCombatStaffRankBottoms[11]; // 72069
			short mbmDeployTeamSubStaffCounts[11]; // 72080
			char mbmDeployTeamSubStaffRankTops[11]; // 72102
			char mbmDeployTeamSubStaffRankBottoms[11]; // 72113
			int mbmDeployTeamCombatPoints[11]; // 72124
			int mbmDeployTeamSubPoints[11]; // 72168
			int mbmDeployTeamSvarsTimeMinute[11]; // 72212
			char mbmDeployTeamSvars4wdEastCount[22]; // 72256
			char mbmDeployTeamSvars4wdWestCount[22]; // 72278
			char mbmDeployTeamSvarsTruckEastCount[22]; // 72300
			char mbmDeployTeamSvarsTruckWestCount[22]; // 72322
			char mbmDeployTeamSvarsArmoredVehicleEastCount[22]; // 72344
			char mbmDeployTeamSvarsArmoredVehicleWestCount[22]; // 72366
			char mbmDeployTeamSvarsArmoredVehicleEastRocketCount[22]; // 72388
			char mbmDeployTeamSvarsWheeledArmoredVehicleWestCount[22]; // 72410
			char mbmDeployTeamSvarsTankEastCount[22]; // 72432
			char mbmDeployTeamSvarsTankWestCount[22]; // 72454
			char mbmDeployTeamSvarsWalkerGearProtoHeuyCount[22]; // 72476
			char mbmDeployTeamSvarsWalkerGearSovietBattleCount[22]; // 72498
			char mbmDeployTeamSvarsWalkerGearSovietSupportCount[22]; // 72520
			char mbmDeployTeamSvarsWalkerGearCfaBattleCount[22]; // 72542
			char mbmDeployTeamSvarsWalkerGearCfaSupportCount[22]; // 72564
			char __pad40[2];
			int mbmDeployRandomMissionSeedSvars; // 72588
			short mbmDeploySequentialMissionIdSvars; // 72592
			short mbmDeploySequentialMissionIdLimitSvars; // 72594
			char mbmClearedDeployRevengeMissionFlags[14]; // 72596
			char mbmDeployableBattleGear; // 72610
			char mbmBattleGearDeployingTeam; // 72611
			char mbmAddDeployLogIndex; // 72612
			char mbmCompSequential; // 72613
			short mbmNewestNRandomMissionId; // 72614
			short mbmNewestRRandomMissionId; // 72616
			char mbmOpenedRRandomMission; // 72618
			char mbmNRandomMissionClearedCount; // 72619
			char mbmIsCancelAllDeploy; // 72620
			char __pad41[3];
			int mbmClearedCountOfflineDeploy; // 72624
			int mbmClearedCountOnlineDeploy; // 72628
			int mbmPoolRewardsSyncVersion; // 72632
			int mbmPoolRewardsSvars[200]; // 72636
			int mbmPoolRewardSourceIds[200]; // 73436
			short mbmPoolRewardSeeds[200]; // 74236
			int mbmPoolRewardValues[200]; // 74636
			int mbmS10081CaptiveStaffSvarsSeed; // 75436
			int mbmS10081CaptiveStaffSvarsHeader; // 75440
			int mbmS10081CaptiveStaffSvarsStatusSync; // 75444
			short mbmS10081CaptiveStaffSvarsStatusNoSync; // 75448
			char __pad42[2];
			int mbmRemoverSvarsHeaders[64]; // 75452
			int mbmRemoverSvarsSeeds[64]; // 75708
			int mbmRemoverSvarsStatusesSync[64]; // 75964
			short mbmRemoverSvarsStatusesNoSync[64]; // 76220
			char mbmRemoverReasons[64]; // 76348
			short mbmAddRemoverIndexSvars; // 76412
			char __pad43[2];
			int mbmDeployRewardSyncVersion; // 76416
			char mbmSecuritySystemLevelsDeveloped[16]; // 76420
			char mbmAnimalHabitatAfghSvars[64]; // 76436
			char mbmAnimalHabitatMafrSvars[64]; // 76500
			char mbmIsOccupation; // 76564
			char mbmRequestDemoAttackedFromOtherPlayer; // 76565
			char mbmDemoAttackedFromOtherPlayerKnowWhereFrom; // 76566
			char __pad44[1];
			int mbmFobDefenseWinCount; // 76568
			int mbmFobDefenseLoseCount; // 76572
			char mbmIsContractOutDisplayed; // 76576
			char mbmIsEnableNuclearDevelop; // 76577
			char mbmIsNuclearDeveloped; // 76578
			char mbmIsNuclearDiscarded; // 76579
			char mbmIsNuclearDeveloping; // 76580
			char __pad45[3];
			int mbmNuclearDevelopTime; // 76584
			int mbmNuclearDevelopMaxTime; // 76588
			int mbmNuclearDevelopStartTime; // 76592
			int mbmPdTotalAddedHostageStaffCount; // 76596
			int mbmPdTotalAddedSoldierStaffCount; // 76600
			int mbmPdTotalAddedVolunteerStaffCount; // 76604
			int mbmPdTotalDeadStaffCount; // 76608
			int mbmPdTotalDefectionCount; // 76612
			int mbmPdTotalFireCount; // 76616
			int mbmPdTotalAddedGmpT; // 76620
			int mbmPdTotalAddedGmp; // 76624
			int mbmPdTotalSubtractedGmpT; // 76628
			int mbmPdTotalSubtractedGmp; // 76632
			int mbmPdTotalAddedHeroicPoint; // 76636
			int mbmPdTotalSubtractedHeroicPoint; // 76640
			int mbmPdTotalAddedResourceCounts[59]; // 76644
			int mbmPdTotalAddedAnimalCounts[99]; // 76880
			int mbmCurrentNamePlateId; // 77276
			int mbmNamePlateGotFlags[2]; // 77280
			int mbmNamePlateNewFlags[2]; // 77288
			int mbmHeroThreshold; // 77296
			int mbmNotHeroThreshold; // 77300
			short mbmPfRatingSaveSeason; // 77304
			short mbmPfRatingReadAttackMatchFlag; // 77306
			short mbmPfRatingReadDefenseMatchFlag; // 77308
			char mbmPfRatingMatchSections[15]; // 77310
			char mbmPfRatingRankingSaveSection; // 77325
			char mbmPfRatingRankingPlayerRanks[16]; // 77326
			char mbmPfRatingRankingPlayerRankDiffs[16]; // 77342
			char __pad46[2];
			int mbmPfRatingRankingPlayerIds[16]; // 77360
			char mbmPfRatingCommonFlag; // 77424
			char __pad47[1];
			short mbmPfRatingShortSaveSeason; // 77426
			short mbmPfRatingShortReadAttackMatchFlag; // 77428
			short mbmPfRatingShortReadDefenseMatchFlag; // 77430
			char mbmPfRatingShortMatchSections[15]; // 77432
			char mbmPfRatingShortRankingSaveSection; // 77447
			char mbmPfRatingShortRankingPlayerRanks[16]; // 77448
			char mbmPfRatingShortRankingPlayerRankDiffs[16]; // 77464
			int mbmPfRatingShortRankingPlayerIds[16]; // 77480
			int mbmChallengeTaskCompleteFlags[15]; // 77544
			int mbmChallengeTaskRewardReceiveFlags[15]; // 77604
			int mbmChallengeTaskOnlineVersionWindowOpen; // 77664
			char mbmServerRankingRecordedFlags[106]; // 77668
			char __pad48[2];
			int mbmServerRankingScores[106]; // 77776
			int mbmBaseColorGotFlag; // 78200
			int mbmBaseColorNewFlag; // 78204
			int mbmDlcWeaponColorGotFlag; // 78208
			int mbmDlcWeaponColorGotFlag2; // 78212
			int mbmDlcWeaponColorNewFlag; // 78216
			int mbmDlcWeaponColorNewFlag2; // 78220
			char mbmIsVisitedFobDeployWelcomeMessage1; // 78224
			char mbmIsVisitedFobDeployWelcomeMessage2; // 78225
			char mbmFobDeployCheckBoxes[6]; // 78226
			char mbmFobDeployGradeSelectorIndexes[6]; // 78232
			char optionSelectedIndices[32]; // 78238
			char armRocketYIsReverse; // 78270
			char armRocketXIsReverse; // 78271
			char bgmVolume; // 78272
			char __pad49[3];
			int keyStr[120]; // 78276
			int buttonNo[120]; // 78756
			int keybordNo[120]; // 79236
			char pushToTalk; // 79716
			char checkPointGimmickIndelibleCount; // 79717
			char __pad50[2];
			int checkPointGimmickIndelibleName[64]; // 79720
			short checkPointGimmickIndelibleState[32]; // 79976
			char missionStartGimmickIndelibleCount; // 80040
			char __pad51[3];
			int missionStartGimmickIndelibleName[64]; // 80044
			short missionStartGimmickIndelibleState[32]; // 80300
			char checkPointGimmickPermanentGimmickSaveData[2592]; // 80364
			char missionStartGimmickPermanentGimmickSaveData[2592]; // 82956
			char __pad52[1];
			char cloakEnabled[2]; // 85549
			char supCboxSaveFormVersion; // 85551
			char supCboxPhaseForSave; // 85552
			char supCboxDropType; // 85553
			char __pad53[2];
			int supCboxEquipId; // 85556
			char supCboxPartsType; // 85560
			char __pad54[3];
			int supCboxPos[3]; // 85564
			int supCboxRot[4]; // 85576
			char supCboxLevelWait; // 85592
			char __pad55[3];
			int supCboxAdvancedRate; // 85596
			int supCboxFallHeight; // 85600
			char supCboxLevels[13]; // 85604
			char supCboxFlags; // 85617
			char supCboxU8buf[115]; // 85618
			char __pad56[1];
			short supCboxU16buf[36]; // 85734
			char __pad57[2];
			int supCboxU32buf[17]; // 85808
			short supCboxLocationOnSave; // 85876
			char supAttackStateForSave; // 85878
			char supAttackAttackType; // 85879
			int supAttackPos[3]; // 85880
			int supAttackRot[4]; // 85892
			char supAttackLevelWait; // 85908
			char supAttackLevelAttack; // 85909
			char supAttackFlags; // 85910
			char __pad58[1];
			int supAttackCommonValue; // 85912
			int supAttackWaitTime; // 85916
			short supAttackLocationOnSave; // 85920
			char stealthAssistLeftCount; // 85922
			char __pad59[1];
			int tppTransferFlag; // 85924
			int tppTransferAcquiredFlag; // 85928
			int dlcPermittedFlag[2]; // 85932
			int dlcAcquiredFlag[2]; // 85940
			int mbDvcTutorialFlag; // 85948
			int mbDvcTopMenuFlag; // 85952
			char heliMusicType; // 85956
			char __pad60[3];
			int heliMusicTrackID; // 85960
			int radioReadFlagGlobalScoped[200]; // 85964
			int markerLocatorInfos[100]; // 86764
			char tipsReadFlag[200]; // 87164
			int userMarkerPosX[5]; // 87364
			int userMarkerPosY[5]; // 87384
			int userMarkerPosZ[5]; // 87404
			short userMarkerAddFlag[5]; // 87424
			short userMarkerGameObjId[5]; // 87434
			short userMarkerLocationId; // 87444
			char userMarkerSaveCount; // 87446
			char __pad61[1];
			int fobResultRankingDateTime; // 87448
			char fobResultRankingPrevRanks[16]; // 87452
			int fobEventResultRankingDateTime; // 87468
			char fobEventResultRankingPrevRanks[16]; // 87472
		};

		static_assert(sizeof(ScriptVars_tpp) == 87488);
		static_assert(offsetof(ScriptVars_tpp, utcTime) == 0);
		static_assert(offsetof(ScriptVars_tpp, totalPlayTime) == 8);
		static_assert(offsetof(ScriptVars_tpp, locationCode) == 16);
		static_assert(offsetof(ScriptVars_tpp, missionCode) == 18);
		static_assert(offsetof(ScriptVars_tpp, prevLocationCode) == 20);
		static_assert(offsetof(ScriptVars_tpp, prevMissionCode) == 22);
		static_assert(offsetof(ScriptVars_tpp, rulesetDynamicPackIndices) == 28);
		static_assert(offsetof(ScriptVars_tpp, mbLayoutCode) == 32);
		static_assert(offsetof(ScriptVars_tpp, mbClusterId) == 34);
		static_assert(offsetof(ScriptVars_tpp, fobSneakMode) == 35);
		static_assert(offsetof(ScriptVars_tpp, fobIsSneak) == 36);
		static_assert(offsetof(ScriptVars_tpp, fobIsEvent) == 37);
		static_assert(offsetof(ScriptVars_tpp, fobIsSecurity) == 38);
		static_assert(offsetof(ScriptVars_tpp, fobIsPlaceMode) == 39);
		static_assert(offsetof(ScriptVars_tpp, fobIsInitializedFreePositionItem) == 40);
		static_assert(offsetof(ScriptVars_tpp, returnStaffHeader) == 44);
		static_assert(offsetof(ScriptVars_tpp, returnStaffSeeds) == 48);
		static_assert(offsetof(ScriptVars_tpp, eulaVersion) == 52);
		static_assert(offsetof(ScriptVars_tpp, eulaVersion2) == 56);
		static_assert(offsetof(ScriptVars_tpp, eulaVersion3) == 60);
		static_assert(offsetof(ScriptVars_tpp, countryIndex) == 64);
		static_assert(offsetof(ScriptVars_tpp, countryStateIndex) == 68);
		static_assert(offsetof(ScriptVars_tpp, infoId) == 72);
		static_assert(offsetof(ScriptVars_tpp, infoIdForMGO) == 76);
		static_assert(offsetof(ScriptVars_tpp, inquiryId) == 80);
		static_assert(offsetof(ScriptVars_tpp, fobPickup) == 84);
		static_assert(offsetof(ScriptVars_tpp, padType) == 88);
		static_assert(offsetof(ScriptVars_tpp, clock) == 92);
		static_assert(offsetof(ScriptVars_tpp, passageSecondsSinceOutMB) == 100);
		static_assert(offsetof(ScriptVars_tpp, weather) == 104);
		static_assert(offsetof(ScriptVars_tpp, weatherNextTime) == 108);
		static_assert(offsetof(ScriptVars_tpp, extraWeatherInterval) == 112);
		static_assert(offsetof(ScriptVars_tpp, weatherAddDensity) == 116);
		static_assert(offsetof(ScriptVars_tpp, requestWeatherType) == 120);
		static_assert(offsetof(ScriptVars_tpp, requestWeatherUserId) == 128);
		static_assert(offsetof(ScriptVars_tpp, requestWeatherInterp) == 148);
		static_assert(offsetof(ScriptVars_tpp, requestWeatherParam) == 168);
		static_assert(offsetof(ScriptVars_tpp, dominatedCpFlagsAfgh) == 188);
		static_assert(offsetof(ScriptVars_tpp, dominatedCpFlagsMafr) == 204);
		static_assert(offsetof(ScriptVars_tpp, totalAlertCount) == 220);
		static_assert(offsetof(ScriptVars_tpp, initialPlayerPosX) == 224);
		static_assert(offsetof(ScriptVars_tpp, initialPlayerPosY) == 228);
		static_assert(offsetof(ScriptVars_tpp, initialPlayerPosZ) == 232);
		static_assert(offsetof(ScriptVars_tpp, playerPosX) == 236);
		static_assert(offsetof(ScriptVars_tpp, playerPosY) == 240);
		static_assert(offsetof(ScriptVars_tpp, playerPosZ) == 244);
		static_assert(offsetof(ScriptVars_tpp, playerPartsType) == 248);
		static_assert(offsetof(ScriptVars_tpp, playerCamoType) == 249);
		static_assert(offsetof(ScriptVars_tpp, playerHandType) == 250);
		static_assert(offsetof(ScriptVars_tpp, playerType) == 251);
		static_assert(offsetof(ScriptVars_tpp, playerFaceId) == 252);
		static_assert(offsetof(ScriptVars_tpp, playerFaceEquipId) == 254);
		static_assert(offsetof(ScriptVars_tpp, playerSkillId) == 256);
		static_assert(offsetof(ScriptVars_tpp, playerInjuryCount) == 257);
		static_assert(offsetof(ScriptVars_tpp, playerBodyInjuryCount) == 258);
		static_assert(offsetof(ScriptVars_tpp, playerLifeMax) == 260);
		static_assert(offsetof(ScriptVars_tpp, initialPlayerRotY) == 262);
		static_assert(offsetof(ScriptVars_tpp, playerRotY) == 264);
		static_assert(offsetof(ScriptVars_tpp, initialPlayerFlag) == 266);
		static_assert(offsetof(ScriptVars_tpp, initialPlayerAction) == 268);
		static_assert(offsetof(ScriptVars_tpp, playerRetryFlag) == 269);
		static_assert(offsetof(ScriptVars_tpp, initialPlayerPairGameObjectId) == 270);
		static_assert(offsetof(ScriptVars_tpp, playerVehicleGameObjectId) == 272);
		static_assert(offsetof(ScriptVars_tpp, initialPlayerVehicleGameObjectId) == 274);
		static_assert(offsetof(ScriptVars_tpp, vehicleGameObjectIdOnPlayerOnCargo) == 276);
		static_assert(offsetof(ScriptVars_tpp, playerDisableActionFlag) == 280);
		static_assert(offsetof(ScriptVars_tpp, playerLife) == 284);
		static_assert(offsetof(ScriptVars_tpp, playerStamina) == 286);
		static_assert(offsetof(ScriptVars_tpp, playerCameraPosition) == 288);
		static_assert(offsetof(ScriptVars_tpp, playerCameraRotation) == 300);
		static_assert(offsetof(ScriptVars_tpp, playerStaffHeader) == 308);
		static_assert(offsetof(ScriptVars_tpp, playerStaffSeed) == 312);
		static_assert(offsetof(ScriptVars_tpp, playerPlayFlag) == 316);
		static_assert(offsetof(ScriptVars_tpp, playerMarkingCountInMission) == 320);
		static_assert(offsetof(ScriptVars_tpp, playerShootCountInMission) == 324);
		static_assert(offsetof(ScriptVars_tpp, shootHitCountInMission) == 328);
		static_assert(offsetof(ScriptVars_tpp, shootHitCountEliminatedInMission) == 332);
		static_assert(offsetof(ScriptVars_tpp, playerOgrePointOnStartMission) == 336);
		static_assert(offsetof(ScriptVars_tpp, playerRecoveryCountFromOgre) == 340);
		static_assert(offsetof(ScriptVars_tpp, totalChickenCapTime) == 344);
		static_assert(offsetof(ScriptVars_tpp, totalChickCapTime) == 348);
		static_assert(offsetof(ScriptVars_tpp, totalDiscoveredCount) == 352);
		static_assert(offsetof(ScriptVars_tpp, totalTakeHitCount) == 356);
		static_assert(offsetof(ScriptVars_tpp, totalMovingDistanceAtStand) == 360);
		static_assert(offsetof(ScriptVars_tpp, totalMovingDistanceAtSquat) == 364);
		static_assert(offsetof(ScriptVars_tpp, totalMovingDistanceAtCrawl) == 368);
		static_assert(offsetof(ScriptVars_tpp, totalMovingDistanceOnHorse) == 372);
		static_assert(offsetof(ScriptVars_tpp, totalMovingDistanceOnVehicle) == 376);
		static_assert(offsetof(ScriptVars_tpp, totalMovingDistanceOnWalkerGear) == 380);
		static_assert(offsetof(ScriptVars_tpp, totalMovingDistanceOnBattleGear) == 384);
		static_assert(offsetof(ScriptVars_tpp, totalMarkingCount) == 388);
		static_assert(offsetof(ScriptVars_tpp, totalReflexCount) == 392);
		static_assert(offsetof(ScriptVars_tpp, useBlackDiamondEmblem) == 396);
		static_assert(offsetof(ScriptVars_tpp, isAvatarPlayerEnable) == 397);
		static_assert(offsetof(ScriptVars_tpp, avatarFaceRaceIndex) == 398);
		static_assert(offsetof(ScriptVars_tpp, avatarFaceTypeIndex) == 399);
		static_assert(offsetof(ScriptVars_tpp, avatarFaceVariationIndex) == 400);
		static_assert(offsetof(ScriptVars_tpp, avatarFaceColorIndex) == 401);
		static_assert(offsetof(ScriptVars_tpp, avatarHairStyleIndex) == 402);
		static_assert(offsetof(ScriptVars_tpp, avatarRightEyeColorIndex) == 403);
		static_assert(offsetof(ScriptVars_tpp, avatarRightEyeBrightnessIndex) == 404);
		static_assert(offsetof(ScriptVars_tpp, avatarLeftEyeColorIndex) == 405);
		static_assert(offsetof(ScriptVars_tpp, avatarLeftEyeBrightnessIndex) == 406);
		static_assert(offsetof(ScriptVars_tpp, avatarHairColor) == 407);
		static_assert(offsetof(ScriptVars_tpp, avatarBerdStyle) == 408);
		static_assert(offsetof(ScriptVars_tpp, avatarBerdLength) == 409);
		static_assert(offsetof(ScriptVars_tpp, avatarEbrwStyle) == 410);
		static_assert(offsetof(ScriptVars_tpp, avatarEbrwWide) == 411);
		static_assert(offsetof(ScriptVars_tpp, avatarGashOrTatoVariationIndex) == 412);
		static_assert(offsetof(ScriptVars_tpp, avatarTatoColorIndex) == 413);
		static_assert(offsetof(ScriptVars_tpp, avatarAcceFlag) == 414);
		static_assert(offsetof(ScriptVars_tpp, avatarMotionFrame) == 415);
		static_assert(offsetof(ScriptVars_tpp, avatarSaveIsValid) == 475);
		static_assert(offsetof(ScriptVars_tpp, avatarEyePresetID) == 476);
		static_assert(offsetof(ScriptVars_tpp, avatarNosePresetID) == 477);
		static_assert(offsetof(ScriptVars_tpp, avatarMouthPresetID) == 478);
		static_assert(offsetof(ScriptVars_tpp, avatarChinPresetID) == 479);
		static_assert(offsetof(ScriptVars_tpp, avatarCheekPresetID) == 480);
		static_assert(offsetof(ScriptVars_tpp, avatarEyebrowPresetID) == 481);
		static_assert(offsetof(ScriptVars_tpp, avatarHeadPresetID) == 482);
		static_assert(offsetof(ScriptVars_tpp, avatarHairPresetID) == 483);
		static_assert(offsetof(ScriptVars_tpp, avatarHairColorPresetID) == 484);
		static_assert(offsetof(ScriptVars_tpp, avatarSkinColorPresetID) == 485);
		static_assert(offsetof(ScriptVars_tpp, avatarBeardPresetID) == 486);
		static_assert(offsetof(ScriptVars_tpp, avatarAccessoryPresetID) == 487);
		static_assert(offsetof(ScriptVars_tpp, avatarAllPartsPresetID) == 488);
		static_assert(offsetof(ScriptVars_tpp, avatarReserve0) == 489);
		static_assert(offsetof(ScriptVars_tpp, avatarReserve1) == 490);
		static_assert(offsetof(ScriptVars_tpp, avatarReserve2) == 491);
		static_assert(offsetof(ScriptVars_tpp, personalName) == 492);
		static_assert(offsetof(ScriptVars_tpp, personalBirthdayMonth) == 556);
		static_assert(offsetof(ScriptVars_tpp, personalBirthdayDay) == 557);
		static_assert(offsetof(ScriptVars_tpp, emblemTextureTag) == 560);
		static_assert(offsetof(ScriptVars_tpp, emblemColorL) == 576);
		static_assert(offsetof(ScriptVars_tpp, emblemColorH) == 592);
		static_assert(offsetof(ScriptVars_tpp, emblemX) == 608);
		static_assert(offsetof(ScriptVars_tpp, emblemY) == 612);
		static_assert(offsetof(ScriptVars_tpp, emblemScale) == 616);
		static_assert(offsetof(ScriptVars_tpp, emblemRotate) == 620);
		static_assert(offsetof(ScriptVars_tpp, emblemFlag) == 624);
		static_assert(offsetof(ScriptVars_tpp, emblemVersion) == 1304);
		static_assert(offsetof(ScriptVars_tpp, emblemSyncFailed) == 1305);
		static_assert(offsetof(ScriptVars_tpp, emblemSyncFailed2) == 1306);
		static_assert(offsetof(ScriptVars_tpp, isServerDataCleared) == 1307);
		static_assert(offsetof(ScriptVars_tpp, isDefaultDisconnected) == 1308);
		static_assert(offsetof(ScriptVars_tpp, didCancelPatchDlcDownloadRequest) == 1309);
		static_assert(offsetof(ScriptVars_tpp, didCancelFobPatchDlcDownloadRequest) == 1310);
		static_assert(offsetof(ScriptVars_tpp, isPersonalDirty) == 1311);
		static_assert(offsetof(ScriptVars_tpp, initWeapons) == 1312);
		static_assert(offsetof(ScriptVars_tpp, initSupportWeapons) == 1318);
		static_assert(offsetof(ScriptVars_tpp, initItems) == 1334);
		static_assert(offsetof(ScriptVars_tpp, initHandEquip) == 1350);
		static_assert(offsetof(ScriptVars_tpp, weapons) == 1352);
		static_assert(offsetof(ScriptVars_tpp, ammoInWeapons) == 1364);
		static_assert(offsetof(ScriptVars_tpp, ammoSubInWeapons) == 1376);
		static_assert(offsetof(ScriptVars_tpp, suppressorLives) == 1388);
		static_assert(offsetof(ScriptVars_tpp, isUseSuppressor) == 1394);
		static_assert(offsetof(ScriptVars_tpp, isUseFlashLight) == 1400);
		static_assert(offsetof(ScriptVars_tpp, isInitialWeapon) == 1406);
		static_assert(offsetof(ScriptVars_tpp, supportWeapons) == 1412);
		static_assert(offsetof(ScriptVars_tpp, items) == 1444);
		static_assert(offsetof(ScriptVars_tpp, cboxPosterIndices) == 1476);
		static_assert(offsetof(ScriptVars_tpp, handEquip) == 1492);
		static_assert(offsetof(ScriptVars_tpp, upgrades) == 1494);
		static_assert(offsetof(ScriptVars_tpp, ammoStockIds) == 1510);
		static_assert(offsetof(ScriptVars_tpp, ammoStockCounts) == 1556);
		static_assert(offsetof(ScriptVars_tpp, totalBatteryPowerAsGmp) == 1648);
		static_assert(offsetof(ScriptVars_tpp, currentInventorySlot) == 1652);
		static_assert(offsetof(ScriptVars_tpp, currentSupportWeaponIndex) == 1656);
		static_assert(offsetof(ScriptVars_tpp, currentItemIndex) == 1660);
		static_assert(offsetof(ScriptVars_tpp, fpsAttackMode) == 1664);
		static_assert(offsetof(ScriptVars_tpp, currentZoomStep) == 1668);
		static_assert(offsetof(ScriptVars_tpp, requestFlagsAboutEquip) == 1673);
		static_assert(offsetof(ScriptVars_tpp, weaponOriginalOwners) == 1674);
		static_assert(offsetof(ScriptVars_tpp, partsType2) == 1680);
		static_assert(offsetof(ScriptVars_tpp, camoType2) == 1681);
		static_assert(offsetof(ScriptVars_tpp, faceEquipId2) == 1682);
		static_assert(offsetof(ScriptVars_tpp, weapons2) == 1684);
		static_assert(offsetof(ScriptVars_tpp, supportWeapons2) == 1690);
		static_assert(offsetof(ScriptVars_tpp, items2) == 1706);
		static_assert(offsetof(ScriptVars_tpp, handEquip2) == 1722);
		static_assert(offsetof(ScriptVars_tpp, dirtyEffectCount) == 1724);
		static_assert(offsetof(ScriptVars_tpp, dirtyEffectBoneIndex) == 1726);
		static_assert(offsetof(ScriptVars_tpp, dirtyEffectPosition) == 1792);
		static_assert(offsetof(ScriptVars_tpp, dirtyEffectRadius) == 2560);
		static_assert(offsetof(ScriptVars_tpp, initCustomizedWeapon) == 2816);
		static_assert(offsetof(ScriptVars_tpp, customizedWeapon) == 2822);
		static_assert(offsetof(ScriptVars_tpp, initChimeraParts) == 2828);
		static_assert(offsetof(ScriptVars_tpp, chimeraParts) == 2864);
		static_assert(offsetof(ScriptVars_tpp, initChimeraPaintType) == 2900);
		static_assert(offsetof(ScriptVars_tpp, chimeraPaintType) == 2903);
		static_assert(offsetof(ScriptVars_tpp, initChimeraColorIndex) == 2906);
		static_assert(offsetof(ScriptVars_tpp, chimeraColorIndex) == 2909);
		static_assert(offsetof(ScriptVars_tpp, isUseCustomizedWeapon) == 2912);
		static_assert(offsetof(ScriptVars_tpp, customizedWeaponSlotIndex) == 2915);
		static_assert(offsetof(ScriptVars_tpp, itemLevels) == 2918);
		static_assert(offsetof(ScriptVars_tpp, itemLevelsDeveloped) == 2990);
		static_assert(offsetof(ScriptVars_tpp, developedItemLevels) == 3062);
		static_assert(offsetof(ScriptVars_tpp, suitDevelopLevel) == 3134);
		static_assert(offsetof(ScriptVars_tpp, initLoadoutInfo) == 3368);
		static_assert(offsetof(ScriptVars_tpp, loadoutInfoSaveFormVersion) == 3369);
		static_assert(offsetof(ScriptVars_tpp, loadoutInfoU8buf) == 3370);
		static_assert(offsetof(ScriptVars_tpp, loadoutInfoU16buf) == 3830);
		static_assert(offsetof(ScriptVars_tpp, loadoutInfoU32buf) == 4120);
		static_assert(offsetof(ScriptVars_tpp, currentLoadoutSlot) == 4392);
		static_assert(offsetof(ScriptVars_tpp, sortieLoadoutInfoSaveFormVersion) == 4393);
		static_assert(offsetof(ScriptVars_tpp, sortieLoadoutInfoU8buf) == 4394);
		static_assert(offsetof(ScriptVars_tpp, sortieLoadoutInfoU16buf) == 4854);
		static_assert(offsetof(ScriptVars_tpp, sortieLoadoutInfoU32buf) == 5144);
		static_assert(offsetof(ScriptVars_tpp, currentSortieLoadoutSlot) == 5416);
		static_assert(offsetof(ScriptVars_tpp, currentSortieLoadoutSlotNonRental) == 5417);
		static_assert(offsetof(ScriptVars_tpp, returnHeliLoadoutInfoSaveFormVersion) == 5418);
		static_assert(offsetof(ScriptVars_tpp, returnHeliLoadoutInfoU8buf) == 5419);
		static_assert(offsetof(ScriptVars_tpp, returnHeliLoadoutInfoU16buf) == 5880);
		static_assert(offsetof(ScriptVars_tpp, returnHeliLoadoutInfoU32buf) == 6168);
		static_assert(offsetof(ScriptVars_tpp, currentReturnHeliLoadoutSlot) == 6440);
		static_assert(offsetof(ScriptVars_tpp, currentForRentLoadoutSlotIndex) == 6441);
		static_assert(offsetof(ScriptVars_tpp, rentalLoadoutPlayerId) == 6444);
		static_assert(offsetof(ScriptVars_tpp, rentalLoadoutPlayerName) == 6448);
		static_assert(offsetof(ScriptVars_tpp, sortiePrepPlayerSnakePartsType) == 6548);
		static_assert(offsetof(ScriptVars_tpp, sortiePrepPlayerSnakeCamoType) == 6549);
		static_assert(offsetof(ScriptVars_tpp, sortiePrepPlayerSnakeFaceEquipId) == 6550);
		static_assert(offsetof(ScriptVars_tpp, sortiePrepPlayerSnakeSuitLevel) == 6552);
		static_assert(offsetof(ScriptVars_tpp, sortiePrepPlayerDDPartsType) == 6553);
		static_assert(offsetof(ScriptVars_tpp, sortiePrepPlayerDDCamoType) == 6554);
		static_assert(offsetof(ScriptVars_tpp, sortiePrepPlayerDDFaceEquipId) == 6556);
		static_assert(offsetof(ScriptVars_tpp, sortiePrepPlayerDDSuitLevel) == 6558);
		static_assert(offsetof(ScriptVars_tpp, sortiePrepPlayerType) == 6559);
		static_assert(offsetof(ScriptVars_tpp, sortiePrepPlayerStaffHeader) == 6560);
		static_assert(offsetof(ScriptVars_tpp, sortiePrepPlayerStaffSeed) == 6564);
		static_assert(offsetof(ScriptVars_tpp, sortiePrepPlayerFaceId) == 6568);
		static_assert(offsetof(ScriptVars_tpp, userPresetCustomizedWeapon) == 6572);
		static_assert(offsetof(ScriptVars_tpp, userPresetChimeraParts) == 6620);
		static_assert(offsetof(ScriptVars_tpp, userPresetChimeraColorIndex) == 6908);
		static_assert(offsetof(ScriptVars_tpp, userPresetChimeraPaintType) == 6932);
		static_assert(offsetof(ScriptVars_tpp, userPresetDWalkerParts) == 6956);
		static_assert(offsetof(ScriptVars_tpp, userPresetDWalkerPartsLevel) == 6980);
		static_assert(offsetof(ScriptVars_tpp, userPresetDWalkerAdditionalParts) == 7004);
		static_assert(offsetof(ScriptVars_tpp, userPresetDWalkerAdditionalPartsLevel) == 7016);
		static_assert(offsetof(ScriptVars_tpp, userPresetDWalkerCurrentSlot) == 7028);
		static_assert(offsetof(ScriptVars_tpp, userPresetBattleGearParts) == 7029);
		static_assert(offsetof(ScriptVars_tpp, userPresetBattleGearPartsLevel) == 7056);
		static_assert(offsetof(ScriptVars_tpp, userPresetBattleGearCurrentSlot) == 7083);
		static_assert(offsetof(ScriptVars_tpp, userPresetHeliParts) == 7084);
		static_assert(offsetof(ScriptVars_tpp, userPresetVehiclePaintType) == 7089);
		static_assert(offsetof(ScriptVars_tpp, userPresetBuddyDogParts) == 7099);
		static_assert(offsetof(ScriptVars_tpp, userPresetBuddyHorseParts) == 7101);
		static_assert(offsetof(ScriptVars_tpp, mbmUserPresetBaseColor) == 7104);
		static_assert(offsetof(ScriptVars_tpp, droppedWeapons) == 7106);
		static_assert(offsetof(ScriptVars_tpp, droppedWeaponPos) == 7112);
		static_assert(offsetof(ScriptVars_tpp, droppedWeaponRot) == 7148);
		static_assert(offsetof(ScriptVars_tpp, droppedCount) == 7196);
		static_assert(offsetof(ScriptVars_tpp, droppedAmmo) == 7208);
		static_assert(offsetof(ScriptVars_tpp, droppedSuppressorLife) == 7220);
		static_assert(offsetof(ScriptVars_tpp, droppedCarrier) == 7224);
		static_assert(offsetof(ScriptVars_tpp, droppedCarrierPlaceNo) == 7230);
		static_assert(offsetof(ScriptVars_tpp, droppedWeaponStatus) == 7233);
		static_assert(offsetof(ScriptVars_tpp, importantItems) == 7236);
		static_assert(offsetof(ScriptVars_tpp, importantItemNumber) == 7300);
		static_assert(offsetof(ScriptVars_tpp, importantItemPos) == 7364);
		static_assert(offsetof(ScriptVars_tpp, importantItemRot) == 7748);
		static_assert(offsetof(ScriptVars_tpp, importantItemStatus) == 8260);
		static_assert(offsetof(ScriptVars_tpp, markingsOfPickable) == 8292);
		static_assert(offsetof(ScriptVars_tpp, missionCodeMarkingsOfPickable) == 8424);
		static_assert(offsetof(ScriptVars_tpp, markingsOfPlaced) == 8428);
		static_assert(offsetof(ScriptVars_tpp, missionCodeMarkingsOfPlaced) == 8560);
		static_assert(offsetof(ScriptVars_tpp, placedWeapons) == 8562);
		static_assert(offsetof(ScriptVars_tpp, placedOwnerIds) == 8626);
		static_assert(offsetof(ScriptVars_tpp, placedPos) == 8692);
		static_assert(offsetof(ScriptVars_tpp, placedRot) == 9076);
		static_assert(offsetof(ScriptVars_tpp, placedAttachTargetNames) == 9588);
		static_assert(offsetof(ScriptVars_tpp, placedAttachTargetIndex) == 9844);
		static_assert(offsetof(ScriptVars_tpp, placedAttachTargetGameObjectType) == 9908);
		static_assert(offsetof(ScriptVars_tpp, placedAttachPoints) == 9940);
		static_assert(offsetof(ScriptVars_tpp, placedCorrelationId) == 10196);
		static_assert(offsetof(ScriptVars_tpp, placedTargetGroupBlast) == 10260);
		static_assert(offsetof(ScriptVars_tpp, placedStatus) == 10324);
		static_assert(offsetof(ScriptVars_tpp, playerDecoyInstanceIndicies) == 10356);
		static_assert(offsetof(ScriptVars_tpp, playerDecoyAngleBallon) == 10380);
		static_assert(offsetof(ScriptVars_tpp, caputreAnimalId) == 10428);
		static_assert(offsetof(ScriptVars_tpp, captureAreaName) == 10460);
		static_assert(offsetof(ScriptVars_tpp, captureAnimalIndex) == 10524);
		static_assert(offsetof(ScriptVars_tpp, collectionPositionsMoved) == 10528);
		static_assert(offsetof(ScriptVars_tpp, collectionRotationsMoved) == 10912);
		static_assert(offsetof(ScriptVars_tpp, collectionLocatorIndicesMoved) == 11040);
		static_assert(offsetof(ScriptVars_tpp, collectionMovedIndexEnd) == 11104);
		static_assert(offsetof(ScriptVars_tpp, locationCodeOfCollectionMoved) == 11108);
		static_assert(offsetof(ScriptVars_tpp, cboxIsValid) == 11110);
		static_assert(offsetof(ScriptVars_tpp, cboxLife) == 11128);
		static_assert(offsetof(ScriptVars_tpp, cboxFlag) == 11192);
		static_assert(offsetof(ScriptVars_tpp, cboxPosterType) == 11224);
		static_assert(offsetof(ScriptVars_tpp, cboxEquipId) == 11240);
		static_assert(offsetof(ScriptVars_tpp, cboxLocation) == 11272);
		static_assert(offsetof(ScriptVars_tpp, buddyTypeMissionStart) == 11528);
		static_assert(offsetof(ScriptVars_tpp, obtainedBuddyTypeMissionStart) == 11529);
		static_assert(offsetof(ScriptVars_tpp, sortieBuddyTypeMissionStart) == 11530);
		static_assert(offsetof(ScriptVars_tpp, deadBuddyTypeMissionStart) == 11531);
		static_assert(offsetof(ScriptVars_tpp, initialBuddyStatusMissionStart) == 11532);
		static_assert(offsetof(ScriptVars_tpp, initialBuddyPosMissionStart) == 11536);
		static_assert(offsetof(ScriptVars_tpp, initialBuddyRotYMissionStart) == 11548);
		static_assert(offsetof(ScriptVars_tpp, buddyCommonFlagMissionStart) == 11552);
		static_assert(offsetof(ScriptVars_tpp, buddyFriendlyPointMissionStart) == 11556);
		static_assert(offsetof(ScriptVars_tpp, reliefVehicleCarryType) == 11572);
		static_assert(offsetof(ScriptVars_tpp, reliefVehicleCarryAttitude) == 11576);
		static_assert(offsetof(ScriptVars_tpp, reliefVehicleCarryLife) == 11592);
		static_assert(offsetof(ScriptVars_tpp, reliefVehicleCarryAmmo) == 11616);
		static_assert(offsetof(ScriptVars_tpp, reliefVehicleCarryState) == 11622);
		static_assert(offsetof(ScriptVars_tpp, buddyType) == 11624);
		static_assert(offsetof(ScriptVars_tpp, obtainedBuddyType) == 11625);
		static_assert(offsetof(ScriptVars_tpp, sortieBuddyType) == 11626);
		static_assert(offsetof(ScriptVars_tpp, disableCallBuddyType) == 11627);
		static_assert(offsetof(ScriptVars_tpp, deadBuddyType) == 11628);
		static_assert(offsetof(ScriptVars_tpp, initialBuddyStatus) == 11629);
		static_assert(offsetof(ScriptVars_tpp, initialBuddyPos) == 11632);
		static_assert(offsetof(ScriptVars_tpp, initialBuddyRotY) == 11644);
		static_assert(offsetof(ScriptVars_tpp, buddyCommonFlag) == 11648);
		static_assert(offsetof(ScriptVars_tpp, buddyMissionFlag) == 11652);
		static_assert(offsetof(ScriptVars_tpp, buddyCallCount) == 11656);
		static_assert(offsetof(ScriptVars_tpp, buddyEspionageCpNames) == 11684);
		static_assert(offsetof(ScriptVars_tpp, buddyFriendlyTimer) == 11716);
		static_assert(offsetof(ScriptVars_tpp, buddyFriendlyPoint) == 11748);
		static_assert(offsetof(ScriptVars_tpp, buddyFriendlyMaxPoint) == 11764);
		static_assert(offsetof(ScriptVars_tpp, buddyFriendlyCount) == 11780);
		static_assert(offsetof(ScriptVars_tpp, buddyQuietEquipType) == 11844);
		static_assert(offsetof(ScriptVars_tpp, buddyQuietCostumeType) == 11846);
		static_assert(offsetof(ScriptVars_tpp, buddyDogFlag) == 11847);
		static_assert(offsetof(ScriptVars_tpp, buddyDogMode) == 11848);
		static_assert(offsetof(ScriptVars_tpp, buddyDogEquipType) == 11849);
		static_assert(offsetof(ScriptVars_tpp, buddyDogEyeType) == 11850);
		static_assert(offsetof(ScriptVars_tpp, buddyDogColorType) == 11851);
		static_assert(offsetof(ScriptVars_tpp, buddyGearAmmoCount) == 11852);
		static_assert(offsetof(ScriptVars_tpp, buddyGearStockAmmoCount) == 11856);
		static_assert(offsetof(ScriptVars_tpp, buddyGearBodyGrade) == 11860);
		static_assert(offsetof(ScriptVars_tpp, buddyGearEquipGrade) == 11861);
		static_assert(offsetof(ScriptVars_tpp, buddyGearReloadGrade) == 11862);
		static_assert(offsetof(ScriptVars_tpp, buddyGearHeadType) == 11863);
		static_assert(offsetof(ScriptVars_tpp, buddyGearHeadGrade) == 11864);
		static_assert(offsetof(ScriptVars_tpp, buddyGearArmType) == 11865);
		static_assert(offsetof(ScriptVars_tpp, buddyGearArmGrade) == 11866);
		static_assert(offsetof(ScriptVars_tpp, buddyGearMainWPType) == 11867);
		static_assert(offsetof(ScriptVars_tpp, buddyGearMainWPGrade) == 11868);
		static_assert(offsetof(ScriptVars_tpp, buddyGearSubWPType) == 11869);
		static_assert(offsetof(ScriptVars_tpp, buddyGearSubWPGrade) == 11870);
		static_assert(offsetof(ScriptVars_tpp, buddyGearColoring0) == 11871);
		static_assert(offsetof(ScriptVars_tpp, buddyGearColoring1) == 11872);
		static_assert(offsetof(ScriptVars_tpp, buddyHorseType) == 11873);
		static_assert(offsetof(ScriptVars_tpp, buddyHorseColorType) == 11874);
		static_assert(offsetof(ScriptVars_tpp, buddyHorseHeadDesignType) == 11875);
		static_assert(offsetof(ScriptVars_tpp, buddyHorseFootDesignType) == 11876);
		static_assert(offsetof(ScriptVars_tpp, battleGearAmmoCountMain) == 11878);
		static_assert(offsetof(ScriptVars_tpp, battleGearAmmoCountSub) == 11882);
		static_assert(offsetof(ScriptVars_tpp, battleGearStockAmmoCountMain) == 11884);
		static_assert(offsetof(ScriptVars_tpp, battleGearStockAmmoCountSub) == 11888);
		static_assert(offsetof(ScriptVars_tpp, battleGearMainWPType) == 11890);
		static_assert(offsetof(ScriptVars_tpp, battleGearMainWPGrade) == 11892);
		static_assert(offsetof(ScriptVars_tpp, battleGearSubWPType) == 11894);
		static_assert(offsetof(ScriptVars_tpp, battleGearSubWPGrade) == 11895);
		static_assert(offsetof(ScriptVars_tpp, battleGearColoring0) == 11896);
		static_assert(offsetof(ScriptVars_tpp, battleGearColoring1) == 11897);
		static_assert(offsetof(ScriptVars_tpp, maxPhase) == 11898);
		static_assert(offsetof(ScriptVars_tpp, playerPhase) == 11899);
		static_assert(offsetof(ScriptVars_tpp, sahelanBrokenMesh) == 11900);
		static_assert(offsetof(ScriptVars_tpp, supportHeliCustomize) == 11904);
		static_assert(offsetof(ScriptVars_tpp, pfRatingPoint) == 11908);
		static_assert(offsetof(ScriptVars_tpp, mbmTppGmp) == 11912);
		static_assert(offsetof(ScriptVars_tpp, mbmServerWalletGmp) == 11916);
		static_assert(offsetof(ScriptVars_tpp, mbmTppHeroicPoint) == 11920);
		static_assert(offsetof(ScriptVars_tpp, missionHeroicPoint) == 11924);
		static_assert(offsetof(ScriptVars_tpp, mbmTppOgrePoint) == 11928);
		static_assert(offsetof(ScriptVars_tpp, missionOgrePoint) == 11932);
		static_assert(offsetof(ScriptVars_tpp, mbmRequestMisogi) == 11936);
		static_assert(offsetof(ScriptVars_tpp, mbmTppTotalIncomeGmp) == 11940);
		static_assert(offsetof(ScriptVars_tpp, mbmIsOpenedCombatSection) == 11944);
		static_assert(offsetof(ScriptVars_tpp, mbmIsOpenedBaseDevSection) == 11945);
		static_assert(offsetof(ScriptVars_tpp, mbmIsOpenedSpySection) == 11946);
		static_assert(offsetof(ScriptVars_tpp, mbmIsOpenedMedicalSection) == 11947);
		static_assert(offsetof(ScriptVars_tpp, mbmIsOpenedSecuritySection) == 11948);
		static_assert(offsetof(ScriptVars_tpp, mbmIsOpenedHospitalSection) == 11949);
		static_assert(offsetof(ScriptVars_tpp, mbmIsOpenedPrisonSection) == 11950);
		static_assert(offsetof(ScriptVars_tpp, mbmIsOpenedSeparationSection) == 11951);
		static_assert(offsetof(ScriptVars_tpp, mbmSectionFuncSvars) == 11952);
		static_assert(offsetof(ScriptVars_tpp, mbmAutoGmpElapsedTimeMinuteSvars) == 11994);
		static_assert(offsetof(ScriptVars_tpp, mbmAutoResourceElapsedTimeMinuteSvars) == 11995);
		static_assert(offsetof(ScriptVars_tpp, mbmCommonMetalProcessingElapsedTimeMinute) == 11996);
		static_assert(offsetof(ScriptVars_tpp, mbmMinorMetalProcessingElapsedTimeMinute) == 11997);
		static_assert(offsetof(ScriptVars_tpp, mbmPreciousMetalProcessingElapsedTimeMinute) == 11998);
		static_assert(offsetof(ScriptVars_tpp, mbmFuelResourceProcessingElapsedTimeMinute) == 11999);
		static_assert(offsetof(ScriptVars_tpp, mbmBioticResourceProcessingElapsedTimeMinute) == 12000);
		static_assert(offsetof(ScriptVars_tpp, isRussianTranslatable) == 12001);
		static_assert(offsetof(ScriptVars_tpp, isPashtoTranslatable) == 12002);
		static_assert(offsetof(ScriptVars_tpp, isAfrikaansTranslatable) == 12003);
		static_assert(offsetof(ScriptVars_tpp, isKikongoTranslatable) == 12004);
		static_assert(offsetof(ScriptVars_tpp, mbmTreatmentElapsedTimeMinute) == 12005);
		static_assert(offsetof(ScriptVars_tpp, mbmBecomeCombatSectionLvMax) == 12006);
		static_assert(offsetof(ScriptVars_tpp, mbmBecomeDevelopSectionLvMax) == 12007);
		static_assert(offsetof(ScriptVars_tpp, mbmBecomeBaseDevSectionLvMax) == 12008);
		static_assert(offsetof(ScriptVars_tpp, mbmBecomeSupportSectionLvMax) == 12009);
		static_assert(offsetof(ScriptVars_tpp, mbmBecomeSpySectionLvMax) == 12010);
		static_assert(offsetof(ScriptVars_tpp, mbmBecomeMedicalSectionLvMax) == 12011);
		static_assert(offsetof(ScriptVars_tpp, mbmBecomeSecuritySectionLvMax) == 12012);
		static_assert(offsetof(ScriptVars_tpp, mbmBecomeAllSectionLvMax) == 12013);
		static_assert(offsetof(ScriptVars_tpp, mbmHighestTotalSectionLv) == 12014);
		static_assert(offsetof(ScriptVars_tpp, mbmStaffSyncVersion) == 12016);
		static_assert(offsetof(ScriptVars_tpp, mbmStaffSvarsHeaders) == 12020);
		static_assert(offsetof(ScriptVars_tpp, mbmStaffSvarsSeeds) == 26020);
		static_assert(offsetof(ScriptVars_tpp, mbmStaffSvarsStatusesSync) == 40020);
		static_assert(offsetof(ScriptVars_tpp, mbmStaffSvarsStatusesNoSync) == 54020);
		static_assert(offsetof(ScriptVars_tpp, mbmStaffSkillGots) == 61020);
		static_assert(offsetof(ScriptVars_tpp, mbmStaffSortKeysSvars) == 61107);
		static_assert(offsetof(ScriptVars_tpp, mbmStaffSortDirsSvars) == 61120);
		static_assert(offsetof(ScriptVars_tpp, mbmStaffListMainListModeSvars) == 61133);
		static_assert(offsetof(ScriptVars_tpp, mbmStaffListAssignListModeSvars) == 61134);
		static_assert(offsetof(ScriptVars_tpp, mbmEnmityElapsedTimeMinuteSvars) == 61135);
		static_assert(offsetof(ScriptVars_tpp, mbmMoraleElapsedTimeMinuteSvars) == 61136);
		static_assert(offsetof(ScriptVars_tpp, mbmLanguageElapsedTimeMinuteSvars) == 61137);
		static_assert(offsetof(ScriptVars_tpp, mbmEnableStaffInitLangKikongo) == 61138);
		static_assert(offsetof(ScriptVars_tpp, mbmPandemicEventMode) == 61139);
		static_assert(offsetof(ScriptVars_tpp, mbmPandemicElapsedTimeMinuteSvars) == 61140);
		static_assert(offsetof(ScriptVars_tpp, mbmPandemicSpreadAttackLimitCount) == 61142);
		static_assert(offsetof(ScriptVars_tpp, mbmPandemicSpreadAttackCount) == 61144);
		static_assert(offsetof(ScriptVars_tpp, mbmDisableKikongoFirst) == 61146);
		static_assert(offsetof(ScriptVars_tpp, mbmMasterGunsmithSkill) == 61147);
		static_assert(offsetof(ScriptVars_tpp, mbmBipedalismWeaponDevelopmentSkill) == 61148);
		static_assert(offsetof(ScriptVars_tpp, mbmBanHeuy) == 61149);
		static_assert(offsetof(ScriptVars_tpp, mbmAssignableMiller) == 61150);
		static_assert(offsetof(ScriptVars_tpp, mbmLockedTanFlag) == 61151);
		static_assert(offsetof(ScriptVars_tpp, mbmStaffInitEnmityLv) == 61152);
		static_assert(offsetof(ScriptVars_tpp, mbmEquipParamStatus) == 61154);
		static_assert(offsetof(ScriptVars_tpp, mbmOnlineOffsetEquipParamStatus) == 62178);
		static_assert(offsetof(ScriptVars_tpp, mbmEquipDevelopIndices) == 63202);
		static_assert(offsetof(ScriptVars_tpp, mbmEquipDevelopTimes) == 63224);
		static_assert(offsetof(ScriptVars_tpp, mbmWeaponPartsReceiverStatus) == 63264);
		static_assert(offsetof(ScriptVars_tpp, mbmWeaponPartsBarrelStatus) == 63498);
		static_assert(offsetof(ScriptVars_tpp, mbmWeaponPartsMagazineStatus) == 63613);
		static_assert(offsetof(ScriptVars_tpp, mbmWeaponPartsStockStatus) == 63805);
		static_assert(offsetof(ScriptVars_tpp, mbmWeaponPartsMuzzleStatus) == 63848);
		static_assert(offsetof(ScriptVars_tpp, mbmWeaponPartsMuzzleOptionStatus) == 63877);
		static_assert(offsetof(ScriptVars_tpp, mbmWeaponPartsSightStatus) == 63917);
		static_assert(offsetof(ScriptVars_tpp, mbmWeaponPartsUnderBarrelStatus) == 63942);
		static_assert(offsetof(ScriptVars_tpp, mbmWeaponPartsLightStatus) == 63965);
		static_assert(offsetof(ScriptVars_tpp, mbmServerDevelopCount) == 63976);
		static_assert(offsetof(ScriptVars_tpp, mbmServerDevelopIds) == 63978);
		static_assert(offsetof(ScriptVars_tpp, mbmServerDevelopUsageGmpCosts) == 64618);
		static_assert(offsetof(ScriptVars_tpp, mbmServerDevelopUsageResourceType1s) == 65258);
		static_assert(offsetof(ScriptVars_tpp, mbmServerDevelopUsageResourceType1Counts) == 65578);
		static_assert(offsetof(ScriptVars_tpp, mbmServerDevelopUsageResourceType2s) == 66218);
		static_assert(offsetof(ScriptVars_tpp, mbmServerDevelopUsageResourceType2Counts) == 66538);
		static_assert(offsetof(ScriptVars_tpp, mbmFobSvars) == 67180);
		static_assert(offsetof(ScriptVars_tpp, mbmClusterSvars) == 67196);
		static_assert(offsetof(ScriptVars_tpp, mbmClusterTimeMinutes) == 67268);
		static_assert(offsetof(ScriptVars_tpp, mbmCompMb) == 67408);
		static_assert(offsetof(ScriptVars_tpp, doesNotRestoreCommandClusterGradeFromLayoutCode) == 67409);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityClusterSvars) == 67412);
		static_assert(offsetof(ScriptVars_tpp, mbmSecuritySpecialPlatformSoldierCounts) == 67552);
		static_assert(offsetof(ScriptVars_tpp, mbmSecuritySpecialPlatformIrSensorCounts) == 67587);
		static_assert(offsetof(ScriptVars_tpp, mbmSecuritySpecialPlatformAntitheftCounts) == 67622);
		static_assert(offsetof(ScriptVars_tpp, mbmSecuritySpecialPlatformCameraCounts) == 67657);
		static_assert(offsetof(ScriptVars_tpp, mbmSecuritySpecialPlatformDecoyCounts) == 67692);
		static_assert(offsetof(ScriptVars_tpp, mbmSecuritySpecialPlatformMineCounts) == 67727);
		static_assert(offsetof(ScriptVars_tpp, mbmSecuritySpecialPlatformUavCounts) == 67762);
		static_assert(offsetof(ScriptVars_tpp, mbmSecuritySpecialPlatformCautionAreaSvars) == 67800);
		static_assert(offsetof(ScriptVars_tpp, mbmSecuritySpecialPlatformPlacedFreeMineCounts) == 67940);
		static_assert(offsetof(ScriptVars_tpp, mbmSecuritySpecialPlatformPlacedFreeCameraCounts) == 67975);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform1SoldierCounts) == 68010);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform1IrSensorCounts) == 68045);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform1AntitheftCounts) == 68080);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform1CameraCounts) == 68115);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform1DecoyCounts) == 68150);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform1MineCounts) == 68185);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform1UavCounts) == 68220);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform1CautionAreaSvars) == 68256);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform1PlacedFreeMineCounts) == 68396);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform1PlacedFreeCameraCounts) == 68431);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform2SoldierCounts) == 68466);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform2IrSensorCounts) == 68501);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform2AntitheftCounts) == 68536);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform2CameraCounts) == 68571);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform2DecoyCounts) == 68606);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform2MineCounts) == 68641);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform2UavCounts) == 68676);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform2CautionAreaSvars) == 68712);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform2PlacedFreeMineCounts) == 68852);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform2PlacedFreeCameraCounts) == 68887);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform3SoldierCounts) == 68922);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform3IrSensorCounts) == 68957);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform3AntitheftCounts) == 68992);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform3CameraCounts) == 69027);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform3DecoyCounts) == 69062);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform3MineCounts) == 69097);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform3UavCounts) == 69132);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform3CautionAreaSvars) == 69168);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform3PlacedFreeMineCounts) == 69308);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform3PlacedFreeCameraCounts) == 69343);
		static_assert(offsetof(ScriptVars_tpp, mbmSecuritySpecialPlatformSvars) == 69380);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform1Svars) == 69520);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform2Svars) == 69660);
		static_assert(offsetof(ScriptVars_tpp, mbmSecurityCommonPlatform3Svars) == 69800);
		static_assert(offsetof(ScriptVars_tpp, mbmInitializedServerWalletFlagDay1) == 69940);
		static_assert(offsetof(ScriptVars_tpp, mbmIsInitializedDay50) == 69941);
		static_assert(offsetof(ScriptVars_tpp, mbmNuclearWeaponLocalCountPatchingDay50) == 69944);
		static_assert(offsetof(ScriptVars_tpp, mbmNuclearSynchronizedDay50) == 69948);
		static_assert(offsetof(ScriptVars_tpp, mbmResourceSyncVersion) == 69952);
		static_assert(offsetof(ScriptVars_tpp, mbmResourceSvars1Fix) == 69956);
		static_assert(offsetof(ScriptVars_tpp, mbmResourceSvars2Fix) == 70192);
		static_assert(offsetof(ScriptVars_tpp, mbmResourceSvars1Diff) == 70428);
		static_assert(offsetof(ScriptVars_tpp, mbmResourceSvars2Diff) == 70664);
		static_assert(offsetof(ScriptVars_tpp, mbmResourceCompensateCounts) == 70900);
		static_assert(offsetof(ScriptVars_tpp, mbmWhiteContainerCountsSvarsFix) == 71136);
		static_assert(offsetof(ScriptVars_tpp, mbmWhiteContainerCountsSvarsDiff) == 71156);
		static_assert(offsetof(ScriptVars_tpp, mbmRedContainerCountsSvarsFix) == 71176);
		static_assert(offsetof(ScriptVars_tpp, mbmRedContainerCountsSvarsDiff) == 71196);
		static_assert(offsetof(ScriptVars_tpp, mbmOpenedWalkerGear) == 71216);
		static_assert(offsetof(ScriptVars_tpp, mbmDataBaseSvars) == 71217);
		static_assert(offsetof(ScriptVars_tpp, mbmCompAnimal) == 71676);
		static_assert(offsetof(ScriptVars_tpp, mbmCompDesign) == 71677);
		static_assert(offsetof(ScriptVars_tpp, mbmCompPhoto) == 71678);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployMissionsSvars) == 71680);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployMissionAveRanksSvars) == 71964);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamCombatStaffCounts) == 72036);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamCombatStaffRankTops) == 72058);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamCombatStaffRankBottoms) == 72069);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSubStaffCounts) == 72080);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSubStaffRankTops) == 72102);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSubStaffRankBottoms) == 72113);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamCombatPoints) == 72124);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSubPoints) == 72168);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsTimeMinute) == 72212);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvars4wdEastCount) == 72256);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvars4wdWestCount) == 72278);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsTruckEastCount) == 72300);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsTruckWestCount) == 72322);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsArmoredVehicleEastCount) == 72344);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsArmoredVehicleWestCount) == 72366);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsArmoredVehicleEastRocketCount) == 72388);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsWheeledArmoredVehicleWestCount) == 72410);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsTankEastCount) == 72432);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsTankWestCount) == 72454);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsWalkerGearProtoHeuyCount) == 72476);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsWalkerGearSovietBattleCount) == 72498);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsWalkerGearSovietSupportCount) == 72520);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsWalkerGearCfaBattleCount) == 72542);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployTeamSvarsWalkerGearCfaSupportCount) == 72564);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployRandomMissionSeedSvars) == 72588);
		static_assert(offsetof(ScriptVars_tpp, mbmDeploySequentialMissionIdSvars) == 72592);
		static_assert(offsetof(ScriptVars_tpp, mbmDeploySequentialMissionIdLimitSvars) == 72594);
		static_assert(offsetof(ScriptVars_tpp, mbmClearedDeployRevengeMissionFlags) == 72596);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployableBattleGear) == 72610);
		static_assert(offsetof(ScriptVars_tpp, mbmBattleGearDeployingTeam) == 72611);
		static_assert(offsetof(ScriptVars_tpp, mbmAddDeployLogIndex) == 72612);
		static_assert(offsetof(ScriptVars_tpp, mbmCompSequential) == 72613);
		static_assert(offsetof(ScriptVars_tpp, mbmNewestNRandomMissionId) == 72614);
		static_assert(offsetof(ScriptVars_tpp, mbmNewestRRandomMissionId) == 72616);
		static_assert(offsetof(ScriptVars_tpp, mbmOpenedRRandomMission) == 72618);
		static_assert(offsetof(ScriptVars_tpp, mbmNRandomMissionClearedCount) == 72619);
		static_assert(offsetof(ScriptVars_tpp, mbmIsCancelAllDeploy) == 72620);
		static_assert(offsetof(ScriptVars_tpp, mbmClearedCountOfflineDeploy) == 72624);
		static_assert(offsetof(ScriptVars_tpp, mbmClearedCountOnlineDeploy) == 72628);
		static_assert(offsetof(ScriptVars_tpp, mbmPoolRewardsSyncVersion) == 72632);
		static_assert(offsetof(ScriptVars_tpp, mbmPoolRewardsSvars) == 72636);
		static_assert(offsetof(ScriptVars_tpp, mbmPoolRewardSourceIds) == 73436);
		static_assert(offsetof(ScriptVars_tpp, mbmPoolRewardSeeds) == 74236);
		static_assert(offsetof(ScriptVars_tpp, mbmPoolRewardValues) == 74636);
		static_assert(offsetof(ScriptVars_tpp, mbmS10081CaptiveStaffSvarsSeed) == 75436);
		static_assert(offsetof(ScriptVars_tpp, mbmS10081CaptiveStaffSvarsHeader) == 75440);
		static_assert(offsetof(ScriptVars_tpp, mbmS10081CaptiveStaffSvarsStatusSync) == 75444);
		static_assert(offsetof(ScriptVars_tpp, mbmS10081CaptiveStaffSvarsStatusNoSync) == 75448);
		static_assert(offsetof(ScriptVars_tpp, mbmRemoverSvarsHeaders) == 75452);
		static_assert(offsetof(ScriptVars_tpp, mbmRemoverSvarsSeeds) == 75708);
		static_assert(offsetof(ScriptVars_tpp, mbmRemoverSvarsStatusesSync) == 75964);
		static_assert(offsetof(ScriptVars_tpp, mbmRemoverSvarsStatusesNoSync) == 76220);
		static_assert(offsetof(ScriptVars_tpp, mbmRemoverReasons) == 76348);
		static_assert(offsetof(ScriptVars_tpp, mbmAddRemoverIndexSvars) == 76412);
		static_assert(offsetof(ScriptVars_tpp, mbmDeployRewardSyncVersion) == 76416);
		static_assert(offsetof(ScriptVars_tpp, mbmSecuritySystemLevelsDeveloped) == 76420);
		static_assert(offsetof(ScriptVars_tpp, mbmAnimalHabitatAfghSvars) == 76436);
		static_assert(offsetof(ScriptVars_tpp, mbmAnimalHabitatMafrSvars) == 76500);
		static_assert(offsetof(ScriptVars_tpp, mbmIsOccupation) == 76564);
		static_assert(offsetof(ScriptVars_tpp, mbmRequestDemoAttackedFromOtherPlayer) == 76565);
		static_assert(offsetof(ScriptVars_tpp, mbmDemoAttackedFromOtherPlayerKnowWhereFrom) == 76566);
		static_assert(offsetof(ScriptVars_tpp, mbmFobDefenseWinCount) == 76568);
		static_assert(offsetof(ScriptVars_tpp, mbmFobDefenseLoseCount) == 76572);
		static_assert(offsetof(ScriptVars_tpp, mbmIsContractOutDisplayed) == 76576);
		static_assert(offsetof(ScriptVars_tpp, mbmIsEnableNuclearDevelop) == 76577);
		static_assert(offsetof(ScriptVars_tpp, mbmIsNuclearDeveloped) == 76578);
		static_assert(offsetof(ScriptVars_tpp, mbmIsNuclearDiscarded) == 76579);
		static_assert(offsetof(ScriptVars_tpp, mbmIsNuclearDeveloping) == 76580);
		static_assert(offsetof(ScriptVars_tpp, mbmNuclearDevelopTime) == 76584);
		static_assert(offsetof(ScriptVars_tpp, mbmNuclearDevelopMaxTime) == 76588);
		static_assert(offsetof(ScriptVars_tpp, mbmNuclearDevelopStartTime) == 76592);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalAddedHostageStaffCount) == 76596);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalAddedSoldierStaffCount) == 76600);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalAddedVolunteerStaffCount) == 76604);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalDeadStaffCount) == 76608);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalDefectionCount) == 76612);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalFireCount) == 76616);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalAddedGmpT) == 76620);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalAddedGmp) == 76624);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalSubtractedGmpT) == 76628);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalSubtractedGmp) == 76632);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalAddedHeroicPoint) == 76636);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalSubtractedHeroicPoint) == 76640);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalAddedResourceCounts) == 76644);
		static_assert(offsetof(ScriptVars_tpp, mbmPdTotalAddedAnimalCounts) == 76880);
		static_assert(offsetof(ScriptVars_tpp, mbmCurrentNamePlateId) == 77276);
		static_assert(offsetof(ScriptVars_tpp, mbmNamePlateGotFlags) == 77280);
		static_assert(offsetof(ScriptVars_tpp, mbmNamePlateNewFlags) == 77288);
		static_assert(offsetof(ScriptVars_tpp, mbmHeroThreshold) == 77296);
		static_assert(offsetof(ScriptVars_tpp, mbmNotHeroThreshold) == 77300);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingSaveSeason) == 77304);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingReadAttackMatchFlag) == 77306);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingReadDefenseMatchFlag) == 77308);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingMatchSections) == 77310);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingRankingSaveSection) == 77325);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingRankingPlayerRanks) == 77326);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingRankingPlayerRankDiffs) == 77342);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingRankingPlayerIds) == 77360);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingCommonFlag) == 77424);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingShortSaveSeason) == 77426);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingShortReadAttackMatchFlag) == 77428);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingShortReadDefenseMatchFlag) == 77430);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingShortMatchSections) == 77432);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingShortRankingSaveSection) == 77447);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingShortRankingPlayerRanks) == 77448);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingShortRankingPlayerRankDiffs) == 77464);
		static_assert(offsetof(ScriptVars_tpp, mbmPfRatingShortRankingPlayerIds) == 77480);
		static_assert(offsetof(ScriptVars_tpp, mbmChallengeTaskCompleteFlags) == 77544);
		static_assert(offsetof(ScriptVars_tpp, mbmChallengeTaskRewardReceiveFlags) == 77604);
		static_assert(offsetof(ScriptVars_tpp, mbmChallengeTaskOnlineVersionWindowOpen) == 77664);
		static_assert(offsetof(ScriptVars_tpp, mbmServerRankingRecordedFlags) == 77668);
		static_assert(offsetof(ScriptVars_tpp, mbmServerRankingScores) == 77776);
		static_assert(offsetof(ScriptVars_tpp, mbmBaseColorGotFlag) == 78200);
		static_assert(offsetof(ScriptVars_tpp, mbmBaseColorNewFlag) == 78204);
		static_assert(offsetof(ScriptVars_tpp, mbmDlcWeaponColorGotFlag) == 78208);
		static_assert(offsetof(ScriptVars_tpp, mbmDlcWeaponColorGotFlag2) == 78212);
		static_assert(offsetof(ScriptVars_tpp, mbmDlcWeaponColorNewFlag) == 78216);
		static_assert(offsetof(ScriptVars_tpp, mbmDlcWeaponColorNewFlag2) == 78220);
		static_assert(offsetof(ScriptVars_tpp, mbmIsVisitedFobDeployWelcomeMessage1) == 78224);
		static_assert(offsetof(ScriptVars_tpp, mbmIsVisitedFobDeployWelcomeMessage2) == 78225);
		static_assert(offsetof(ScriptVars_tpp, mbmFobDeployCheckBoxes) == 78226);
		static_assert(offsetof(ScriptVars_tpp, mbmFobDeployGradeSelectorIndexes) == 78232);
		static_assert(offsetof(ScriptVars_tpp, optionSelectedIndices) == 78238);
		static_assert(offsetof(ScriptVars_tpp, armRocketYIsReverse) == 78270);
		static_assert(offsetof(ScriptVars_tpp, armRocketXIsReverse) == 78271);
		static_assert(offsetof(ScriptVars_tpp, bgmVolume) == 78272);
		static_assert(offsetof(ScriptVars_tpp, keyStr) == 78276);
		static_assert(offsetof(ScriptVars_tpp, buttonNo) == 78756);
		static_assert(offsetof(ScriptVars_tpp, keybordNo) == 79236);
		static_assert(offsetof(ScriptVars_tpp, pushToTalk) == 79716);
		static_assert(offsetof(ScriptVars_tpp, checkPointGimmickIndelibleCount) == 79717);
		static_assert(offsetof(ScriptVars_tpp, checkPointGimmickIndelibleName) == 79720);
		static_assert(offsetof(ScriptVars_tpp, checkPointGimmickIndelibleState) == 79976);
		static_assert(offsetof(ScriptVars_tpp, missionStartGimmickIndelibleCount) == 80040);
		static_assert(offsetof(ScriptVars_tpp, missionStartGimmickIndelibleName) == 80044);
		static_assert(offsetof(ScriptVars_tpp, missionStartGimmickIndelibleState) == 80300);
		static_assert(offsetof(ScriptVars_tpp, checkPointGimmickPermanentGimmickSaveData) == 80364);
		static_assert(offsetof(ScriptVars_tpp, missionStartGimmickPermanentGimmickSaveData) == 82956);
		static_assert(offsetof(ScriptVars_tpp, cloakEnabled) == 85549);
		static_assert(offsetof(ScriptVars_tpp, supCboxSaveFormVersion) == 85551);
		static_assert(offsetof(ScriptVars_tpp, supCboxPhaseForSave) == 85552);
		static_assert(offsetof(ScriptVars_tpp, supCboxDropType) == 85553);
		static_assert(offsetof(ScriptVars_tpp, supCboxEquipId) == 85556);
		static_assert(offsetof(ScriptVars_tpp, supCboxPartsType) == 85560);
		static_assert(offsetof(ScriptVars_tpp, supCboxPos) == 85564);
		static_assert(offsetof(ScriptVars_tpp, supCboxRot) == 85576);
		static_assert(offsetof(ScriptVars_tpp, supCboxLevelWait) == 85592);
		static_assert(offsetof(ScriptVars_tpp, supCboxAdvancedRate) == 85596);
		static_assert(offsetof(ScriptVars_tpp, supCboxFallHeight) == 85600);
		static_assert(offsetof(ScriptVars_tpp, supCboxLevels) == 85604);
		static_assert(offsetof(ScriptVars_tpp, supCboxFlags) == 85617);
		static_assert(offsetof(ScriptVars_tpp, supCboxU8buf) == 85618);
		static_assert(offsetof(ScriptVars_tpp, supCboxU16buf) == 85734);
		static_assert(offsetof(ScriptVars_tpp, supCboxU32buf) == 85808);
		static_assert(offsetof(ScriptVars_tpp, supCboxLocationOnSave) == 85876);
		static_assert(offsetof(ScriptVars_tpp, supAttackStateForSave) == 85878);
		static_assert(offsetof(ScriptVars_tpp, supAttackAttackType) == 85879);
		static_assert(offsetof(ScriptVars_tpp, supAttackPos) == 85880);
		static_assert(offsetof(ScriptVars_tpp, supAttackRot) == 85892);
		static_assert(offsetof(ScriptVars_tpp, supAttackLevelWait) == 85908);
		static_assert(offsetof(ScriptVars_tpp, supAttackLevelAttack) == 85909);
		static_assert(offsetof(ScriptVars_tpp, supAttackFlags) == 85910);
		static_assert(offsetof(ScriptVars_tpp, supAttackCommonValue) == 85912);
		static_assert(offsetof(ScriptVars_tpp, supAttackWaitTime) == 85916);
		static_assert(offsetof(ScriptVars_tpp, supAttackLocationOnSave) == 85920);
		static_assert(offsetof(ScriptVars_tpp, stealthAssistLeftCount) == 85922);
		static_assert(offsetof(ScriptVars_tpp, tppTransferFlag) == 85924);
		static_assert(offsetof(ScriptVars_tpp, tppTransferAcquiredFlag) == 85928);
		static_assert(offsetof(ScriptVars_tpp, dlcPermittedFlag) == 85932);
		static_assert(offsetof(ScriptVars_tpp, dlcAcquiredFlag) == 85940);
		static_assert(offsetof(ScriptVars_tpp, mbDvcTutorialFlag) == 85948);
		static_assert(offsetof(ScriptVars_tpp, mbDvcTopMenuFlag) == 85952);
		static_assert(offsetof(ScriptVars_tpp, heliMusicType) == 85956);
		static_assert(offsetof(ScriptVars_tpp, heliMusicTrackID) == 85960);
		static_assert(offsetof(ScriptVars_tpp, radioReadFlagGlobalScoped) == 85964);
		static_assert(offsetof(ScriptVars_tpp, markerLocatorInfos) == 86764);
		static_assert(offsetof(ScriptVars_tpp, tipsReadFlag) == 87164);
		static_assert(offsetof(ScriptVars_tpp, userMarkerPosX) == 87364);
		static_assert(offsetof(ScriptVars_tpp, userMarkerPosY) == 87384);
		static_assert(offsetof(ScriptVars_tpp, userMarkerPosZ) == 87404);
		static_assert(offsetof(ScriptVars_tpp, userMarkerAddFlag) == 87424);
		static_assert(offsetof(ScriptVars_tpp, userMarkerGameObjId) == 87434);
		static_assert(offsetof(ScriptVars_tpp, userMarkerLocationId) == 87444);
		static_assert(offsetof(ScriptVars_tpp, userMarkerSaveCount) == 87446);
		static_assert(offsetof(ScriptVars_tpp, fobResultRankingDateTime) == 87448);
		static_assert(offsetof(ScriptVars_tpp, fobResultRankingPrevRanks) == 87452);
		static_assert(offsetof(ScriptVars_tpp, fobEventResultRankingDateTime) == 87468);
		static_assert(offsetof(ScriptVars_tpp, fobEventResultRankingPrevRanks) == 87472);

		union ScriptVars
		{
			ScriptVars_tpp tpp;
			ScriptVars_mgo mgo;
		};
	}

	namespace tpp::gm::player
	{
		enum PlayerSlotType : std::uint32_t
		{
			PRIMARY_1,
			PRIMARY_2,
			SECONDARY,
			SUPPORT,
			ITEM,
			STOLE,
			HAND,
		};

		namespace impl
		{
			struct EquipControllerImpl_tpp;

			struct EquipControllerImpl_tpp
			{
				struct vtable
				{
					void(__fastcall* InitializePlayerAtIndex)(tpp::gm::player::impl::EquipControllerImpl_tpp*);
					void(__fastcall* SetupWeapon)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int, int, unsigned __int8, unsigned int, unsigned int, bool);
					void(__fastcall* SetCurrentSlot)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, PlayerSlotType, unsigned __int8, char, char, int);
					void(__fastcall* SetSlotDirectly)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, unsigned int, int, int, bool, bool);
					void* (__fastcall* UnsetSlot)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, unsigned int);
					void* (__fastcall* UnsetSlots)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
					int* (__fastcall* GetCurrentWeapon)(tpp::gm::player::impl::EquipControllerImpl_tpp*, int*, unsigned int);
					unsigned __int8(__fastcall* GetCurrentEquipSlot)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
					void* (__fastcall* GetCurrentActiveWeaponSlot)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
					void* (__fastcall* SetCurrentActiveWeaponSlot)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int);
					unsigned __int8(__fastcall* GetCurrentSupportIndex)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
					unsigned __int8(__fastcall* GetCurrentItemIndex)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
					void* (__fastcall* GetLastItemIndex)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
					void* (__fastcall* SetCurrentItemIndex)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, unsigned int, bool);
					void* (__fastcall* SetCurrentItemIdToEquipHud)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, unsigned int);
					void* (__fastcall* SetOriginalOwner)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int, int);
					void* (__fastcall* GetOriginalOwner)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int);
					void* (__fastcall* GetCurrentEquipId)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
					unsigned int (__fastcall* GetEquipIdSlot)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int, unsigned int);
					void* (__fastcall* SetEquipState)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int, unsigned int, int, unsigned __int8, bool, unsigned __int8);
					void* (__fastcall* SetSlot)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int, int, int, bool, bool);
					void* (__fastcall* SetAmmoCount)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int, unsigned int, bool);
					void* (__fastcall* GetAmmoStockIndexBySlot)(tpp::gm::player::impl::EquipControllerImpl_tpp*, int, unsigned int);
					void* (__fastcall* SetAmmoStockCountWithMax)(tpp::gm::player::impl::EquipControllerImpl_tpp*, int, int, unsigned int, unsigned int);
					void* (__fastcall* SetAmmoStockCount)(tpp::gm::player::impl::EquipControllerImpl_tpp*, int, int, unsigned int);
					void* (__fastcall* GetItemStockCount)(tpp::gm::player::impl::EquipControllerImpl_tpp*, int);
					void* (__fastcall* IncrementItemStockCount)(tpp::gm::player::impl::EquipControllerImpl_tpp*, int, unsigned int);
					void* (__fastcall* DecrementItemStockCount)(tpp::gm::player::impl::EquipControllerImpl_tpp*, int, unsigned int);
					void* (__fastcall* SetBatteryByStockCount)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned short, int, int, int, int, unsigned __int8, bool);
					void* (__fastcall* SetBattery)(tpp::gm::player::impl::EquipControllerImpl_tpp*, float, int, int, int, int, unsigned __int8, bool);
					void* (__fastcall* UpdateBattery)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
					void* (__fastcall* RemoveFromAmmoStock)(tpp::gm::player::impl::EquipControllerImpl_tpp*, int);
					void* (__fastcall* CalcAmmoAndStockIfPickUp)(tpp::gm::player::impl::EquipControllerImpl_tpp*, int, unsigned short, unsigned int, int&, int&, int&);
					void* (__fastcall* SetCurrentWeaponToEquipHud)(tpp::gm::player::impl::EquipControllerImpl_tpp*);
					void* (__fastcall* SetWeaponToEquipHud)(tpp::gm::player::impl::EquipControllerImpl_tpp*, int, unsigned __int8, int);
					void* (__fastcall* ConvertPlayerIndexToAccessType)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
					void* (__fastcall* UpdateAmmoToEquipHud)(tpp::gm::player::impl::EquipControllerImpl_tpp*);
					void* (__fastcall* CreateSupplyAmmoBox)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, void*, unsigned int, unsigned __int8*);
					void* (__fastcall* GetItemLevel)(tpp::gm::player::impl::EquipControllerImpl_tpp*, int);
					void* (__fastcall* SetItemLevel)(tpp::gm::player::impl::EquipControllerImpl_tpp*, int, unsigned int);
					void* (__fastcall* GetItemLevelDeveloped)(tpp::gm::player::impl::EquipControllerImpl_tpp*, int);
					void* (__fastcall* SetSuppressorLife)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int, unsigned __int8);
					void* (__fastcall* SetUseSuppressor)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int, bool, bool);
					void* (__fastcall* SetUseFlashLight)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int, bool, bool);
					void* (__fastcall* SetUnderBarrelMode)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int, bool);
					void* (__fastcall* SetMagAttachTo2nd)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int, bool);
					void* (__fastcall* IsCurrenWeaponNoUse)(tpp::gm::player::impl::EquipControllerImpl_tpp*);
					void* (__fastcall* UpdateTextureStreamer)(tpp::gm::player::impl::EquipControllerImpl_tpp*);
					void* (__fastcall* IsLargeTextureLoaded)(tpp::gm::player::impl::EquipControllerImpl_tpp*);
					void* (__fastcall* SetVisibilityToEquip)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, void*);
					void* (__fastcall* ChangeEquip)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, void*);
					void* (__fastcall* UnsetEquip)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, void*);
					void* (__fastcall* SetupWeaponToTemporarySlot)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, unsigned int, void*, int);
					void* (__fastcall* UnsetTemporaryWeapon)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, bool);
					void* (__fastcall* RestoreWeaponSlotFromTemporarySlot)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, unsigned int, void*, int);
					void* (__fastcall* GetAlignment)(tpp::gm::player::impl::EquipControllerImpl_tpp*);
					void* (__fastcall* ForceRemoveItem)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
					void* (__fastcall* ForceRemoveSpecialItem)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, bool);
					void* (__fastcall* ForceRemoveCBox)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
					void* (__fastcall* ForceRemoveTimeCigarette)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
					void* (__fastcall* NotifyShieldDamage)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, void*);
					void* (__fastcall* SetupHandSlot)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int, bool, bool, bool);
					void* (__fastcall* SetHandSlotEnabled)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, bool);
					void* (__fastcall* DropShield)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
					void* (__fastcall* SendMessageEquipHudClosed)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
				};

				vtable* __vftable;
				char __pad0[240];
			};

			struct EquipControllerImpl_mgo;

			struct EquipControllerImpl_mgo
			{
				struct vtable
				{
					void(__fastcall* InitializePlayerAtIndex)(tpp::gm::player::impl::EquipControllerImpl_mgo*);
					void(__fastcall* SetupWeapon)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int, int, unsigned __int8, unsigned int, unsigned int, bool);
					void(__fastcall* SetCurrentSlot)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, PlayerSlotType, unsigned __int8, char, char, int);
					void* pad[4];
					void(__fastcall* SetSlotDirectly)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, unsigned int, int, int, bool, bool);
					void* (__fastcall* UnsetSlot)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, unsigned int);
					void* (__fastcall* UnsetSlots)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
					int* (__fastcall* GetCurrentWeapon)(tpp::gm::player::impl::EquipControllerImpl_mgo*, int*, unsigned int);
					unsigned __int8(__fastcall* GetCurrentEquipSlot)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
					void* (__fastcall* GetCurrentActiveWeaponSlot)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
					void* (__fastcall* SetCurrentActiveWeaponSlot)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int);
					unsigned __int8(__fastcall* GetCurrentSupportIndex)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
					unsigned __int8(__fastcall* GetCurrentItemIndex)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
					void* (__fastcall* GetLastItemIndex)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
					void* (__fastcall* SetCurrentItemIndex)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, unsigned int, bool);
					void* (__fastcall* SetCurrentItemIdToEquipHud)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, unsigned int);
					void* (__fastcall* SetOriginalOwner)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int, int);
					void* (__fastcall* GetOriginalOwner)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int);
					void* (__fastcall* GetCurrentEquipId)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
					void* pad2[5];
					unsigned int (__fastcall* GetEquipIdSlot)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int, unsigned int);
					void* (__fastcall* SetEquipState)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int, unsigned int, int, unsigned __int8, bool, unsigned __int8);
					void* (__fastcall* SetSlot)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int, int, int, bool, bool);
					void* (__fastcall* SetAmmoCount)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int, unsigned int, bool);
					void* (__fastcall* GetAmmoStockIndexBySlot)(tpp::gm::player::impl::EquipControllerImpl_mgo*, int, unsigned int);
					void* (__fastcall* SetAmmoStockCountWithMax)(tpp::gm::player::impl::EquipControllerImpl_mgo*, int, int, unsigned int, unsigned int);
					void* (__fastcall* SetAmmoStockCount)(tpp::gm::player::impl::EquipControllerImpl_mgo*, int, int, unsigned int);
					void* (__fastcall* GetItemStockCount)(tpp::gm::player::impl::EquipControllerImpl_mgo*, int);
					void* (__fastcall* IncrementItemStockCount)(tpp::gm::player::impl::EquipControllerImpl_mgo*, int, unsigned int);
					void* (__fastcall* DecrementItemStockCount)(tpp::gm::player::impl::EquipControllerImpl_mgo*, int, unsigned int);
					void* (__fastcall* SetBatteryByStockCount)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned short, int, int, int, int, unsigned __int8, bool);
					void* (__fastcall* SetBattery)(tpp::gm::player::impl::EquipControllerImpl_mgo*, float, int, int, int, int, unsigned __int8, bool);
					void* (__fastcall* UpdateBattery)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
					void* (__fastcall* RemoveFromAmmoStock)(tpp::gm::player::impl::EquipControllerImpl_mgo*, int);
					void* (__fastcall* CalcAmmoAndStockIfPickUp)(tpp::gm::player::impl::EquipControllerImpl_mgo*, int, unsigned short, unsigned int, int&, int&, int&);
					void* (__fastcall* SetCurrentWeaponToEquipHud)(tpp::gm::player::impl::EquipControllerImpl_mgo*);
					void* (__fastcall* SetWeaponToEquipHud)(tpp::gm::player::impl::EquipControllerImpl_mgo*, int, unsigned __int8, int);
					void* (__fastcall* ConvertPlayerIndexToAccessType)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
					void* (__fastcall* UpdateAmmoToEquipHud)(tpp::gm::player::impl::EquipControllerImpl_mgo*);
					void* (__fastcall* CreateSupplyAmmoBox)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, void*, unsigned int, unsigned __int8*);
					void* (__fastcall* GetItemLevel)(tpp::gm::player::impl::EquipControllerImpl_mgo*, int);
					void* (__fastcall* SetItemLevel)(tpp::gm::player::impl::EquipControllerImpl_mgo*, int, unsigned int);
					void* (__fastcall* GetItemLevelDeveloped)(tpp::gm::player::impl::EquipControllerImpl_mgo*, int);
					void* (__fastcall* SetSuppressorLife)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int, unsigned __int8);
					void* (__fastcall* SetUseSuppressor)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int, bool, bool);
					void* (__fastcall* SetUseFlashLight)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int, bool, bool);
					void* (__fastcall* SetUnderBarrelMode)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int, bool);
					void* (__fastcall* SetMagAttachTo2nd)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int, bool);
					void* (__fastcall* IsCurrenWeaponNoUse)(tpp::gm::player::impl::EquipControllerImpl_mgo*);
					void* (__fastcall* UpdateTextureStreamer)(tpp::gm::player::impl::EquipControllerImpl_mgo*);
					void* (__fastcall* IsLargeTextureLoaded)(tpp::gm::player::impl::EquipControllerImpl_mgo*);
					void* (__fastcall* SetVisibilityToEquip)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, void*);
					void* (__fastcall* ChangeEquip)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, void*);
					void* (__fastcall* UnsetEquip)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, void*);
					void* (__fastcall* SetupWeaponToTemporarySlot)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, unsigned int, void*, int);
					void* (__fastcall* UnsetTemporaryWeapon)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, bool);
					void* (__fastcall* RestoreWeaponSlotFromTemporarySlot)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, unsigned int, void*, int);
					void* (__fastcall* GetAlignment)(tpp::gm::player::impl::EquipControllerImpl_mgo*);
					void* (__fastcall* ForceRemoveItem)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
					void* (__fastcall* ForceRemoveSpecialItem)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, bool);
					void* (__fastcall* ForceRemoveCBox)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
					void* (__fastcall* ForceRemoveTimeCigarette)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
					void* (__fastcall* NotifyShieldDamage)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, void*);
					void* (__fastcall* SetupHandSlot)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int, bool, bool, bool);
					void* (__fastcall* SetHandSlotEnabled)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, bool);
					void* (__fastcall* DropShield)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
					void* (__fastcall* SendMessageEquipHudClosed)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
				};

				vtable* __vftable;
				char __pad0[248];
			};

			union EquipControllerImpl
			{
				EquipControllerImpl_tpp tpp;
				EquipControllerImpl_mgo mgo;
			};

			struct PadImpl
			{
				struct ButtonMask
				{
					unsigned int id;
					unsigned int mask;
				};

				struct vtable
				{

				};

				vtable* __vftable;
				char __pad0[32];
				unsigned int mask;
				char __pad1[660];
				ButtonMask buttonMasks[8];
				char __pad2[432];
			};

			static_assert(offsetof(PadImpl, buttonMasks) == 704);

			struct Player2SystemImpl_tpp
			{
				struct vtable
				{

				};

				vtable* __vftable;
				char __pad0[168];
				tpp::gm::player::impl::PadImpl* pad;
				char __pad1[16];
				tpp::gm::player::impl::EquipControllerImpl_tpp* controller;
				char __pad2[324];
				unsigned int localPlayerIndex;
			};

			static_assert(offsetof(Player2SystemImpl_tpp, controller) == 200);

			struct Player2SystemImpl_mgo
			{
				struct vtable
				{

				};

				vtable* __vftable;
				char __pad0[168];
				tpp::gm::player::impl::PadImpl* pad;
				char __pad1[16];
				tpp::gm::player::impl::EquipControllerImpl_mgo* controller;
				char __pad2[404];
				unsigned int localPlayerIndex;
			};

			static_assert(offsetof(Player2SystemImpl_mgo, controller) == 200);

			union Player2SystemImpl
			{
				Player2SystemImpl_tpp tpp;
				Player2SystemImpl_mgo mgo;
			};

			struct PlayerCameraImpl_mgo
			{
				char __pad0[716];
				float fov;
			};

			struct PlayerCameraImpl_tpp
			{
				char __pad0[732];
				float fov;
			};

			union PlayerCameraImpl
			{
				PlayerCameraImpl_tpp tpp;
				PlayerCameraImpl_mgo mgo;
			};

		}

		struct Pad
		{
			fox::Quark quark;
			tpp::gm::player::impl::PadImpl pad;
		};

		struct EquipController
		{
			fox::Quark quark;
			tpp::gm::player::impl::EquipControllerImpl controller;
		};

		struct Player2System
		{
			struct vtable
			{

			};

			vtable* __vftable;
			tpp::gm::player::impl::Player2SystemImpl* player2System;
		};
	}

	namespace fox::uix::impl
	{
		struct UixUtilityImpl;

		struct UixUtilityImpl_vtbl_mgo
		{
			void* (__fastcall* __constructor)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* __empty1)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* __empty2)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* GetButtonNumber)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, bool);
			void* (__fastcall* GetActivePadNo)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* IsButtonPress)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsButtonPressByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsButtonPressing)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsButtonPressingByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsButtonRelease)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsButtonReleaseByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsButtonStandby)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsButtonStandbyByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsButtonRepeat)(fox::uix::impl::UixUtilityImpl* this_, unsigned int, float, float);
			void* (__fastcall* IsButtonRepeatByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, float, float);
			void* (__fastcall* IsButtonLongPress)(fox::uix::impl::UixUtilityImpl* this_, unsigned int, float);
			void* (__fastcall* IsButtonLongPressByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, float);
			void* (__fastcall* GetButtonLongPressResultByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, float);
			void* (__fastcall* GetAnalogStickAxis)(fox::uix::impl::UixUtilityImpl* this_, unsigned int, float*, float*);
			void* (__fastcall* SetDisablePad)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* ResetDisablePad)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsDisablePad)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsUseKeyboard)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* SetUseKeyboard)(fox::uix::impl::UixUtilityImpl* this_, bool);
			void* (__fastcall* IsEnableKeyboard)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* GetKeyButtonPress)(fox::uix::impl::UixUtilityImpl* this_, fox::KeyCode);
			void* (__fastcall* GetInputString)(fox::uix::impl::UixUtilityImpl* this_, char*, int);
			void* (__fastcall* ClearInputString)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* ButtonNameToKeyCode)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, unsigned int*);
			void* (__fastcall* IsKeyboardPress)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsKeyboardPressByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsKeyboardPressing)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsKeyboardPressingByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsKeyboardRelease)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsKeyboardReleaseByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsKeyboardStandby)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsKeyboardStandbyByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsKeyboardRepeat)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsKeyboardRepeatByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsKeyboardLongPress)(fox::uix::impl::UixUtilityImpl* this_, unsigned int, float);
			void* (__fastcall* IsKeyboardLongPressByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, float);
			void* (__fastcall* GetKeyboardLongPressResultByName1)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, float);
			void* (__fastcall* GetMouseButtonsByKeyCode)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* GetMouseMoveXY)(fox::uix::impl::UixUtilityImpl* this_, float*, float*);
			void* (__fastcall* GetMouseAccelXY)(fox::uix::impl::UixUtilityImpl* this_, float*, float*);
			void* (__fastcall* GetMouseWheel)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* IsMouseButtonDown)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsMouseButtonDownByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsMouseButtonPressing)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsMouseButtonPressingByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsMouseButtonUp)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsMouseButtonUpByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* GetKeyboardLongPressResultByName2)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, float);
			void* (__fastcall* GetMotionSensorOrientation)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* GetMotionSensorAcceleration)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* GetMotionSensorAngularVelocity)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* IsMotionSensorAvailable)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* ResetMotionSensorOrientation)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* SendTrigger)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*, unsigned __int64, unsigned __int64);
			void* (__fastcall* SendTriggerToParent)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*, unsigned __int64, unsigned __int64);
			void* (__fastcall* IsTriggerReceived1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::TriggerPool const*, unsigned __int64, unsigned __int64*);
			void* (__fastcall* IsTriggerReceived2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::TriggerPool const*, unsigned __int64, bool);
			void* (__fastcall* GetTriggerPool)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* SendVisibleWindowMessage)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface*, bool);
			void* (__fastcall* SendConnectWindowMessage)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface*, fox::ui::Model*, fox::ui::ModelNode*);
			void* (__fastcall* GetChildWindowInterface)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*, fox::StringId);
			void* (__fastcall* GetLayout1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout const*, fox::StringId);
			void* (__fastcall* GetLayout2)(fox::uix::impl::UixUtilityImpl* this_, void* const, fox::StringId);
			void* (__fastcall* GetLayout3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*, fox::StringId);
			void* (__fastcall* IsHaveLayout)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout const*, fox::StringId);
			void* (__fastcall* GetModel)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout const*, fox::StringId);
			void* (__fastcall* GetAnimation1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model const*, fox::StringId);
			void* (__fastcall* GetAnimation2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout const*, fox::StringId);
			void* (__fastcall* IsHaveModelNodeCommon)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model const*, fox::StringId);
			void* (__fastcall* GetModelNodeCommon)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model const*, fox::StringId);
			void* (__fastcall* GetModelNodeText)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model const*, fox::StringId);
			void* (__fastcall* GetModelNodeMesh)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model const*, fox::StringId);
			void* (__fastcall* GetModelNodeLine)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model const*, fox::StringId);
			void* (__fastcall* GetOrthogonalProjectionScaleFromModelComponentCamera)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*);
			void* pad[7];
			void* (__fastcall* SetVisible1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, bool);
			void* (__fastcall* SetVisible2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, bool);
			void* (__fastcall* IsVisible1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			void* (__fastcall* IsVisible2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
			void (__fastcall* SetAlpha1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void (__fastcall* SetAlpha2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			float (__fastcall* GetAlpha1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			float (__fastcall* GetAlpha2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
			void* (__fastcall* SetColorRGB1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, fox::Rgba8);
			void* (__fastcall* SetColorRGB2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::Rgba8);
			void* (__fastcall* SetColorRGB3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, fox::Color*);
			void* (__fastcall* SetColorRGB4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::Color*);
			void* (__fastcall* SetColorRGB5)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float, float, float);
			void* (__fastcall* SetColorRGB6)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float, float, float);
			void* (__fastcall* SetColorGroup1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, unsigned int);
			void* (__fastcall* SetColorGroup2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, unsigned int);
			void* (__fastcall* GetColorGroup3)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* GetTranslation1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			void* (__fastcall* GetTranslation2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
			void* (__fastcall* SetTranslation1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetTranslation2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetTranslationX1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetTranslationX2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* SetTranslationY1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetTranslationY2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* SetTranslationZ1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetTranslationZ2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* GetRotation1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			void* (__fastcall* GetRotation2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
			void* (__fastcall* GetRotationDegree1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			void* (__fastcall* GetRotationDegree2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
			void* (__fastcall* SetRotation1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetRotation2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetRotationDegree1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetRotationDegree2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetRotationDegreeX1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetRotationDegreeX2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* SetRotationDegreeY1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetRotationDegreeY2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* SetRotationDegreeZ1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetRotationDegreeZ2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* SetQuaternion1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, Vectormath::Aos::Quat*);
			void* (__fastcall* SetQuaternion2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, Vectormath::Aos::Quat*);
			void* (__fastcall* SetScale1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetScale2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* SetScale3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetScale4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, Vectormath::Aos::Vector3*);
			void* (__fastcall* GetScale1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			void* (__fastcall* GetScale2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
			void* (__fastcall* SetStatus1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeLine*, unsigned int);
			void* (__fastcall* SetStatus2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, unsigned int);
			void* (__fastcall* SetStatus3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeLine*, unsigned int);
			void* (__fastcall* SetStatus4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*, unsigned int);
			void* (__fastcall* ResetStatus1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeLine*, unsigned int);
			void* (__fastcall* ResetStatus2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, unsigned int);
			void* (__fastcall* ResetStatus3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeLine*, unsigned int);
			void* (__fastcall* ResetStatus4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*, unsigned int);
			void* (__fastcall* SetVerticalAlign)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, char);
			void* (__fastcall* SetTextAlign)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, char);
			void* (__fastcall* SetFontSize)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, float, float);
			void* (__fastcall* GetLineWidth)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, unsigned int);
			void* (__fastcall* GetLineHeight)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, unsigned int);
			void* (__fastcall* SetLineWidth)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, unsigned int, float);
			void* (__fastcall* SetLineHeight)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, unsigned int, float);
			void* (__fastcall* ConnectComponent1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::ui::LayoutComponent*, unsigned int);
			void* (__fastcall* ConnectComponent2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::ui::LayoutComponent*, fox::StringId);
			void* (__fastcall* ConnectComponent3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::ui::LayoutComponent*, fox::ui::ModelNode const*);
			void* (__fastcall* GetTraversalTranslation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::ui::ModelNode*);
			void* (__fastcall* GetTraversalScale)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::ui::ModelNode*);
			void* (__fastcall* SetTextureName1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, unsigned int, unsigned int, bool, unsigned int, unsigned int, unsigned int);
			void* (__fastcall* SetTextureName2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::StringId, fox::StringId, bool, fox::StringId, fox::StringId, fox::StringId);
			void* (__fastcall* SetTextureName3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::Path const*, fox::StringId);
			void* (__fastcall* SetTextureName4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, char const*, fox::StringId, int);
			void* (__fastcall* SetTextureName5)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, char const*, fox::StringId);
			void* (__fastcall* SetTextureName6)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::PathId, fox::StringId, int);
			void* (__fastcall* SetTextureName7)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::PathId, fox::StringId);
			void* (__fastcall* CheckAndSetTextureName1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, char const*, fox::StringId, int);
			void* (__fastcall* CheckAndSetTextureName2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, char const*, fox::StringId);
			void* (__fastcall* CheckAndSetTextureName3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::PathId, fox::StringId, int);
			void* (__fastcall* CheckAndSetTextureName4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::PathId, fox::StringId);
			void* (__fastcall* SetGrTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, void*, unsigned int);
			void* (__fastcall* CreatePrefetchTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::PathId, int);
			void* (__fastcall* RemovePrefetchTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::PathId);
			void* (__fastcall* IsReadyPrefetchTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::PathId, int);
			void* (__fastcall* RegisterLoadingMeshTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::PathId);
			void* (__fastcall* UnregisterLoadingMeshTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::PathId);
			void* (__fastcall* SetLoadingTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::PathId, int);
			void* (__fastcall* IsDispLoadingTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, unsigned int);
			void* (__fastcall* IsTextureLoaded)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::StringId, int);
			void* (__fastcall* SetTextureLevel)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::StringId, int);
			void* (__fastcall* SetShaderParameter)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::StringId, Vectormath::Aos::Vector4*);
			void* (__fastcall* SetShaderTechnique)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::StringId);
			void* (__fastcall* SetShaderBaseTexUvShift)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, float, float);
			void* (__fastcall* SetShaderMaskTexUvShift)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, float, float);
			void* (__fastcall* SetShaderScreenTexUvShift)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, float, float);
			void* (__fastcall* SetShaderBaseTexUvRepeat)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, float, float);
			void* (__fastcall* SetShaderMaskTexUvRepeat)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, float, float);
			void* (__fastcall* SetShaderScreenTexUvRepeat)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, float, float);
			void* pad2[3];
			void* (__fastcall* SetVertexTranslate)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, unsigned int, Vectormath::Aos::Vector3*, Vectormath::Aos::Vector3*);
			void* (__fastcall* FindWindow_)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* EnableAllInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* EnableScaleInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* EnableRotationInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* EnableTranslationInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* EnableColorRGBInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* EnableColorAlphaInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* DisableAllInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* DisableScaleInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* DisableRotationInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* DisableTranslationInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* DisableColorRGBInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* DisableColorAlphaInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* PostShowAndStartMessage1)(fox::uix::impl::UixUtilityImpl* this_, void*);
			void* (__fastcall* PostShowAndStartMessage2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* PostHideAndStopMessage1)(fox::uix::impl::UixUtilityImpl* this_, void*);
			void* (__fastcall* PostHideAndStopMessage2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* PostWakeLayoutMessage)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* PostSleepLayoutMessage)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* StartAnimation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* StartInstantAnimation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*, bool);
			void* (__fastcall* StartLoopAnimation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* StartAnimationWithRatio)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*, float);
			void* (__fastcall* StartAnimationWithStartRatio)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*, float);
			void* (__fastcall* RecoverForAnimationWithRatio)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* PauseAnimation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* PauseInitAnimation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* PauseEndAnimation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* IsAnimationRunning)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* SetAnimationStopTime)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*, float);
			void* (__fastcall* SetAnimationSpeed)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*, float);
			void* (__fastcall* IsAnimationPastTime)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*, float);
			void* (__fastcall* CreateTextUnit)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::TextUnit**, int);
			void* (__fastcall* DeleteTextUnit)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::TextUnit*);
			void* (__fastcall* SetTextForModelNodeText)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, fox::ui::TextUnit*, char const*, bool);
			void* (__fastcall* SetScrollTextForModelNodeText)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, fox::ui::TextUnit*, char const*, bool);
			void* (__fastcall* SetScrollTextForPrefabListRecord)(fox::uix::impl::UixUtilityImpl* this_, fox::uix::PrefabRecordCallFunc*, fox::ui::ModelNodeText*, fox::ui::TextUnit*, char const*, bool);
			void* (__fastcall* UpdateScrollTextForPrefabListRecord)(fox::uix::impl::UixUtilityImpl* this_, fox::uix::PrefabRecordCallFunc*, bool);
			void* (__fastcall* SetupTextUnits)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, fox::ui::TextUnit*, unsigned int, char const*, bool, bool);
			void* (__fastcall* SetTextUnitsForModelNodeText)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, fox::ui::TextUnit*, unsigned int);
			void* (__fastcall* CreateBoxText)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, fox::ui::TextUnit*, unsigned int, char const*, bool, bool);
			void* (__fastcall* CreateScrollBoxText)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, fox::ui::TextUnit*, unsigned int, char const*);
			void* pad5[5];
			void* (__fastcall* GetLangText1)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* GetLangText2)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* StartTextScroll)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*);
			void* (__fastcall* StopTextScroll)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*);
			void* (__fastcall* GetTextTotalHeightInTextNode)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText const*);
			void* (__fastcall* GetTextMaxWidthInTextNode)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText const*);
			void* (__fastcall* GetTextNodeWidth)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText const*);
			void* (__fastcall* GetTextNodeHeight)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText const*);
			void* (__fastcall* SetTextNodeWidth)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, float);
			void* (__fastcall* SetTextNodeHeight)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, float);
			void* (__fastcall* SetOneSideVertexControl)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*, fox::StringId, fox::StringId, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*, float, float, Vectormath::Aos::Vector3*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetBodyGauge)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, float, float, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*);
			void* (__fastcall* SetBodyGaugeWithAxis)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, float, float, Vectormath::Aos::Vector3*, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*);
			void* (__fastcall* SetBaseGauge)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, float, float, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*);
			void* (__fastcall* SetBaseGaugeWithAxis)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, float, float, Vectormath::Aos::Vector3*, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*);
			void* (__fastcall* SetGaugeLayout)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, fox::StringId, fox::StringId, float, float, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*);
			void* (__fastcall* SetGaugeLayoutWithAxis)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, fox::StringId, fox::StringId, float, float, Vectormath::Aos::Vector3*, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*);
			void* (__fastcall* SetLineDraw)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeLine*, fox::ui::LineDraw*, Vectormath::Aos::Vector4*);
			void* (__fastcall* ResetLineDraw)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeLine*);
			void* (__fastcall* GetReceivePropertyValue)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::GraphState*, fox::StringId);
			void* (__fastcall* EndUpdateGraphState)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::GraphState*);
			void* (__fastcall* SetModelNodePriority)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeCommon*, char);
			void* (__fastcall* UpdateModelDrawOrder)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*);
			void* (__fastcall* SetModelStencil)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*, bool);
			void* (__fastcall* SetModelNodeBillboard)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeCommon*, bool);
			void* (__fastcall* GetDrawPriority)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*);
			void* (__fastcall* SetDrawPriority)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, char);
			void* (__fastcall* GetDrawPriorityFromTable)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* DisableShaderReset1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*);
			void* (__fastcall* DisableShaderReset2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*);
			void* (__fastcall* DisableShaderReset3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*);
			void* (__fastcall* GetGameFrameRawDeltaTime)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* GetGameFrameElapsedTimeSinceStartup)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* RotQuatToEuler)(fox::uix::impl::UixUtilityImpl* this_, Vectormath::Aos::Quat*);
			void* (__fastcall* RotEulerToQuat)(fox::uix::impl::UixUtilityImpl* this_, Vectormath::Aos::Vector3*);
			void* (__fastcall* RotEularRadianToDegree)(fox::uix::impl::UixUtilityImpl* this_, Vectormath::Aos::Vector3*);
			void* (__fastcall* RotEularDegreeToRadian)(fox::uix::impl::UixUtilityImpl* this_, Vectormath::Aos::Vector3*);
			void* (__fastcall* GetPathIdFromChar)(fox::uix::impl::UixUtilityImpl* this_, char const*);
			fox::Color* (__fastcall* GetPaletteColor)(fox::uix::impl::UixUtilityImpl* this_, fox::Color*, fox::StringId);
		};

		struct UixUtilityImpl_vtbl_tpp
		{
			void* (__fastcall* __constructor)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* __empty1)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* __empty2)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* GetButtonNumber)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, bool);
			void* (__fastcall* GetActivePadNo)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* IsButtonPress)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsButtonPressByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsButtonPressing)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsButtonPressingByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsButtonRelease)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsButtonReleaseByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsButtonStandby)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsButtonStandbyByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsButtonRepeat)(fox::uix::impl::UixUtilityImpl* this_, unsigned int, float, float);
			void* (__fastcall* IsButtonRepeatByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, float, float);
			void* (__fastcall* IsButtonLongPress)(fox::uix::impl::UixUtilityImpl* this_, unsigned int, float);
			void* (__fastcall* IsButtonLongPressByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, float);
			void* (__fastcall* GetButtonLongPressResultByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, float);
			void* (__fastcall* GetAnalogStickAxis)(fox::uix::impl::UixUtilityImpl* this_, unsigned int, float*, float*);
			void* (__fastcall* SetDisablePad)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* ResetDisablePad)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsDisablePad)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsUseKeyboard)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* SetUseKeyboard)(fox::uix::impl::UixUtilityImpl* this_, bool);
			void* (__fastcall* IsEnableKeyboard)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* GetKeyButtonPress)(fox::uix::impl::UixUtilityImpl* this_, fox::KeyCode);
			void* (__fastcall* GetInputString)(fox::uix::impl::UixUtilityImpl* this_, char*, int);
			void* (__fastcall* ClearInputString)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* ButtonNameToKeyCode)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, unsigned int*);
			void* (__fastcall* IsKeyboardPress)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsKeyboardPressByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsKeyboardPressing)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsKeyboardPressingByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsKeyboardRelease)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsKeyboardReleaseByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsKeyboardStandby)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsKeyboardStandbyByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsKeyboardRepeat)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsKeyboardRepeatByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsKeyboardLongPress)(fox::uix::impl::UixUtilityImpl* this_, unsigned int, float);
			void* (__fastcall* IsKeyboardLongPressByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, float);
			void* (__fastcall* GetKeyboardLongPressResultByName1)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, float);
			void* (__fastcall* GetMouseButtonsByKeyCode)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* GetMouseMoveXY)(fox::uix::impl::UixUtilityImpl* this_, float*, float*);
			void* (__fastcall* GetMouseAccelXY)(fox::uix::impl::UixUtilityImpl* this_, float*, float*);
			void* (__fastcall* GetMouseWheel)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* IsMouseButtonDown)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsMouseButtonDownByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsMouseButtonPressing)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsMouseButtonPressingByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* IsMouseButtonUp)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* IsMouseButtonUpByName)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* GetKeyboardLongPressResultByName2)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId, float);
			void* (__fastcall* GetMotionSensorOrientation)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* GetMotionSensorAcceleration)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* GetMotionSensorAngularVelocity)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* IsMotionSensorAvailable)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* ResetMotionSensorOrientation)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* SendTrigger)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*, unsigned __int64, unsigned __int64);
			void* (__fastcall* SendTriggerToParent)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*, unsigned __int64, unsigned __int64);
			void* (__fastcall* IsTriggerReceived1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::TriggerPool const*, unsigned __int64, unsigned __int64*);
			void* (__fastcall* IsTriggerReceived2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::TriggerPool const*, unsigned __int64, bool);
			void* (__fastcall* GetTriggerPool)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* SendVisibleWindowMessage)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface*, bool);
			void* (__fastcall* SendConnectWindowMessage)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface*, fox::ui::Model*, fox::ui::ModelNode*);
			void* (__fastcall* GetChildWindowInterface)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*, fox::StringId);
			void* pad1[2];
			fox::ui::Layout* (__fastcall* GetLayout1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout const*, fox::StringId);
			fox::ui::Layout* (__fastcall* GetLayout2)(fox::uix::impl::UixUtilityImpl* this_, void* const, fox::StringId);
			fox::ui::Layout* (__fastcall* GetLayout3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*, fox::StringId);
			void* (__fastcall* IsHaveLayout)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout const*, fox::StringId);
			void* pad2[1];
			fox::ui::Model* (__fastcall* GetModel)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout const*, fox::StringId);
			void* (__fastcall* GetAnimation1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model const*, fox::StringId);
			void* (__fastcall* GetAnimation2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout const*, fox::StringId);
			void* (__fastcall* IsHaveModelNodeCommon)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model const*, fox::StringId);
			void* (__fastcall* GetModelNodeCommon)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model const*, fox::StringId);
			void* (__fastcall* GetModelNodeText)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model const*, fox::StringId);
			fox::ui::ModelNodeMesh* (__fastcall* GetModelNodeMesh)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model const*, fox::StringId);
			void* (__fastcall* GetModelNodeLine)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model const*, fox::StringId);
			void* (__fastcall* GetOrthogonalProjectionScaleFromModelComponentCamera)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*);
			void* pad3[3];
			void* (__fastcall* SetVisible1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, bool);
			void* (__fastcall* SetVisible2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, bool);
			void* (__fastcall* IsVisible1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			void* (__fastcall* IsVisible2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
			void (__fastcall* SetAlpha1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void (__fastcall* SetAlpha2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			float (__fastcall* GetAlpha1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			float (__fastcall* GetAlpha2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
			void* (__fastcall* SetColorRGB1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, fox::Rgba8);
			void* (__fastcall* SetColorRGB2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::Rgba8);
			void* (__fastcall* SetColorRGB3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, fox::Color*);
			void* (__fastcall* SetColorRGB4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::Color*);
			void* (__fastcall* SetColorRGB5)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float, float, float);
			void* (__fastcall* SetColorRGB6)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float, float, float);
			void* (__fastcall* SetColorGroup1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, unsigned int);
			void* (__fastcall* SetColorGroup2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, unsigned int);
			void* (__fastcall* GetColorGroup3)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* GetTranslation1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			void* (__fastcall* GetTranslation2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
			void* (__fastcall* SetTranslation1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetTranslation2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetTranslationX1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetTranslationX2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* SetTranslationY1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetTranslationY2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* SetTranslationZ1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetTranslationZ2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* GetRotation1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			void* (__fastcall* GetRotation2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
			void* (__fastcall* GetRotationDegree1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			void* (__fastcall* GetRotationDegree2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
			void* (__fastcall* SetRotation1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetRotation2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetRotationDegree1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetRotationDegree2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetRotationDegreeX1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetRotationDegreeX2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* SetRotationDegreeY1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetRotationDegreeY2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* SetRotationDegreeZ1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetRotationDegreeZ2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* SetQuaternion1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, Vectormath::Aos::Quat*);
			void* (__fastcall* SetQuaternion2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, Vectormath::Aos::Quat*);
			void* (__fastcall* SetScale1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetScale2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* SetScale3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetScale4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, Vectormath::Aos::Vector3*);
			void* (__fastcall* GetScale1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			void* (__fastcall* GetScale2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
			void* (__fastcall* SetStatus1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeLine*, unsigned int);
			void* (__fastcall* SetStatus2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, unsigned int);
			void* (__fastcall* SetStatus3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeLine*, unsigned int);
			void* (__fastcall* SetStatus4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*, unsigned int);
			void* (__fastcall* ResetStatus1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeLine*, unsigned int);
			void* (__fastcall* ResetStatus2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, unsigned int);
			void* (__fastcall* ResetStatus3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeLine*, unsigned int);
			void* (__fastcall* ResetStatus4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*, unsigned int);
			void* (__fastcall* SetVerticalAlign)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, char);
			void* (__fastcall* SetTextAlign)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, char);
			void* (__fastcall* SetFontSize)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, float, float);
			void* (__fastcall* GetLineWidth)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, unsigned int);
			void* (__fastcall* GetLineHeight)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, unsigned int);
			void* (__fastcall* SetLineWidth)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, unsigned int, float);
			void* (__fastcall* SetLineHeight)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, unsigned int, float);
			void* (__fastcall* ConnectComponent1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::ui::LayoutComponent*, unsigned int);
			void* (__fastcall* ConnectComponent2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::ui::LayoutComponent*, fox::StringId);
			void* (__fastcall* ConnectComponent3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::ui::LayoutComponent*, fox::ui::ModelNode const*);
			void* (__fastcall* GetTraversalTranslation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::ui::ModelNode*);
			void* (__fastcall* GetTraversalScale)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, fox::ui::ModelNode*);
			void* (__fastcall* SetTextureName1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, unsigned int, unsigned int, bool, unsigned int, unsigned int, unsigned int);
			void* (__fastcall* SetTextureName2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::StringId, fox::StringId, bool, fox::StringId, fox::StringId, fox::StringId);
			void* (__fastcall* SetTextureName3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::Path const*, fox::StringId);
			void* (__fastcall* SetTextureName4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, char const*, fox::StringId, int);
			void* (__fastcall* SetTextureName5)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, char const*, fox::StringId);
			void* (__fastcall* SetTextureName6)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::PathId, fox::StringId, int);
			void* (__fastcall* SetTextureName7)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::PathId, fox::StringId);
			void* (__fastcall* CheckAndSetTextureName1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, char const*, fox::StringId, int);
			void* (__fastcall* CheckAndSetTextureName2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, char const*, fox::StringId);
			void* (__fastcall* CheckAndSetTextureName3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::PathId, fox::StringId, int);
			void* (__fastcall* CheckAndSetTextureName4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::PathId, fox::StringId);
			void* (__fastcall* SetGrTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, void*, unsigned int);
			void* (__fastcall* CreatePrefetchTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::PathId, int);
			void* (__fastcall* RemovePrefetchTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::PathId);
			void* (__fastcall* IsReadyPrefetchTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::PathId, int);
			void* (__fastcall* RegisterLoadingMeshTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::PathId);
			void* (__fastcall* UnregisterLoadingMeshTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::PathId);
			void* (__fastcall* SetLoadingTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::PathId, int);
			void* (__fastcall* IsDispLoadingTexture)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, unsigned int);
			void* (__fastcall* IsTextureLoaded)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::StringId, int);
			void* (__fastcall* SetTextureLevel)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::StringId, int);
			void* (__fastcall* SetShaderParameter)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::StringId, Vectormath::Aos::Vector4*);
			void* (__fastcall* SetShaderTechnique)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::StringId);
			void* (__fastcall* SetShaderBaseTexUvShift)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, float, float);
			void* (__fastcall* SetShaderMaskTexUvShift)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, float, float);
			void* (__fastcall* SetShaderScreenTexUvShift)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, float, float);
			void* (__fastcall* SetShaderBaseTexUvRepeat)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, float, float);
			void* (__fastcall* SetShaderMaskTexUvRepeat)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, float, float);
			void* (__fastcall* SetShaderScreenTexUvRepeat)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, float, float);
			void* pad4[3];
			void* (__fastcall* SetVertexTranslate)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, unsigned int, Vectormath::Aos::Vector3*, Vectormath::Aos::Vector3*);
			void* (__fastcall* FindWindow_)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* EnableAllInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* EnableScaleInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* EnableRotationInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* EnableTranslationInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* EnableColorRGBInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* EnableColorAlphaInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* DisableAllInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* DisableScaleInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* DisableRotationInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* DisableTranslationInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* DisableColorRGBInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* DisableColorAlphaInheritance)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* PostShowAndStartMessage1)(fox::uix::impl::UixUtilityImpl* this_, void*);
			void* (__fastcall* PostShowAndStartMessage2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* PostHideAndStopMessage1)(fox::uix::impl::UixUtilityImpl* this_, void*);
			void* (__fastcall* PostHideAndStopMessage2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* PostWakeLayoutMessage)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* PostSleepLayoutMessage)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::WindowInterface const*);
			void* (__fastcall* StartAnimation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* StartInstantAnimation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*, bool);
			void* (__fastcall* StartLoopAnimation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* StartAnimationWithRatio)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*, float);
			void* (__fastcall* StartAnimationWithStartRatio)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*, float);
			void* (__fastcall* RecoverForAnimationWithRatio)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* PauseAnimation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* PauseInitAnimation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* PauseEndAnimation)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* IsAnimationRunning)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*);
			void* (__fastcall* SetAnimationStopTime)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*, float);
			void* (__fastcall* SetAnimationSpeed)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*, float);
			void* (__fastcall* IsAnimationPastTime)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Animation*, float);
			void* (__fastcall* CreateTextUnit)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::TextUnit**, int);
			void* (__fastcall* DeleteTextUnit)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::TextUnit*);
			void* (__fastcall* SetTextForModelNodeText)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, fox::ui::TextUnit*, char const*, bool);
			void* (__fastcall* SetScrollTextForModelNodeText)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, fox::ui::TextUnit*, char const*, bool);
			void* (__fastcall* SetScrollTextForPrefabListRecord)(fox::uix::impl::UixUtilityImpl* this_, fox::uix::PrefabRecordCallFunc*, fox::ui::ModelNodeText*, fox::ui::TextUnit*, char const*, bool);
			void* (__fastcall* UpdateScrollTextForPrefabListRecord)(fox::uix::impl::UixUtilityImpl* this_, fox::uix::PrefabRecordCallFunc*, bool);
			void* (__fastcall* SetupTextUnits)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, fox::ui::TextUnit*, unsigned int, char const*, bool, bool);
			void* (__fastcall* SetTextUnitsForModelNodeText)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, fox::ui::TextUnit*, unsigned int);
			void* (__fastcall* CreateBoxText)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, fox::ui::TextUnit*, unsigned int, char const*, bool, bool);
			void* (__fastcall* CreateScrollBoxText)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, fox::ui::TextUnit*, unsigned int, char const*);
			void* pad5[5];
			void* (__fastcall* GetLangText1)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* GetLangText2)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
			void* (__fastcall* StartTextScroll)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*);
			void* (__fastcall* StopTextScroll)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*);
			void* (__fastcall* GetTextTotalHeightInTextNode)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText const*);
			void* (__fastcall* GetTextMaxWidthInTextNode)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText const*);
			void* (__fastcall* GetTextNodeWidth)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText const*);
			void* (__fastcall* GetTextNodeHeight)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText const*);
			void* (__fastcall* SetTextNodeWidth)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, float);
			void* (__fastcall* SetTextNodeHeight)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeText*, float);
			void* (__fastcall* SetOneSideVertexControl)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*, fox::StringId, fox::StringId, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*, float, float, Vectormath::Aos::Vector3*, Vectormath::Aos::Vector3*);
			void* (__fastcall* SetBodyGauge)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, float, float, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*);
			void* (__fastcall* SetBodyGaugeWithAxis)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, float, float, Vectormath::Aos::Vector3*, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*);
			void* (__fastcall* SetBaseGauge)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, float, float, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*);
			void* (__fastcall* SetBaseGaugeWithAxis)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, float, float, Vectormath::Aos::Vector3*, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*);
			void* (__fastcall* SetGaugeLayout)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, fox::StringId, fox::StringId, float, float, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*);
			void* (__fastcall* SetGaugeLayoutWithAxis)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, fox::StringId, fox::StringId, float, float, Vectormath::Aos::Vector3*, fox::ui::ModelNodeCommon*, fox::ui::ModelNodeCommon*);
			void* (__fastcall* SetLineDraw)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeLine*, fox::ui::LineDraw*, Vectormath::Aos::Vector4*);
			void* (__fastcall* ResetLineDraw)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeLine*);
			void* (__fastcall* GetReceivePropertyValue)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::GraphState*, fox::StringId);
			void* (__fastcall* EndUpdateGraphState)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::GraphState*);
			void* (__fastcall* SetModelNodePriority)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeCommon*, char);
			void* (__fastcall* UpdateModelDrawOrder)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*);
			void* (__fastcall* SetModelStencil)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*, bool);
			void* (__fastcall* SetModelNodeBillboard)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeCommon*, bool);
			void* (__fastcall* GetDrawPriority)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*);
			void* (__fastcall* SetDrawPriority)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*, char);
			void* (__fastcall* GetDrawPriorityFromTable)(fox::uix::impl::UixUtilityImpl* this_, unsigned int);
			void* (__fastcall* DisableShaderReset1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*);
			void* (__fastcall* DisableShaderReset2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Model*);
			void* (__fastcall* DisableShaderReset3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::Layout*);
			void* (__fastcall* GetGameFrameRawDeltaTime)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* GetGameFrameElapsedTimeSinceStartup)(fox::uix::impl::UixUtilityImpl* this_);
			void* (__fastcall* RotQuatToEuler)(fox::uix::impl::UixUtilityImpl* this_, Vectormath::Aos::Quat*);
			void* (__fastcall* RotEulerToQuat)(fox::uix::impl::UixUtilityImpl* this_, Vectormath::Aos::Vector3*);
			void* (__fastcall* RotEularRadianToDegree)(fox::uix::impl::UixUtilityImpl* this_, Vectormath::Aos::Vector3*);
			void* (__fastcall* RotEularDegreeToRadian)(fox::uix::impl::UixUtilityImpl* this_, Vectormath::Aos::Vector3*);
			void* (__fastcall* GetPathIdFromChar)(fox::uix::impl::UixUtilityImpl* this_, char const*);
			fox::Color* (__fastcall* GetPaletteColor)(fox::uix::impl::UixUtilityImpl* this_, fox::Color*, fox::StringId);
		};

		union UixUtilityImpl_vtbl
		{
			UixUtilityImpl_vtbl_tpp tpp;
			UixUtilityImpl_vtbl_mgo mgo;
		};

		struct UixUtilityImpl
		{
			UixUtilityImpl_vtbl* __vftable;
		};
	}

	namespace tpp::ui::hud
	{
		struct CommonDataManager_mgo
		{
			char __pad0[4440];
			AnnounceLogViewer* announceLogViewer;
		};

		struct CommonDataManager_tpp
		{
			char __pad0[4392];
			AnnounceLogViewer* announceLogViewer;
		};

		union CommonDataManager
		{
			CommonDataManager_mgo mgo;
			CommonDataManager_tpp tpp;
		};
	}
	
	namespace tpp::sys
	{
		struct WeatherManager_unk1
		{
			char __pad0[3784];
			bool pause;
			char __pad1[7];
			float current;
		};

		struct WeatherManager
		{
			char __pad0[128];
			WeatherManager_unk1* clock;
			char __pad1[418];
		};
	}

	namespace fox
	{
		struct ApplicationSystem_mgo
		{
			struct vtable
			{

			};

			vtable* __vftable;
			void* blockController;
			tpp::gm::ScriptVars* scriptVars;
			void* scriptVarsSystem;
			void* gameUtility;
			void* attachUtility;
			void* timeSystem;
			void* gameConfig;
			void* unk;
			tpp::ui::impl::UiSystemImpl* uiSystem;
		};

		struct ApplicationSystem_tpp
		{
			struct vtable
			{

			};

			vtable* __vftable;
			void* blockController;
			tpp::gm::ScriptVars* scriptVars;
			void* scriptVarsSystem;
			void* gameUtility;
			void* attachUtility;
			void* timeSystem;
			void* gameConfig;
			tpp::ui::impl::UiSystemImpl* uiSystem;
			void* demoService;
			void* applicationSystemSub_9;
			void* playerRecordInstance;
			void* networkSystem;
			void* coder;
			void* applicationSystemSub_13;
			void* weatherSystem;
			void* applicationSystemSub_15;
			void* applicationSystemSub_16;
			void* applicationSystemSub_17;
			void* applicationSystemSub_18;
			void* damageParameterTable;
			void* applicationSystemSub_20;
			void* applicationSystemSub_21;
			void* ladderManager;
			void* applicationSystemSub_23;
			void* landingZoneSystem;
			void* snipePoint2System;
			void* searchMissilePointSystem;
			void* hidePointSystem;
			void* buddyService;
			void* applicationSystemSub_29;
			void* gameStatusInterface;
			void* missionSystem;
			void* corpseManager;
			tpp::mbm::impl::MotherBaseManagementSystemImpl* motherBaseManagementSystem;
			void* motherBaseManagementService;
			void* applicationSystemSub_35;
			void* applicationSystemSub_36;
			void* applicationSystemSub_37;
			void* applicationSystemSub_38;
			void* speechSystem;
			void* gimmickManager;
			void* permanentGimmickSystem;
			void* searchableLightSystem;
			void* coloringSystem;
			void* applicationSystemSub_44;
			void* applicationSystemSub_45;
			void* neutralizeMessageSender;
			void* supportRequestSystem;
			void* applicationSystemSub_48;
			void* applicationSystemSub_49;
			void* playerInfoInterface;
			void* player2Utility;
			void* applicationSystemSub_52;
			void* applicationSystemSub_53;
			void* coverPointSystem;
			void* applicationSystemSub_55;
			void* combatLocatorSystem;
			void* noticeObjectSystem;
			void* noiseAreaSystem;
			void* applicationSystemSub_59;
			void* applicationSystemSub_60;
			void* applicationSystemSub_61;
			void* applicationSystemSub_62;
			void* applicationSystemSub_63;
			void* applicationSystemSub_64;
			void* applicationSystemSub_65;
			void* blastSystem;
			void* applicationSystemSub_67;
			void* applicationSystemSub_68;
			void* applicationSystemSub_69;
			void* applicationSystemSub_70;
			void* applicationSystemSub_71;
			void* applicationSystemSub_72;
			void* applicationSystemSub_73;
			void* applicationSystemSub_74;
			void* applicationSystemSub_75;
			void* applicationSystemSub_76;
			void* applicationSystemSub_77;
			void* chimeraPartsSetWork;
			void* applicationSystemSub_79;
			void* applicationSystemSub_80;
			void* applicationSystemSub_81;
			void* applicationSystemSub_82;
			void* vechicleSystem;
			void* horseSystem;
			void* applicationSystemSub_85;
			void* walkerGearSystem;
			void* walkerGear2Utility;
			void* applicationSystemSub_88;
			void* soldier2FaceSystem;
			void* applicationSystemSub_90;
			void* applicationSystemSub_91;
			void* applicationSystemSub_92;
			void* customizeSystem;
			void* applicationSystemSub_94;
			void* applicationSystemSub_95;
			void* applicationSystemSub_96;
			void* equipBlockController2;
			void* efInterface;
			void* animalInfoService;
			void* animalSystem;
			void* avatarEditResourceSystem;
			void* eventLogSystem;
			void* applicationSystemSub_103;
			void* applicationSystemSub_104;
			void* applicationSystemSub_105;
			void* applicationSystemSub_106;
			void* applicationSystemSub_107;
			void* applicationSystemSub_108;
			void* applicationSystemSub_109;
			void* applicationSystemSub_110;
			void* applicationSystemSub_111;
			void* applicationSystemSub_112;
			void* applicationSystemSub_113;
			void* applicationSystemSub_114;
			void* applicationSystemSub_115;
			void* applicationSystemSub_116;
			void* applicationSystemSub_117;
			void* applicationSystemSub_118;
			void* applicationSystemSub_119;
			void* applicationSystemSub_120;
		};

		union ApplicationSystem
		{
			ApplicationSystem_tpp tpp;
			ApplicationSystem_mgo mgo;
		};

		struct QuarkSystemTable
		{
			void* coreSystem;
			void* memorySystem;
			void* logSystem;
			void* __system_0;
			void* __system_1;
			void* luaSystem;
			void* luaExtSystem;
			void* __system_4;
			void* timeSystem;
			void* __system_6;
			void* __system_7;
			void* fileSystem;
			void* __system_9;
			void* __system_10;
			void* graphicsSystem;
			void* networkSystem;
			void* __system_13;
			void* synchronizationSystem;
			void* __system_15;
			ApplicationSystem* applicationSystem;
			void* __system_17;
			void* __system_18;
			void* soundSystem;
			void* padSystem;
			void* __system_21;
			void* __system_22;
			void* __system_23;
			fox::impl::message2::MessageSystem2Impl* messageSystem2;
			void* __system_25;
			void* uixSystem;
			void* eventTimerSystem;
			void* __system_28;
			void* __system_29;
		};
	}

	namespace tpp::mp
	{
		struct TeamInfo
		{

		};

		struct Ruleset_unk1
		{
			struct vtable
			{
				void(__fastcall* meth_1)(Ruleset_unk1*);
				void(__fastcall* meth_2)(Ruleset_unk1*);
				void(__fastcall* meth_3)(Ruleset_unk1*);
				void(__fastcall* meth_4)(Ruleset_unk1*);
				void(__fastcall* meth_5)(Ruleset_unk1*);
				void(__fastcall* meth_6)(Ruleset_unk1*);
				void(__fastcall* meth_7)(Ruleset_unk1*);
				void(__fastcall* meth_8)(Ruleset_unk1*);
				void(__fastcall* meth_9)(Ruleset_unk1*);
				void(__fastcall* meth_10)(Ruleset_unk1*);
				void(__fastcall* meth_11)(Ruleset_unk1*);
				void(__fastcall* meth_12)(Ruleset_unk1*);
				void(__fastcall* meth_13)(Ruleset_unk1*);
				void(__fastcall* meth_14)(Ruleset_unk1*);
				void(__fastcall* meth_15)(Ruleset_unk1*);
				void(__fastcall* meth_16)(Ruleset_unk1*);
				void(__fastcall* meth_17)(Ruleset_unk1*);
				void(__fastcall* meth_18)(Ruleset_unk1*);
				void(__fastcall* meth_19)(Ruleset_unk1*);
				const char* (__fastcall* GetGamerTagBySessionIndex)(Ruleset_unk1*, unsigned char);
				void(__fastcall* meth_21)(Ruleset_unk1*);
				void(__fastcall* meth_22)(Ruleset_unk1*);
				void(__fastcall* meth_23)(Ruleset_unk1*);
				void(__fastcall* meth_24)(Ruleset_unk1*);
				void(__fastcall* meth_25)(Ruleset_unk1*);
				void(__fastcall* meth_26)(Ruleset_unk1*);
				void(__fastcall* meth_27)(Ruleset_unk1*);
				double(__fastcall* GetTimeSpentInCurrentRound)(Ruleset_unk1*);
				double(__fastcall* GetTimeSpentInCurrentState)(Ruleset_unk1*);
				void(__fastcall* meth_30)(Ruleset_unk1*);
			};

			vtable* __vftable;
			char __pad0[32];
		};

		struct Ruleset_Player
		{

		};

		struct Ruleset_unk2
		{
			char __pad0[24];
			void* rulesetData;
		};

		struct Ruleset
		{
			char __pad0[56];
			Ruleset_unk2* unk2;
			char __pad9[24];
			Ruleset_unk1 unk1;
			char __pad1[64];
			int numTeams;
			TeamInfo** teams;
			char __pad2[20];
			int numPlayers;
			tpp::mp::Ruleset_Player** players;
			char __pad3[128];
			int a1;
			char __pad4[68];
			fox::Script* script;
			char __pad5[12];
			int state;
			char __pad6[60];
			unsigned char currentRound;
			char __pad7[9];
			unsigned char localPlayerSessionIndex;
			char __pad8[961];
			char playerTeams[16];
		};

		static_assert(offsetof(Ruleset, playerTeams) == 1496);
		static_assert(offsetof(Ruleset, state) == 460);
		static_assert(offsetof(Ruleset, currentRound) == 524);

		struct RulesetManager_unk1
		{
			char __pad0[24];
			Ruleset* activeRuleset;
		};

		struct RulesetManager
		{
			char __pad0[16];
			RulesetManager_unk1* unk1;
		};

	}

#pragma pack(push, 1)
	struct match_slot_t
	{
		char m_match_rule;
		char m_map_id;
		char m_variant;
		char m_dn;
		char m_time_limit;
		char m_tickets;
		char m_unique_char;
		char m_unique_char_solid;
		char m_unique_char_liquid;
		char m_walker_gear;
		char m_weather_change;
		char unk;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct match_rules_t
	{
		match_slot_t slots[5];
		char pl_match_num;
		char pl_current_match;
		char pl_match_type;
		char pl_member_min;
		char pl_is_dedicated_host;
		char pl_auto_leave;
		char pl_dlc;
		char unk1;
		short pl_briefing_time;
		short unk2;
		int pl_total_match;
		char __pad4[8];
	};

#pragma pack(pop)

#pragma pack(push, 1)
	struct match_settings_t
	{
		int match_type;
		int unk1;
		int match_rule;
		int match_variant;
		int map_id;
		int day_night;
		int skill_level;
		int cheat_rate;
		int member_max;
		char unique_char;
		char unk5_2;
		char unk5_3;
		char walker_gear;
		char rank;
		char __pad0[3];
		int host_comment;
		char __pad1[28];
		match_rules_t rules;
		char __pad4[4];
		const char* password;
		int has_password;
		char __pad5[8];
	};
#pragma pack(pop)

	struct steam_callback_t
	{
		struct vtable
		{

		};
		vtable* __vftable;
		int callback_flags;
		int i_callback;
		void* arg;
		void* callback;
	};

#pragma pack(push, 1)
	struct mgo_match_data_member_t
	{
		char unk[8];
		char name[32];
		char __pad0[5];
		steam_id id;
		char __pad1[9];
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct mgo_match_data_t
	{
		match_slot_t slot;
		match_rules_t rules;
		mgo_match_data_member_t members[16];
		char member_num;
		char member_limit;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct mgo_match_t
	{
		int error1;
		char __pad10[4];
		int error2;
		char __pad11[1];
		char is_joining_invite;
		steam_id invite_lobby_id;
		char __pad1[2];
		match_settings_t match_settings;
		char __pad2[48];
		__int16 lobby_member_limit;
		char __pad3[90];
		steam_id lobby_owner;
		char __pad4[76];
		steam_id lobby_id;
		char __pad5[1004];
		char st_started;
		char st_is_transition;
		char __pad6[2];
		match_rules_t match_rules;
		steam_id lobby_id2;
		char __pad7[412];
		mgo_match_data_t* data;
		steam_id kicked_ids[16];
		int kick_num;
		char __pad8[852];
		steam_callback_t lobby_data_changed;
		steam_callback_t lobby_chat_update;
		steam_callback_t lobby_chat_msg;
		steam_callback_t unk_callback1;
		steam_callback_t unk_callback2;
		steam_callback_t unk_callback3;
		char __pad9[8];
	};
#pragma pack(pop)

	static_assert(offsetof(mgo_match_t, st_started) == 1444);

	struct mgo_match_container_t
	{
		int unk1;
		int unk2;
		mgo_match_t* match;
		void* unk3;
	};

	struct mgo_matchmaking_manager;
	struct mgo_matchmaking_manager_interface1;
	struct mgo_matchmaking_manager_interface2;

	struct mgo_matchmaking_manager_vtbl
	{
		void* (__fastcall* get_number)(mgo_matchmaking_manager* this_); // return 37
		void* (__fastcall* update_match)(mgo_matchmaking_manager* this_);
		void* (__fastcall* sub_1408920B0)(mgo_matchmaking_manager* this_);
		void* (__fastcall* sub_140890ED0)(mgo_matchmaking_manager* this_);
	};

	struct mgo_matchmaking_manager_interface1_vtbl
	{
		void* (__fastcall* sub_140890DCC)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140891150)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_14089A5B0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408911A0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140891530)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_14089FF20)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896DD0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896DF0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896DB0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140897F60)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_14089FCE0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896DE0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140891320)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408967F0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896820)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896850)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408993F0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A1E70)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896880)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896AB0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408995B0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A0D10)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A1FE0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898780)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408995A0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_14089A6A0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408914F0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898DF0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898860)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898A70)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408988D0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A0450)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A1930)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A10E0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A1100)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408990A0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408980B0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898BF0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898840)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A11E0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140899090)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A0B20)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A0120)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A0DC0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140897F80)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896A50)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896D80)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896D90)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_14089A190)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A05A0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A05C0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A05E0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898330)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898340)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898C10)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140891590)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896E90)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_14089A230)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140899680)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A0680)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898990)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898EF0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_14089A290)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408994C0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896DA0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898C20)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896FA0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408970E0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898EA0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A0D00)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* nullsub_1)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* nullsub_2)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140899C40)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140897F20)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140897F40)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140898E90)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_14089FCB0)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140896E20)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_140897260)(mgo_matchmaking_manager_interface1* this_);
		void* (__fastcall* sub_1408A0150)(mgo_matchmaking_manager_interface1* this_);
	};

	struct mgo_matchmaking_manager_interface2_vtbl
	{
		void* (__fastcall* sub_140899C50)(mgo_matchmaking_manager_interface2* this_);
		void* (__fastcall* sub_140890DD8)(mgo_matchmaking_manager_interface2* this_);
	};

	struct mgo_matchmaking_manager_interface1
	{
		mgo_matchmaking_manager_interface1_vtbl* __vftable;
	};

	struct mgo_matchmaking_manager_interface2
	{
		mgo_matchmaking_manager_interface2_vtbl* __vftable;
	};

	struct mgo_matchmaking_manager
	{
		mgo_matchmaking_manager_vtbl* __vftable;
		mgo_matchmaking_manager_interface1 interface1;
		mgo_matchmaking_manager_interface2 interface2;
		char __pad0[16];
		mgo_match_container_t* match_container;
		char __pad1[4480];
		match_settings_t match_settings;
		char __pad2[788];
		int state;
		int unk3;
		char __pad3[22];
		char unk1;
		char __pad4[8];
		char unk2;
		char __pad5[100]; // idk
	};

	static_assert(sizeof(match_rules_t) == 84);

	static_assert(sizeof(mgo_match_container_t) == 24);

	static_assert(offsetof(mgo_match_t, is_joining_invite) == 13);
	static_assert(offsetof(mgo_match_t, invite_lobby_id) == 14);
	static_assert(offsetof(mgo_match_t, lobby_id) == 432);
	static_assert(sizeof(mgo_match_t) == 3144);

	static_assert(sizeof(match_settings_t) == 184);

	static_assert(offsetof(match_settings_t, member_max) == 32);
	static_assert(offsetof(mgo_matchmaking_manager, state) == 5500);
}
