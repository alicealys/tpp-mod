#pragma once
#include <d3d11.h>

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
			char __pad0[24];
		};
	}

	namespace lua
	{
		struct lua_State
		{

		};
	}

	namespace tpp::ui::menu
	{
		struct UiCommonDataManager;
	}

	namespace gn
	{
		struct Buffer
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

		struct HashMapBase
		{
			char __pad0[48];
		};

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
				fox::gr::SceneObjectList** objectList1;
				fox::gr::SceneObjectList* objectList2;
				char __pad3[16];
				void* lock;
			};

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
				char __pad2[36];
				int flags;
				char __pad3[8];
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
					float a1;
					float a2;
					char a3_1;
					char a3_2;
					char a3_3;
					char a3_4;
					char a4_1;
					char a4_2;
					char a4_3;
					char a4_4;
					float a5;
					float a6;
					float a7;
					float a8;
				};

				struct ImageGlyphManager
				{
					char __pad0[8];
					_TextureGlyphData* imageGlyphs;
					fox::HashMapBase hashMap;
					char __pad1[24];
					int numGlyphs;
					int numUnk;
					char __pad2[256];
				};

				struct FontSystem_unk1
				{
					char __pad0[10];
					char c1;
					char __pad1[53];
					__int16 s1;
					char c2;
				};

				struct FontSystem_unk2
				{
					char __pad0[72];
					ImageGlyphManager* imageGlyphManager;
				};

				struct FontSystem
				{
					char __pad0[8];
					void* ptr_array1[9];
					fox::gr::dg::FontSystem_unk1* unk1;
					fox::gr::dg::FontSystem_unk2* unk2;
				};

				struct DynamicVertexBuffer_unk1
				{
					char __pad0[20];
					int count;
				};

				struct DynamicVertexBuffer
				{
					char __pad0[64];
					DynamicVertexBuffer_unk1* unk1;
					char __pad1[8];
					gn::Buffer* buffer;
					char __pad2[16];
					void* ptr1;
					unsigned int a1;
					unsigned int a2;
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
					void* ptr4;
					char __pad1[4];
					float f1;
					int viewX;
					int viewY;
					int viewWidth;
					int viewHeight;
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

				struct ShadowConstantRegister
				{

				};

				struct CommandBuffer
				{
					char __pad0[40];
					fox::gr::dg::ShadowConstantRegister* shadowConstantRegister;
					char __pad1[16];
					fox::gr::ResourceId material;
					char __pad2[556];
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

				namespace plugins
				{
					struct Draw2DRenderer_unk
					{
						unsigned char flags1[8];
						char __pad0[112];
						unsigned int flags2;
						unsigned int flags3;
					};

					struct Draw2DRenderer
					{
						char __pad0[720];
						Vectormath::Aos::Vector4 viewVec1;
						Vectormath::Aos::Vector4 viewVec2;
						BuildDraw2DParameters* parameters;
						CommandBuffer* commandBuffer;
						Draw2DRenderer_unk* unk;
						char __pad1[16];
						fox::gr::ResourceId shader;
						gn::Buffer* buffer;
						unsigned int size;
						char __pad2[14];
						char unk1;
						char __pad3[5];
						DynamicVertexBuffer* vertexBuffer;
						void* ptr1;
						char __pad4[92];
						_TextureGlyphData* glyphData;
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

		}

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

			struct Member_SessionInterface_vtbl
			{
				int(__fastcall* GetIndex)(Member* this_);
				void* (__fastcall* GetIntAnimationController)(Member* this_);
				void(__fastcall* GetSessionIdString)(Member* this_, char*, void*);
				void(__fastcall* __destructor)(Member* this_);
			};

			struct Member_SessionInterface
			{
				Member_SessionInterface_vtbl* __vftable;
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
					void* (__fastcall* GetLocalMemberInterface)(SessionImpl2_SessionInterface* this_);
					void* (__fastcall* GetHostMemberInterface)(SessionImpl2_SessionInterface* this_);
					void* (__fastcall* GetMemberInterfaceAtIndex)(SessionImpl2_SessionInterface* this_, void*);
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
					unsigned int flags;
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
			}
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
				struct MenuSystemImpl
				{

				};
			}

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
	}

	namespace tpp::mbm
	{
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

				struct StaffSeed
				{
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
			fox::StringId unkString;
			int a5;
			char a6;
		};

		struct FobTarget : ServerRequestBase
		{
			mbm::PlayerBasicInfo* playerInfos;
			char __pad2[8];
			short maxPlayers;
			char __pad3[10];
			SessionConnectInfo* sessionConnectInfo;
			DisplayName* displayName1;
			DisplayName* displayName2;
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
		struct ScriptVars
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

		static_assert(sizeof(ScriptVars) == 87488);
		static_assert(offsetof(ScriptVars, utcTime) == 0);
		static_assert(offsetof(ScriptVars, totalPlayTime) == 8);
		static_assert(offsetof(ScriptVars, locationCode) == 16);
		static_assert(offsetof(ScriptVars, missionCode) == 18);
		static_assert(offsetof(ScriptVars, prevLocationCode) == 20);
		static_assert(offsetof(ScriptVars, prevMissionCode) == 22);
		static_assert(offsetof(ScriptVars, rulesetDynamicPackIndices) == 28);
		static_assert(offsetof(ScriptVars, mbLayoutCode) == 32);
		static_assert(offsetof(ScriptVars, mbClusterId) == 34);
		static_assert(offsetof(ScriptVars, fobSneakMode) == 35);
		static_assert(offsetof(ScriptVars, fobIsSneak) == 36);
		static_assert(offsetof(ScriptVars, fobIsEvent) == 37);
		static_assert(offsetof(ScriptVars, fobIsSecurity) == 38);
		static_assert(offsetof(ScriptVars, fobIsPlaceMode) == 39);
		static_assert(offsetof(ScriptVars, fobIsInitializedFreePositionItem) == 40);
		static_assert(offsetof(ScriptVars, returnStaffHeader) == 44);
		static_assert(offsetof(ScriptVars, returnStaffSeeds) == 48);
		static_assert(offsetof(ScriptVars, eulaVersion) == 52);
		static_assert(offsetof(ScriptVars, eulaVersion2) == 56);
		static_assert(offsetof(ScriptVars, eulaVersion3) == 60);
		static_assert(offsetof(ScriptVars, countryIndex) == 64);
		static_assert(offsetof(ScriptVars, countryStateIndex) == 68);
		static_assert(offsetof(ScriptVars, infoId) == 72);
		static_assert(offsetof(ScriptVars, infoIdForMGO) == 76);
		static_assert(offsetof(ScriptVars, inquiryId) == 80);
		static_assert(offsetof(ScriptVars, fobPickup) == 84);
		static_assert(offsetof(ScriptVars, padType) == 88);
		static_assert(offsetof(ScriptVars, clock) == 92);
		static_assert(offsetof(ScriptVars, passageSecondsSinceOutMB) == 100);
		static_assert(offsetof(ScriptVars, weather) == 104);
		static_assert(offsetof(ScriptVars, weatherNextTime) == 108);
		static_assert(offsetof(ScriptVars, extraWeatherInterval) == 112);
		static_assert(offsetof(ScriptVars, weatherAddDensity) == 116);
		static_assert(offsetof(ScriptVars, requestWeatherType) == 120);
		static_assert(offsetof(ScriptVars, requestWeatherUserId) == 128);
		static_assert(offsetof(ScriptVars, requestWeatherInterp) == 148);
		static_assert(offsetof(ScriptVars, requestWeatherParam) == 168);
		static_assert(offsetof(ScriptVars, dominatedCpFlagsAfgh) == 188);
		static_assert(offsetof(ScriptVars, dominatedCpFlagsMafr) == 204);
		static_assert(offsetof(ScriptVars, totalAlertCount) == 220);
		static_assert(offsetof(ScriptVars, initialPlayerPosX) == 224);
		static_assert(offsetof(ScriptVars, initialPlayerPosY) == 228);
		static_assert(offsetof(ScriptVars, initialPlayerPosZ) == 232);
		static_assert(offsetof(ScriptVars, playerPosX) == 236);
		static_assert(offsetof(ScriptVars, playerPosY) == 240);
		static_assert(offsetof(ScriptVars, playerPosZ) == 244);
		static_assert(offsetof(ScriptVars, playerPartsType) == 248);
		static_assert(offsetof(ScriptVars, playerCamoType) == 249);
		static_assert(offsetof(ScriptVars, playerHandType) == 250);
		static_assert(offsetof(ScriptVars, playerType) == 251);
		static_assert(offsetof(ScriptVars, playerFaceId) == 252);
		static_assert(offsetof(ScriptVars, playerFaceEquipId) == 254);
		static_assert(offsetof(ScriptVars, playerSkillId) == 256);
		static_assert(offsetof(ScriptVars, playerInjuryCount) == 257);
		static_assert(offsetof(ScriptVars, playerBodyInjuryCount) == 258);
		static_assert(offsetof(ScriptVars, playerLifeMax) == 260);
		static_assert(offsetof(ScriptVars, initialPlayerRotY) == 262);
		static_assert(offsetof(ScriptVars, playerRotY) == 264);
		static_assert(offsetof(ScriptVars, initialPlayerFlag) == 266);
		static_assert(offsetof(ScriptVars, initialPlayerAction) == 268);
		static_assert(offsetof(ScriptVars, playerRetryFlag) == 269);
		static_assert(offsetof(ScriptVars, initialPlayerPairGameObjectId) == 270);
		static_assert(offsetof(ScriptVars, playerVehicleGameObjectId) == 272);
		static_assert(offsetof(ScriptVars, initialPlayerVehicleGameObjectId) == 274);
		static_assert(offsetof(ScriptVars, vehicleGameObjectIdOnPlayerOnCargo) == 276);
		static_assert(offsetof(ScriptVars, playerDisableActionFlag) == 280);
		static_assert(offsetof(ScriptVars, playerLife) == 284);
		static_assert(offsetof(ScriptVars, playerStamina) == 286);
		static_assert(offsetof(ScriptVars, playerCameraPosition) == 288);
		static_assert(offsetof(ScriptVars, playerCameraRotation) == 300);
		static_assert(offsetof(ScriptVars, playerStaffHeader) == 308);
		static_assert(offsetof(ScriptVars, playerStaffSeed) == 312);
		static_assert(offsetof(ScriptVars, playerPlayFlag) == 316);
		static_assert(offsetof(ScriptVars, playerMarkingCountInMission) == 320);
		static_assert(offsetof(ScriptVars, playerShootCountInMission) == 324);
		static_assert(offsetof(ScriptVars, shootHitCountInMission) == 328);
		static_assert(offsetof(ScriptVars, shootHitCountEliminatedInMission) == 332);
		static_assert(offsetof(ScriptVars, playerOgrePointOnStartMission) == 336);
		static_assert(offsetof(ScriptVars, playerRecoveryCountFromOgre) == 340);
		static_assert(offsetof(ScriptVars, totalChickenCapTime) == 344);
		static_assert(offsetof(ScriptVars, totalChickCapTime) == 348);
		static_assert(offsetof(ScriptVars, totalDiscoveredCount) == 352);
		static_assert(offsetof(ScriptVars, totalTakeHitCount) == 356);
		static_assert(offsetof(ScriptVars, totalMovingDistanceAtStand) == 360);
		static_assert(offsetof(ScriptVars, totalMovingDistanceAtSquat) == 364);
		static_assert(offsetof(ScriptVars, totalMovingDistanceAtCrawl) == 368);
		static_assert(offsetof(ScriptVars, totalMovingDistanceOnHorse) == 372);
		static_assert(offsetof(ScriptVars, totalMovingDistanceOnVehicle) == 376);
		static_assert(offsetof(ScriptVars, totalMovingDistanceOnWalkerGear) == 380);
		static_assert(offsetof(ScriptVars, totalMovingDistanceOnBattleGear) == 384);
		static_assert(offsetof(ScriptVars, totalMarkingCount) == 388);
		static_assert(offsetof(ScriptVars, totalReflexCount) == 392);
		static_assert(offsetof(ScriptVars, useBlackDiamondEmblem) == 396);
		static_assert(offsetof(ScriptVars, isAvatarPlayerEnable) == 397);
		static_assert(offsetof(ScriptVars, avatarFaceRaceIndex) == 398);
		static_assert(offsetof(ScriptVars, avatarFaceTypeIndex) == 399);
		static_assert(offsetof(ScriptVars, avatarFaceVariationIndex) == 400);
		static_assert(offsetof(ScriptVars, avatarFaceColorIndex) == 401);
		static_assert(offsetof(ScriptVars, avatarHairStyleIndex) == 402);
		static_assert(offsetof(ScriptVars, avatarRightEyeColorIndex) == 403);
		static_assert(offsetof(ScriptVars, avatarRightEyeBrightnessIndex) == 404);
		static_assert(offsetof(ScriptVars, avatarLeftEyeColorIndex) == 405);
		static_assert(offsetof(ScriptVars, avatarLeftEyeBrightnessIndex) == 406);
		static_assert(offsetof(ScriptVars, avatarHairColor) == 407);
		static_assert(offsetof(ScriptVars, avatarBerdStyle) == 408);
		static_assert(offsetof(ScriptVars, avatarBerdLength) == 409);
		static_assert(offsetof(ScriptVars, avatarEbrwStyle) == 410);
		static_assert(offsetof(ScriptVars, avatarEbrwWide) == 411);
		static_assert(offsetof(ScriptVars, avatarGashOrTatoVariationIndex) == 412);
		static_assert(offsetof(ScriptVars, avatarTatoColorIndex) == 413);
		static_assert(offsetof(ScriptVars, avatarAcceFlag) == 414);
		static_assert(offsetof(ScriptVars, avatarMotionFrame) == 415);
		static_assert(offsetof(ScriptVars, avatarSaveIsValid) == 475);
		static_assert(offsetof(ScriptVars, avatarEyePresetID) == 476);
		static_assert(offsetof(ScriptVars, avatarNosePresetID) == 477);
		static_assert(offsetof(ScriptVars, avatarMouthPresetID) == 478);
		static_assert(offsetof(ScriptVars, avatarChinPresetID) == 479);
		static_assert(offsetof(ScriptVars, avatarCheekPresetID) == 480);
		static_assert(offsetof(ScriptVars, avatarEyebrowPresetID) == 481);
		static_assert(offsetof(ScriptVars, avatarHeadPresetID) == 482);
		static_assert(offsetof(ScriptVars, avatarHairPresetID) == 483);
		static_assert(offsetof(ScriptVars, avatarHairColorPresetID) == 484);
		static_assert(offsetof(ScriptVars, avatarSkinColorPresetID) == 485);
		static_assert(offsetof(ScriptVars, avatarBeardPresetID) == 486);
		static_assert(offsetof(ScriptVars, avatarAccessoryPresetID) == 487);
		static_assert(offsetof(ScriptVars, avatarAllPartsPresetID) == 488);
		static_assert(offsetof(ScriptVars, avatarReserve0) == 489);
		static_assert(offsetof(ScriptVars, avatarReserve1) == 490);
		static_assert(offsetof(ScriptVars, avatarReserve2) == 491);
		static_assert(offsetof(ScriptVars, personalName) == 492);
		static_assert(offsetof(ScriptVars, personalBirthdayMonth) == 556);
		static_assert(offsetof(ScriptVars, personalBirthdayDay) == 557);
		static_assert(offsetof(ScriptVars, emblemTextureTag) == 560);
		static_assert(offsetof(ScriptVars, emblemColorL) == 576);
		static_assert(offsetof(ScriptVars, emblemColorH) == 592);
		static_assert(offsetof(ScriptVars, emblemX) == 608);
		static_assert(offsetof(ScriptVars, emblemY) == 612);
		static_assert(offsetof(ScriptVars, emblemScale) == 616);
		static_assert(offsetof(ScriptVars, emblemRotate) == 620);
		static_assert(offsetof(ScriptVars, emblemFlag) == 624);
		static_assert(offsetof(ScriptVars, emblemVersion) == 1304);
		static_assert(offsetof(ScriptVars, emblemSyncFailed) == 1305);
		static_assert(offsetof(ScriptVars, emblemSyncFailed2) == 1306);
		static_assert(offsetof(ScriptVars, isServerDataCleared) == 1307);
		static_assert(offsetof(ScriptVars, isDefaultDisconnected) == 1308);
		static_assert(offsetof(ScriptVars, didCancelPatchDlcDownloadRequest) == 1309);
		static_assert(offsetof(ScriptVars, didCancelFobPatchDlcDownloadRequest) == 1310);
		static_assert(offsetof(ScriptVars, isPersonalDirty) == 1311);
		static_assert(offsetof(ScriptVars, initWeapons) == 1312);
		static_assert(offsetof(ScriptVars, initSupportWeapons) == 1318);
		static_assert(offsetof(ScriptVars, initItems) == 1334);
		static_assert(offsetof(ScriptVars, initHandEquip) == 1350);
		static_assert(offsetof(ScriptVars, weapons) == 1352);
		static_assert(offsetof(ScriptVars, ammoInWeapons) == 1364);
		static_assert(offsetof(ScriptVars, ammoSubInWeapons) == 1376);
		static_assert(offsetof(ScriptVars, suppressorLives) == 1388);
		static_assert(offsetof(ScriptVars, isUseSuppressor) == 1394);
		static_assert(offsetof(ScriptVars, isUseFlashLight) == 1400);
		static_assert(offsetof(ScriptVars, isInitialWeapon) == 1406);
		static_assert(offsetof(ScriptVars, supportWeapons) == 1412);
		static_assert(offsetof(ScriptVars, items) == 1444);
		static_assert(offsetof(ScriptVars, cboxPosterIndices) == 1476);
		static_assert(offsetof(ScriptVars, handEquip) == 1492);
		static_assert(offsetof(ScriptVars, upgrades) == 1494);
		static_assert(offsetof(ScriptVars, ammoStockIds) == 1510);
		static_assert(offsetof(ScriptVars, ammoStockCounts) == 1556);
		static_assert(offsetof(ScriptVars, totalBatteryPowerAsGmp) == 1648);
		static_assert(offsetof(ScriptVars, currentInventorySlot) == 1652);
		static_assert(offsetof(ScriptVars, currentSupportWeaponIndex) == 1656);
		static_assert(offsetof(ScriptVars, currentItemIndex) == 1660);
		static_assert(offsetof(ScriptVars, fpsAttackMode) == 1664);
		static_assert(offsetof(ScriptVars, currentZoomStep) == 1668);
		static_assert(offsetof(ScriptVars, requestFlagsAboutEquip) == 1673);
		static_assert(offsetof(ScriptVars, weaponOriginalOwners) == 1674);
		static_assert(offsetof(ScriptVars, partsType2) == 1680);
		static_assert(offsetof(ScriptVars, camoType2) == 1681);
		static_assert(offsetof(ScriptVars, faceEquipId2) == 1682);
		static_assert(offsetof(ScriptVars, weapons2) == 1684);
		static_assert(offsetof(ScriptVars, supportWeapons2) == 1690);
		static_assert(offsetof(ScriptVars, items2) == 1706);
		static_assert(offsetof(ScriptVars, handEquip2) == 1722);
		static_assert(offsetof(ScriptVars, dirtyEffectCount) == 1724);
		static_assert(offsetof(ScriptVars, dirtyEffectBoneIndex) == 1726);
		static_assert(offsetof(ScriptVars, dirtyEffectPosition) == 1792);
		static_assert(offsetof(ScriptVars, dirtyEffectRadius) == 2560);
		static_assert(offsetof(ScriptVars, initCustomizedWeapon) == 2816);
		static_assert(offsetof(ScriptVars, customizedWeapon) == 2822);
		static_assert(offsetof(ScriptVars, initChimeraParts) == 2828);
		static_assert(offsetof(ScriptVars, chimeraParts) == 2864);
		static_assert(offsetof(ScriptVars, initChimeraPaintType) == 2900);
		static_assert(offsetof(ScriptVars, chimeraPaintType) == 2903);
		static_assert(offsetof(ScriptVars, initChimeraColorIndex) == 2906);
		static_assert(offsetof(ScriptVars, chimeraColorIndex) == 2909);
		static_assert(offsetof(ScriptVars, isUseCustomizedWeapon) == 2912);
		static_assert(offsetof(ScriptVars, customizedWeaponSlotIndex) == 2915);
		static_assert(offsetof(ScriptVars, itemLevels) == 2918);
		static_assert(offsetof(ScriptVars, itemLevelsDeveloped) == 2990);
		static_assert(offsetof(ScriptVars, developedItemLevels) == 3062);
		static_assert(offsetof(ScriptVars, suitDevelopLevel) == 3134);
		static_assert(offsetof(ScriptVars, initLoadoutInfo) == 3368);
		static_assert(offsetof(ScriptVars, loadoutInfoSaveFormVersion) == 3369);
		static_assert(offsetof(ScriptVars, loadoutInfoU8buf) == 3370);
		static_assert(offsetof(ScriptVars, loadoutInfoU16buf) == 3830);
		static_assert(offsetof(ScriptVars, loadoutInfoU32buf) == 4120);
		static_assert(offsetof(ScriptVars, currentLoadoutSlot) == 4392);
		static_assert(offsetof(ScriptVars, sortieLoadoutInfoSaveFormVersion) == 4393);
		static_assert(offsetof(ScriptVars, sortieLoadoutInfoU8buf) == 4394);
		static_assert(offsetof(ScriptVars, sortieLoadoutInfoU16buf) == 4854);
		static_assert(offsetof(ScriptVars, sortieLoadoutInfoU32buf) == 5144);
		static_assert(offsetof(ScriptVars, currentSortieLoadoutSlot) == 5416);
		static_assert(offsetof(ScriptVars, currentSortieLoadoutSlotNonRental) == 5417);
		static_assert(offsetof(ScriptVars, returnHeliLoadoutInfoSaveFormVersion) == 5418);
		static_assert(offsetof(ScriptVars, returnHeliLoadoutInfoU8buf) == 5419);
		static_assert(offsetof(ScriptVars, returnHeliLoadoutInfoU16buf) == 5880);
		static_assert(offsetof(ScriptVars, returnHeliLoadoutInfoU32buf) == 6168);
		static_assert(offsetof(ScriptVars, currentReturnHeliLoadoutSlot) == 6440);
		static_assert(offsetof(ScriptVars, currentForRentLoadoutSlotIndex) == 6441);
		static_assert(offsetof(ScriptVars, rentalLoadoutPlayerId) == 6444);
		static_assert(offsetof(ScriptVars, rentalLoadoutPlayerName) == 6448);
		static_assert(offsetof(ScriptVars, sortiePrepPlayerSnakePartsType) == 6548);
		static_assert(offsetof(ScriptVars, sortiePrepPlayerSnakeCamoType) == 6549);
		static_assert(offsetof(ScriptVars, sortiePrepPlayerSnakeFaceEquipId) == 6550);
		static_assert(offsetof(ScriptVars, sortiePrepPlayerSnakeSuitLevel) == 6552);
		static_assert(offsetof(ScriptVars, sortiePrepPlayerDDPartsType) == 6553);
		static_assert(offsetof(ScriptVars, sortiePrepPlayerDDCamoType) == 6554);
		static_assert(offsetof(ScriptVars, sortiePrepPlayerDDFaceEquipId) == 6556);
		static_assert(offsetof(ScriptVars, sortiePrepPlayerDDSuitLevel) == 6558);
		static_assert(offsetof(ScriptVars, sortiePrepPlayerType) == 6559);
		static_assert(offsetof(ScriptVars, sortiePrepPlayerStaffHeader) == 6560);
		static_assert(offsetof(ScriptVars, sortiePrepPlayerStaffSeed) == 6564);
		static_assert(offsetof(ScriptVars, sortiePrepPlayerFaceId) == 6568);
		static_assert(offsetof(ScriptVars, userPresetCustomizedWeapon) == 6572);
		static_assert(offsetof(ScriptVars, userPresetChimeraParts) == 6620);
		static_assert(offsetof(ScriptVars, userPresetChimeraColorIndex) == 6908);
		static_assert(offsetof(ScriptVars, userPresetChimeraPaintType) == 6932);
		static_assert(offsetof(ScriptVars, userPresetDWalkerParts) == 6956);
		static_assert(offsetof(ScriptVars, userPresetDWalkerPartsLevel) == 6980);
		static_assert(offsetof(ScriptVars, userPresetDWalkerAdditionalParts) == 7004);
		static_assert(offsetof(ScriptVars, userPresetDWalkerAdditionalPartsLevel) == 7016);
		static_assert(offsetof(ScriptVars, userPresetDWalkerCurrentSlot) == 7028);
		static_assert(offsetof(ScriptVars, userPresetBattleGearParts) == 7029);
		static_assert(offsetof(ScriptVars, userPresetBattleGearPartsLevel) == 7056);
		static_assert(offsetof(ScriptVars, userPresetBattleGearCurrentSlot) == 7083);
		static_assert(offsetof(ScriptVars, userPresetHeliParts) == 7084);
		static_assert(offsetof(ScriptVars, userPresetVehiclePaintType) == 7089);
		static_assert(offsetof(ScriptVars, userPresetBuddyDogParts) == 7099);
		static_assert(offsetof(ScriptVars, userPresetBuddyHorseParts) == 7101);
		static_assert(offsetof(ScriptVars, mbmUserPresetBaseColor) == 7104);
		static_assert(offsetof(ScriptVars, droppedWeapons) == 7106);
		static_assert(offsetof(ScriptVars, droppedWeaponPos) == 7112);
		static_assert(offsetof(ScriptVars, droppedWeaponRot) == 7148);
		static_assert(offsetof(ScriptVars, droppedCount) == 7196);
		static_assert(offsetof(ScriptVars, droppedAmmo) == 7208);
		static_assert(offsetof(ScriptVars, droppedSuppressorLife) == 7220);
		static_assert(offsetof(ScriptVars, droppedCarrier) == 7224);
		static_assert(offsetof(ScriptVars, droppedCarrierPlaceNo) == 7230);
		static_assert(offsetof(ScriptVars, droppedWeaponStatus) == 7233);
		static_assert(offsetof(ScriptVars, importantItems) == 7236);
		static_assert(offsetof(ScriptVars, importantItemNumber) == 7300);
		static_assert(offsetof(ScriptVars, importantItemPos) == 7364);
		static_assert(offsetof(ScriptVars, importantItemRot) == 7748);
		static_assert(offsetof(ScriptVars, importantItemStatus) == 8260);
		static_assert(offsetof(ScriptVars, markingsOfPickable) == 8292);
		static_assert(offsetof(ScriptVars, missionCodeMarkingsOfPickable) == 8424);
		static_assert(offsetof(ScriptVars, markingsOfPlaced) == 8428);
		static_assert(offsetof(ScriptVars, missionCodeMarkingsOfPlaced) == 8560);
		static_assert(offsetof(ScriptVars, placedWeapons) == 8562);
		static_assert(offsetof(ScriptVars, placedOwnerIds) == 8626);
		static_assert(offsetof(ScriptVars, placedPos) == 8692);
		static_assert(offsetof(ScriptVars, placedRot) == 9076);
		static_assert(offsetof(ScriptVars, placedAttachTargetNames) == 9588);
		static_assert(offsetof(ScriptVars, placedAttachTargetIndex) == 9844);
		static_assert(offsetof(ScriptVars, placedAttachTargetGameObjectType) == 9908);
		static_assert(offsetof(ScriptVars, placedAttachPoints) == 9940);
		static_assert(offsetof(ScriptVars, placedCorrelationId) == 10196);
		static_assert(offsetof(ScriptVars, placedTargetGroupBlast) == 10260);
		static_assert(offsetof(ScriptVars, placedStatus) == 10324);
		static_assert(offsetof(ScriptVars, playerDecoyInstanceIndicies) == 10356);
		static_assert(offsetof(ScriptVars, playerDecoyAngleBallon) == 10380);
		static_assert(offsetof(ScriptVars, caputreAnimalId) == 10428);
		static_assert(offsetof(ScriptVars, captureAreaName) == 10460);
		static_assert(offsetof(ScriptVars, captureAnimalIndex) == 10524);
		static_assert(offsetof(ScriptVars, collectionPositionsMoved) == 10528);
		static_assert(offsetof(ScriptVars, collectionRotationsMoved) == 10912);
		static_assert(offsetof(ScriptVars, collectionLocatorIndicesMoved) == 11040);
		static_assert(offsetof(ScriptVars, collectionMovedIndexEnd) == 11104);
		static_assert(offsetof(ScriptVars, locationCodeOfCollectionMoved) == 11108);
		static_assert(offsetof(ScriptVars, cboxIsValid) == 11110);
		static_assert(offsetof(ScriptVars, cboxLife) == 11128);
		static_assert(offsetof(ScriptVars, cboxFlag) == 11192);
		static_assert(offsetof(ScriptVars, cboxPosterType) == 11224);
		static_assert(offsetof(ScriptVars, cboxEquipId) == 11240);
		static_assert(offsetof(ScriptVars, cboxLocation) == 11272);
		static_assert(offsetof(ScriptVars, buddyTypeMissionStart) == 11528);
		static_assert(offsetof(ScriptVars, obtainedBuddyTypeMissionStart) == 11529);
		static_assert(offsetof(ScriptVars, sortieBuddyTypeMissionStart) == 11530);
		static_assert(offsetof(ScriptVars, deadBuddyTypeMissionStart) == 11531);
		static_assert(offsetof(ScriptVars, initialBuddyStatusMissionStart) == 11532);
		static_assert(offsetof(ScriptVars, initialBuddyPosMissionStart) == 11536);
		static_assert(offsetof(ScriptVars, initialBuddyRotYMissionStart) == 11548);
		static_assert(offsetof(ScriptVars, buddyCommonFlagMissionStart) == 11552);
		static_assert(offsetof(ScriptVars, buddyFriendlyPointMissionStart) == 11556);
		static_assert(offsetof(ScriptVars, reliefVehicleCarryType) == 11572);
		static_assert(offsetof(ScriptVars, reliefVehicleCarryAttitude) == 11576);
		static_assert(offsetof(ScriptVars, reliefVehicleCarryLife) == 11592);
		static_assert(offsetof(ScriptVars, reliefVehicleCarryAmmo) == 11616);
		static_assert(offsetof(ScriptVars, reliefVehicleCarryState) == 11622);
		static_assert(offsetof(ScriptVars, buddyType) == 11624);
		static_assert(offsetof(ScriptVars, obtainedBuddyType) == 11625);
		static_assert(offsetof(ScriptVars, sortieBuddyType) == 11626);
		static_assert(offsetof(ScriptVars, disableCallBuddyType) == 11627);
		static_assert(offsetof(ScriptVars, deadBuddyType) == 11628);
		static_assert(offsetof(ScriptVars, initialBuddyStatus) == 11629);
		static_assert(offsetof(ScriptVars, initialBuddyPos) == 11632);
		static_assert(offsetof(ScriptVars, initialBuddyRotY) == 11644);
		static_assert(offsetof(ScriptVars, buddyCommonFlag) == 11648);
		static_assert(offsetof(ScriptVars, buddyMissionFlag) == 11652);
		static_assert(offsetof(ScriptVars, buddyCallCount) == 11656);
		static_assert(offsetof(ScriptVars, buddyEspionageCpNames) == 11684);
		static_assert(offsetof(ScriptVars, buddyFriendlyTimer) == 11716);
		static_assert(offsetof(ScriptVars, buddyFriendlyPoint) == 11748);
		static_assert(offsetof(ScriptVars, buddyFriendlyMaxPoint) == 11764);
		static_assert(offsetof(ScriptVars, buddyFriendlyCount) == 11780);
		static_assert(offsetof(ScriptVars, buddyQuietEquipType) == 11844);
		static_assert(offsetof(ScriptVars, buddyQuietCostumeType) == 11846);
		static_assert(offsetof(ScriptVars, buddyDogFlag) == 11847);
		static_assert(offsetof(ScriptVars, buddyDogMode) == 11848);
		static_assert(offsetof(ScriptVars, buddyDogEquipType) == 11849);
		static_assert(offsetof(ScriptVars, buddyDogEyeType) == 11850);
		static_assert(offsetof(ScriptVars, buddyDogColorType) == 11851);
		static_assert(offsetof(ScriptVars, buddyGearAmmoCount) == 11852);
		static_assert(offsetof(ScriptVars, buddyGearStockAmmoCount) == 11856);
		static_assert(offsetof(ScriptVars, buddyGearBodyGrade) == 11860);
		static_assert(offsetof(ScriptVars, buddyGearEquipGrade) == 11861);
		static_assert(offsetof(ScriptVars, buddyGearReloadGrade) == 11862);
		static_assert(offsetof(ScriptVars, buddyGearHeadType) == 11863);
		static_assert(offsetof(ScriptVars, buddyGearHeadGrade) == 11864);
		static_assert(offsetof(ScriptVars, buddyGearArmType) == 11865);
		static_assert(offsetof(ScriptVars, buddyGearArmGrade) == 11866);
		static_assert(offsetof(ScriptVars, buddyGearMainWPType) == 11867);
		static_assert(offsetof(ScriptVars, buddyGearMainWPGrade) == 11868);
		static_assert(offsetof(ScriptVars, buddyGearSubWPType) == 11869);
		static_assert(offsetof(ScriptVars, buddyGearSubWPGrade) == 11870);
		static_assert(offsetof(ScriptVars, buddyGearColoring0) == 11871);
		static_assert(offsetof(ScriptVars, buddyGearColoring1) == 11872);
		static_assert(offsetof(ScriptVars, buddyHorseType) == 11873);
		static_assert(offsetof(ScriptVars, buddyHorseColorType) == 11874);
		static_assert(offsetof(ScriptVars, buddyHorseHeadDesignType) == 11875);
		static_assert(offsetof(ScriptVars, buddyHorseFootDesignType) == 11876);
		static_assert(offsetof(ScriptVars, battleGearAmmoCountMain) == 11878);
		static_assert(offsetof(ScriptVars, battleGearAmmoCountSub) == 11882);
		static_assert(offsetof(ScriptVars, battleGearStockAmmoCountMain) == 11884);
		static_assert(offsetof(ScriptVars, battleGearStockAmmoCountSub) == 11888);
		static_assert(offsetof(ScriptVars, battleGearMainWPType) == 11890);
		static_assert(offsetof(ScriptVars, battleGearMainWPGrade) == 11892);
		static_assert(offsetof(ScriptVars, battleGearSubWPType) == 11894);
		static_assert(offsetof(ScriptVars, battleGearSubWPGrade) == 11895);
		static_assert(offsetof(ScriptVars, battleGearColoring0) == 11896);
		static_assert(offsetof(ScriptVars, battleGearColoring1) == 11897);
		static_assert(offsetof(ScriptVars, maxPhase) == 11898);
		static_assert(offsetof(ScriptVars, playerPhase) == 11899);
		static_assert(offsetof(ScriptVars, sahelanBrokenMesh) == 11900);
		static_assert(offsetof(ScriptVars, supportHeliCustomize) == 11904);
		static_assert(offsetof(ScriptVars, pfRatingPoint) == 11908);
		static_assert(offsetof(ScriptVars, mbmTppGmp) == 11912);
		static_assert(offsetof(ScriptVars, mbmServerWalletGmp) == 11916);
		static_assert(offsetof(ScriptVars, mbmTppHeroicPoint) == 11920);
		static_assert(offsetof(ScriptVars, missionHeroicPoint) == 11924);
		static_assert(offsetof(ScriptVars, mbmTppOgrePoint) == 11928);
		static_assert(offsetof(ScriptVars, missionOgrePoint) == 11932);
		static_assert(offsetof(ScriptVars, mbmRequestMisogi) == 11936);
		static_assert(offsetof(ScriptVars, mbmTppTotalIncomeGmp) == 11940);
		static_assert(offsetof(ScriptVars, mbmIsOpenedCombatSection) == 11944);
		static_assert(offsetof(ScriptVars, mbmIsOpenedBaseDevSection) == 11945);
		static_assert(offsetof(ScriptVars, mbmIsOpenedSpySection) == 11946);
		static_assert(offsetof(ScriptVars, mbmIsOpenedMedicalSection) == 11947);
		static_assert(offsetof(ScriptVars, mbmIsOpenedSecuritySection) == 11948);
		static_assert(offsetof(ScriptVars, mbmIsOpenedHospitalSection) == 11949);
		static_assert(offsetof(ScriptVars, mbmIsOpenedPrisonSection) == 11950);
		static_assert(offsetof(ScriptVars, mbmIsOpenedSeparationSection) == 11951);
		static_assert(offsetof(ScriptVars, mbmSectionFuncSvars) == 11952);
		static_assert(offsetof(ScriptVars, mbmAutoGmpElapsedTimeMinuteSvars) == 11994);
		static_assert(offsetof(ScriptVars, mbmAutoResourceElapsedTimeMinuteSvars) == 11995);
		static_assert(offsetof(ScriptVars, mbmCommonMetalProcessingElapsedTimeMinute) == 11996);
		static_assert(offsetof(ScriptVars, mbmMinorMetalProcessingElapsedTimeMinute) == 11997);
		static_assert(offsetof(ScriptVars, mbmPreciousMetalProcessingElapsedTimeMinute) == 11998);
		static_assert(offsetof(ScriptVars, mbmFuelResourceProcessingElapsedTimeMinute) == 11999);
		static_assert(offsetof(ScriptVars, mbmBioticResourceProcessingElapsedTimeMinute) == 12000);
		static_assert(offsetof(ScriptVars, isRussianTranslatable) == 12001);
		static_assert(offsetof(ScriptVars, isPashtoTranslatable) == 12002);
		static_assert(offsetof(ScriptVars, isAfrikaansTranslatable) == 12003);
		static_assert(offsetof(ScriptVars, isKikongoTranslatable) == 12004);
		static_assert(offsetof(ScriptVars, mbmTreatmentElapsedTimeMinute) == 12005);
		static_assert(offsetof(ScriptVars, mbmBecomeCombatSectionLvMax) == 12006);
		static_assert(offsetof(ScriptVars, mbmBecomeDevelopSectionLvMax) == 12007);
		static_assert(offsetof(ScriptVars, mbmBecomeBaseDevSectionLvMax) == 12008);
		static_assert(offsetof(ScriptVars, mbmBecomeSupportSectionLvMax) == 12009);
		static_assert(offsetof(ScriptVars, mbmBecomeSpySectionLvMax) == 12010);
		static_assert(offsetof(ScriptVars, mbmBecomeMedicalSectionLvMax) == 12011);
		static_assert(offsetof(ScriptVars, mbmBecomeSecuritySectionLvMax) == 12012);
		static_assert(offsetof(ScriptVars, mbmBecomeAllSectionLvMax) == 12013);
		static_assert(offsetof(ScriptVars, mbmHighestTotalSectionLv) == 12014);
		static_assert(offsetof(ScriptVars, mbmStaffSyncVersion) == 12016);
		static_assert(offsetof(ScriptVars, mbmStaffSvarsHeaders) == 12020);
		static_assert(offsetof(ScriptVars, mbmStaffSvarsSeeds) == 26020);
		static_assert(offsetof(ScriptVars, mbmStaffSvarsStatusesSync) == 40020);
		static_assert(offsetof(ScriptVars, mbmStaffSvarsStatusesNoSync) == 54020);
		static_assert(offsetof(ScriptVars, mbmStaffSkillGots) == 61020);
		static_assert(offsetof(ScriptVars, mbmStaffSortKeysSvars) == 61107);
		static_assert(offsetof(ScriptVars, mbmStaffSortDirsSvars) == 61120);
		static_assert(offsetof(ScriptVars, mbmStaffListMainListModeSvars) == 61133);
		static_assert(offsetof(ScriptVars, mbmStaffListAssignListModeSvars) == 61134);
		static_assert(offsetof(ScriptVars, mbmEnmityElapsedTimeMinuteSvars) == 61135);
		static_assert(offsetof(ScriptVars, mbmMoraleElapsedTimeMinuteSvars) == 61136);
		static_assert(offsetof(ScriptVars, mbmLanguageElapsedTimeMinuteSvars) == 61137);
		static_assert(offsetof(ScriptVars, mbmEnableStaffInitLangKikongo) == 61138);
		static_assert(offsetof(ScriptVars, mbmPandemicEventMode) == 61139);
		static_assert(offsetof(ScriptVars, mbmPandemicElapsedTimeMinuteSvars) == 61140);
		static_assert(offsetof(ScriptVars, mbmPandemicSpreadAttackLimitCount) == 61142);
		static_assert(offsetof(ScriptVars, mbmPandemicSpreadAttackCount) == 61144);
		static_assert(offsetof(ScriptVars, mbmDisableKikongoFirst) == 61146);
		static_assert(offsetof(ScriptVars, mbmMasterGunsmithSkill) == 61147);
		static_assert(offsetof(ScriptVars, mbmBipedalismWeaponDevelopmentSkill) == 61148);
		static_assert(offsetof(ScriptVars, mbmBanHeuy) == 61149);
		static_assert(offsetof(ScriptVars, mbmAssignableMiller) == 61150);
		static_assert(offsetof(ScriptVars, mbmLockedTanFlag) == 61151);
		static_assert(offsetof(ScriptVars, mbmStaffInitEnmityLv) == 61152);
		static_assert(offsetof(ScriptVars, mbmEquipParamStatus) == 61154);
		static_assert(offsetof(ScriptVars, mbmOnlineOffsetEquipParamStatus) == 62178);
		static_assert(offsetof(ScriptVars, mbmEquipDevelopIndices) == 63202);
		static_assert(offsetof(ScriptVars, mbmEquipDevelopTimes) == 63224);
		static_assert(offsetof(ScriptVars, mbmWeaponPartsReceiverStatus) == 63264);
		static_assert(offsetof(ScriptVars, mbmWeaponPartsBarrelStatus) == 63498);
		static_assert(offsetof(ScriptVars, mbmWeaponPartsMagazineStatus) == 63613);
		static_assert(offsetof(ScriptVars, mbmWeaponPartsStockStatus) == 63805);
		static_assert(offsetof(ScriptVars, mbmWeaponPartsMuzzleStatus) == 63848);
		static_assert(offsetof(ScriptVars, mbmWeaponPartsMuzzleOptionStatus) == 63877);
		static_assert(offsetof(ScriptVars, mbmWeaponPartsSightStatus) == 63917);
		static_assert(offsetof(ScriptVars, mbmWeaponPartsUnderBarrelStatus) == 63942);
		static_assert(offsetof(ScriptVars, mbmWeaponPartsLightStatus) == 63965);
		static_assert(offsetof(ScriptVars, mbmServerDevelopCount) == 63976);
		static_assert(offsetof(ScriptVars, mbmServerDevelopIds) == 63978);
		static_assert(offsetof(ScriptVars, mbmServerDevelopUsageGmpCosts) == 64618);
		static_assert(offsetof(ScriptVars, mbmServerDevelopUsageResourceType1s) == 65258);
		static_assert(offsetof(ScriptVars, mbmServerDevelopUsageResourceType1Counts) == 65578);
		static_assert(offsetof(ScriptVars, mbmServerDevelopUsageResourceType2s) == 66218);
		static_assert(offsetof(ScriptVars, mbmServerDevelopUsageResourceType2Counts) == 66538);
		static_assert(offsetof(ScriptVars, mbmFobSvars) == 67180);
		static_assert(offsetof(ScriptVars, mbmClusterSvars) == 67196);
		static_assert(offsetof(ScriptVars, mbmClusterTimeMinutes) == 67268);
		static_assert(offsetof(ScriptVars, mbmCompMb) == 67408);
		static_assert(offsetof(ScriptVars, doesNotRestoreCommandClusterGradeFromLayoutCode) == 67409);
		static_assert(offsetof(ScriptVars, mbmSecurityClusterSvars) == 67412);
		static_assert(offsetof(ScriptVars, mbmSecuritySpecialPlatformSoldierCounts) == 67552);
		static_assert(offsetof(ScriptVars, mbmSecuritySpecialPlatformIrSensorCounts) == 67587);
		static_assert(offsetof(ScriptVars, mbmSecuritySpecialPlatformAntitheftCounts) == 67622);
		static_assert(offsetof(ScriptVars, mbmSecuritySpecialPlatformCameraCounts) == 67657);
		static_assert(offsetof(ScriptVars, mbmSecuritySpecialPlatformDecoyCounts) == 67692);
		static_assert(offsetof(ScriptVars, mbmSecuritySpecialPlatformMineCounts) == 67727);
		static_assert(offsetof(ScriptVars, mbmSecuritySpecialPlatformUavCounts) == 67762);
		static_assert(offsetof(ScriptVars, mbmSecuritySpecialPlatformCautionAreaSvars) == 67800);
		static_assert(offsetof(ScriptVars, mbmSecuritySpecialPlatformPlacedFreeMineCounts) == 67940);
		static_assert(offsetof(ScriptVars, mbmSecuritySpecialPlatformPlacedFreeCameraCounts) == 67975);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform1SoldierCounts) == 68010);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform1IrSensorCounts) == 68045);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform1AntitheftCounts) == 68080);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform1CameraCounts) == 68115);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform1DecoyCounts) == 68150);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform1MineCounts) == 68185);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform1UavCounts) == 68220);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform1CautionAreaSvars) == 68256);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform1PlacedFreeMineCounts) == 68396);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform1PlacedFreeCameraCounts) == 68431);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform2SoldierCounts) == 68466);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform2IrSensorCounts) == 68501);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform2AntitheftCounts) == 68536);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform2CameraCounts) == 68571);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform2DecoyCounts) == 68606);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform2MineCounts) == 68641);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform2UavCounts) == 68676);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform2CautionAreaSvars) == 68712);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform2PlacedFreeMineCounts) == 68852);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform2PlacedFreeCameraCounts) == 68887);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform3SoldierCounts) == 68922);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform3IrSensorCounts) == 68957);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform3AntitheftCounts) == 68992);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform3CameraCounts) == 69027);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform3DecoyCounts) == 69062);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform3MineCounts) == 69097);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform3UavCounts) == 69132);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform3CautionAreaSvars) == 69168);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform3PlacedFreeMineCounts) == 69308);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform3PlacedFreeCameraCounts) == 69343);
		static_assert(offsetof(ScriptVars, mbmSecuritySpecialPlatformSvars) == 69380);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform1Svars) == 69520);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform2Svars) == 69660);
		static_assert(offsetof(ScriptVars, mbmSecurityCommonPlatform3Svars) == 69800);
		static_assert(offsetof(ScriptVars, mbmInitializedServerWalletFlagDay1) == 69940);
		static_assert(offsetof(ScriptVars, mbmIsInitializedDay50) == 69941);
		static_assert(offsetof(ScriptVars, mbmNuclearWeaponLocalCountPatchingDay50) == 69944);
		static_assert(offsetof(ScriptVars, mbmNuclearSynchronizedDay50) == 69948);
		static_assert(offsetof(ScriptVars, mbmResourceSyncVersion) == 69952);
		static_assert(offsetof(ScriptVars, mbmResourceSvars1Fix) == 69956);
		static_assert(offsetof(ScriptVars, mbmResourceSvars2Fix) == 70192);
		static_assert(offsetof(ScriptVars, mbmResourceSvars1Diff) == 70428);
		static_assert(offsetof(ScriptVars, mbmResourceSvars2Diff) == 70664);
		static_assert(offsetof(ScriptVars, mbmResourceCompensateCounts) == 70900);
		static_assert(offsetof(ScriptVars, mbmWhiteContainerCountsSvarsFix) == 71136);
		static_assert(offsetof(ScriptVars, mbmWhiteContainerCountsSvarsDiff) == 71156);
		static_assert(offsetof(ScriptVars, mbmRedContainerCountsSvarsFix) == 71176);
		static_assert(offsetof(ScriptVars, mbmRedContainerCountsSvarsDiff) == 71196);
		static_assert(offsetof(ScriptVars, mbmOpenedWalkerGear) == 71216);
		static_assert(offsetof(ScriptVars, mbmDataBaseSvars) == 71217);
		static_assert(offsetof(ScriptVars, mbmCompAnimal) == 71676);
		static_assert(offsetof(ScriptVars, mbmCompDesign) == 71677);
		static_assert(offsetof(ScriptVars, mbmCompPhoto) == 71678);
		static_assert(offsetof(ScriptVars, mbmDeployMissionsSvars) == 71680);
		static_assert(offsetof(ScriptVars, mbmDeployMissionAveRanksSvars) == 71964);
		static_assert(offsetof(ScriptVars, mbmDeployTeamCombatStaffCounts) == 72036);
		static_assert(offsetof(ScriptVars, mbmDeployTeamCombatStaffRankTops) == 72058);
		static_assert(offsetof(ScriptVars, mbmDeployTeamCombatStaffRankBottoms) == 72069);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSubStaffCounts) == 72080);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSubStaffRankTops) == 72102);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSubStaffRankBottoms) == 72113);
		static_assert(offsetof(ScriptVars, mbmDeployTeamCombatPoints) == 72124);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSubPoints) == 72168);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsTimeMinute) == 72212);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvars4wdEastCount) == 72256);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvars4wdWestCount) == 72278);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsTruckEastCount) == 72300);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsTruckWestCount) == 72322);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsArmoredVehicleEastCount) == 72344);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsArmoredVehicleWestCount) == 72366);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsArmoredVehicleEastRocketCount) == 72388);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsWheeledArmoredVehicleWestCount) == 72410);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsTankEastCount) == 72432);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsTankWestCount) == 72454);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsWalkerGearProtoHeuyCount) == 72476);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsWalkerGearSovietBattleCount) == 72498);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsWalkerGearSovietSupportCount) == 72520);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsWalkerGearCfaBattleCount) == 72542);
		static_assert(offsetof(ScriptVars, mbmDeployTeamSvarsWalkerGearCfaSupportCount) == 72564);
		static_assert(offsetof(ScriptVars, mbmDeployRandomMissionSeedSvars) == 72588);
		static_assert(offsetof(ScriptVars, mbmDeploySequentialMissionIdSvars) == 72592);
		static_assert(offsetof(ScriptVars, mbmDeploySequentialMissionIdLimitSvars) == 72594);
		static_assert(offsetof(ScriptVars, mbmClearedDeployRevengeMissionFlags) == 72596);
		static_assert(offsetof(ScriptVars, mbmDeployableBattleGear) == 72610);
		static_assert(offsetof(ScriptVars, mbmBattleGearDeployingTeam) == 72611);
		static_assert(offsetof(ScriptVars, mbmAddDeployLogIndex) == 72612);
		static_assert(offsetof(ScriptVars, mbmCompSequential) == 72613);
		static_assert(offsetof(ScriptVars, mbmNewestNRandomMissionId) == 72614);
		static_assert(offsetof(ScriptVars, mbmNewestRRandomMissionId) == 72616);
		static_assert(offsetof(ScriptVars, mbmOpenedRRandomMission) == 72618);
		static_assert(offsetof(ScriptVars, mbmNRandomMissionClearedCount) == 72619);
		static_assert(offsetof(ScriptVars, mbmIsCancelAllDeploy) == 72620);
		static_assert(offsetof(ScriptVars, mbmClearedCountOfflineDeploy) == 72624);
		static_assert(offsetof(ScriptVars, mbmClearedCountOnlineDeploy) == 72628);
		static_assert(offsetof(ScriptVars, mbmPoolRewardsSyncVersion) == 72632);
		static_assert(offsetof(ScriptVars, mbmPoolRewardsSvars) == 72636);
		static_assert(offsetof(ScriptVars, mbmPoolRewardSourceIds) == 73436);
		static_assert(offsetof(ScriptVars, mbmPoolRewardSeeds) == 74236);
		static_assert(offsetof(ScriptVars, mbmPoolRewardValues) == 74636);
		static_assert(offsetof(ScriptVars, mbmS10081CaptiveStaffSvarsSeed) == 75436);
		static_assert(offsetof(ScriptVars, mbmS10081CaptiveStaffSvarsHeader) == 75440);
		static_assert(offsetof(ScriptVars, mbmS10081CaptiveStaffSvarsStatusSync) == 75444);
		static_assert(offsetof(ScriptVars, mbmS10081CaptiveStaffSvarsStatusNoSync) == 75448);
		static_assert(offsetof(ScriptVars, mbmRemoverSvarsHeaders) == 75452);
		static_assert(offsetof(ScriptVars, mbmRemoverSvarsSeeds) == 75708);
		static_assert(offsetof(ScriptVars, mbmRemoverSvarsStatusesSync) == 75964);
		static_assert(offsetof(ScriptVars, mbmRemoverSvarsStatusesNoSync) == 76220);
		static_assert(offsetof(ScriptVars, mbmRemoverReasons) == 76348);
		static_assert(offsetof(ScriptVars, mbmAddRemoverIndexSvars) == 76412);
		static_assert(offsetof(ScriptVars, mbmDeployRewardSyncVersion) == 76416);
		static_assert(offsetof(ScriptVars, mbmSecuritySystemLevelsDeveloped) == 76420);
		static_assert(offsetof(ScriptVars, mbmAnimalHabitatAfghSvars) == 76436);
		static_assert(offsetof(ScriptVars, mbmAnimalHabitatMafrSvars) == 76500);
		static_assert(offsetof(ScriptVars, mbmIsOccupation) == 76564);
		static_assert(offsetof(ScriptVars, mbmRequestDemoAttackedFromOtherPlayer) == 76565);
		static_assert(offsetof(ScriptVars, mbmDemoAttackedFromOtherPlayerKnowWhereFrom) == 76566);
		static_assert(offsetof(ScriptVars, mbmFobDefenseWinCount) == 76568);
		static_assert(offsetof(ScriptVars, mbmFobDefenseLoseCount) == 76572);
		static_assert(offsetof(ScriptVars, mbmIsContractOutDisplayed) == 76576);
		static_assert(offsetof(ScriptVars, mbmIsEnableNuclearDevelop) == 76577);
		static_assert(offsetof(ScriptVars, mbmIsNuclearDeveloped) == 76578);
		static_assert(offsetof(ScriptVars, mbmIsNuclearDiscarded) == 76579);
		static_assert(offsetof(ScriptVars, mbmIsNuclearDeveloping) == 76580);
		static_assert(offsetof(ScriptVars, mbmNuclearDevelopTime) == 76584);
		static_assert(offsetof(ScriptVars, mbmNuclearDevelopMaxTime) == 76588);
		static_assert(offsetof(ScriptVars, mbmNuclearDevelopStartTime) == 76592);
		static_assert(offsetof(ScriptVars, mbmPdTotalAddedHostageStaffCount) == 76596);
		static_assert(offsetof(ScriptVars, mbmPdTotalAddedSoldierStaffCount) == 76600);
		static_assert(offsetof(ScriptVars, mbmPdTotalAddedVolunteerStaffCount) == 76604);
		static_assert(offsetof(ScriptVars, mbmPdTotalDeadStaffCount) == 76608);
		static_assert(offsetof(ScriptVars, mbmPdTotalDefectionCount) == 76612);
		static_assert(offsetof(ScriptVars, mbmPdTotalFireCount) == 76616);
		static_assert(offsetof(ScriptVars, mbmPdTotalAddedGmpT) == 76620);
		static_assert(offsetof(ScriptVars, mbmPdTotalAddedGmp) == 76624);
		static_assert(offsetof(ScriptVars, mbmPdTotalSubtractedGmpT) == 76628);
		static_assert(offsetof(ScriptVars, mbmPdTotalSubtractedGmp) == 76632);
		static_assert(offsetof(ScriptVars, mbmPdTotalAddedHeroicPoint) == 76636);
		static_assert(offsetof(ScriptVars, mbmPdTotalSubtractedHeroicPoint) == 76640);
		static_assert(offsetof(ScriptVars, mbmPdTotalAddedResourceCounts) == 76644);
		static_assert(offsetof(ScriptVars, mbmPdTotalAddedAnimalCounts) == 76880);
		static_assert(offsetof(ScriptVars, mbmCurrentNamePlateId) == 77276);
		static_assert(offsetof(ScriptVars, mbmNamePlateGotFlags) == 77280);
		static_assert(offsetof(ScriptVars, mbmNamePlateNewFlags) == 77288);
		static_assert(offsetof(ScriptVars, mbmHeroThreshold) == 77296);
		static_assert(offsetof(ScriptVars, mbmNotHeroThreshold) == 77300);
		static_assert(offsetof(ScriptVars, mbmPfRatingSaveSeason) == 77304);
		static_assert(offsetof(ScriptVars, mbmPfRatingReadAttackMatchFlag) == 77306);
		static_assert(offsetof(ScriptVars, mbmPfRatingReadDefenseMatchFlag) == 77308);
		static_assert(offsetof(ScriptVars, mbmPfRatingMatchSections) == 77310);
		static_assert(offsetof(ScriptVars, mbmPfRatingRankingSaveSection) == 77325);
		static_assert(offsetof(ScriptVars, mbmPfRatingRankingPlayerRanks) == 77326);
		static_assert(offsetof(ScriptVars, mbmPfRatingRankingPlayerRankDiffs) == 77342);
		static_assert(offsetof(ScriptVars, mbmPfRatingRankingPlayerIds) == 77360);
		static_assert(offsetof(ScriptVars, mbmPfRatingCommonFlag) == 77424);
		static_assert(offsetof(ScriptVars, mbmPfRatingShortSaveSeason) == 77426);
		static_assert(offsetof(ScriptVars, mbmPfRatingShortReadAttackMatchFlag) == 77428);
		static_assert(offsetof(ScriptVars, mbmPfRatingShortReadDefenseMatchFlag) == 77430);
		static_assert(offsetof(ScriptVars, mbmPfRatingShortMatchSections) == 77432);
		static_assert(offsetof(ScriptVars, mbmPfRatingShortRankingSaveSection) == 77447);
		static_assert(offsetof(ScriptVars, mbmPfRatingShortRankingPlayerRanks) == 77448);
		static_assert(offsetof(ScriptVars, mbmPfRatingShortRankingPlayerRankDiffs) == 77464);
		static_assert(offsetof(ScriptVars, mbmPfRatingShortRankingPlayerIds) == 77480);
		static_assert(offsetof(ScriptVars, mbmChallengeTaskCompleteFlags) == 77544);
		static_assert(offsetof(ScriptVars, mbmChallengeTaskRewardReceiveFlags) == 77604);
		static_assert(offsetof(ScriptVars, mbmChallengeTaskOnlineVersionWindowOpen) == 77664);
		static_assert(offsetof(ScriptVars, mbmServerRankingRecordedFlags) == 77668);
		static_assert(offsetof(ScriptVars, mbmServerRankingScores) == 77776);
		static_assert(offsetof(ScriptVars, mbmBaseColorGotFlag) == 78200);
		static_assert(offsetof(ScriptVars, mbmBaseColorNewFlag) == 78204);
		static_assert(offsetof(ScriptVars, mbmDlcWeaponColorGotFlag) == 78208);
		static_assert(offsetof(ScriptVars, mbmDlcWeaponColorGotFlag2) == 78212);
		static_assert(offsetof(ScriptVars, mbmDlcWeaponColorNewFlag) == 78216);
		static_assert(offsetof(ScriptVars, mbmDlcWeaponColorNewFlag2) == 78220);
		static_assert(offsetof(ScriptVars, mbmIsVisitedFobDeployWelcomeMessage1) == 78224);
		static_assert(offsetof(ScriptVars, mbmIsVisitedFobDeployWelcomeMessage2) == 78225);
		static_assert(offsetof(ScriptVars, mbmFobDeployCheckBoxes) == 78226);
		static_assert(offsetof(ScriptVars, mbmFobDeployGradeSelectorIndexes) == 78232);
		static_assert(offsetof(ScriptVars, optionSelectedIndices) == 78238);
		static_assert(offsetof(ScriptVars, armRocketYIsReverse) == 78270);
		static_assert(offsetof(ScriptVars, armRocketXIsReverse) == 78271);
		static_assert(offsetof(ScriptVars, bgmVolume) == 78272);
		static_assert(offsetof(ScriptVars, keyStr) == 78276);
		static_assert(offsetof(ScriptVars, buttonNo) == 78756);
		static_assert(offsetof(ScriptVars, keybordNo) == 79236);
		static_assert(offsetof(ScriptVars, pushToTalk) == 79716);
		static_assert(offsetof(ScriptVars, checkPointGimmickIndelibleCount) == 79717);
		static_assert(offsetof(ScriptVars, checkPointGimmickIndelibleName) == 79720);
		static_assert(offsetof(ScriptVars, checkPointGimmickIndelibleState) == 79976);
		static_assert(offsetof(ScriptVars, missionStartGimmickIndelibleCount) == 80040);
		static_assert(offsetof(ScriptVars, missionStartGimmickIndelibleName) == 80044);
		static_assert(offsetof(ScriptVars, missionStartGimmickIndelibleState) == 80300);
		static_assert(offsetof(ScriptVars, checkPointGimmickPermanentGimmickSaveData) == 80364);
		static_assert(offsetof(ScriptVars, missionStartGimmickPermanentGimmickSaveData) == 82956);
		static_assert(offsetof(ScriptVars, cloakEnabled) == 85549);
		static_assert(offsetof(ScriptVars, supCboxSaveFormVersion) == 85551);
		static_assert(offsetof(ScriptVars, supCboxPhaseForSave) == 85552);
		static_assert(offsetof(ScriptVars, supCboxDropType) == 85553);
		static_assert(offsetof(ScriptVars, supCboxEquipId) == 85556);
		static_assert(offsetof(ScriptVars, supCboxPartsType) == 85560);
		static_assert(offsetof(ScriptVars, supCboxPos) == 85564);
		static_assert(offsetof(ScriptVars, supCboxRot) == 85576);
		static_assert(offsetof(ScriptVars, supCboxLevelWait) == 85592);
		static_assert(offsetof(ScriptVars, supCboxAdvancedRate) == 85596);
		static_assert(offsetof(ScriptVars, supCboxFallHeight) == 85600);
		static_assert(offsetof(ScriptVars, supCboxLevels) == 85604);
		static_assert(offsetof(ScriptVars, supCboxFlags) == 85617);
		static_assert(offsetof(ScriptVars, supCboxU8buf) == 85618);
		static_assert(offsetof(ScriptVars, supCboxU16buf) == 85734);
		static_assert(offsetof(ScriptVars, supCboxU32buf) == 85808);
		static_assert(offsetof(ScriptVars, supCboxLocationOnSave) == 85876);
		static_assert(offsetof(ScriptVars, supAttackStateForSave) == 85878);
		static_assert(offsetof(ScriptVars, supAttackAttackType) == 85879);
		static_assert(offsetof(ScriptVars, supAttackPos) == 85880);
		static_assert(offsetof(ScriptVars, supAttackRot) == 85892);
		static_assert(offsetof(ScriptVars, supAttackLevelWait) == 85908);
		static_assert(offsetof(ScriptVars, supAttackLevelAttack) == 85909);
		static_assert(offsetof(ScriptVars, supAttackFlags) == 85910);
		static_assert(offsetof(ScriptVars, supAttackCommonValue) == 85912);
		static_assert(offsetof(ScriptVars, supAttackWaitTime) == 85916);
		static_assert(offsetof(ScriptVars, supAttackLocationOnSave) == 85920);
		static_assert(offsetof(ScriptVars, stealthAssistLeftCount) == 85922);
		static_assert(offsetof(ScriptVars, tppTransferFlag) == 85924);
		static_assert(offsetof(ScriptVars, tppTransferAcquiredFlag) == 85928);
		static_assert(offsetof(ScriptVars, dlcPermittedFlag) == 85932);
		static_assert(offsetof(ScriptVars, dlcAcquiredFlag) == 85940);
		static_assert(offsetof(ScriptVars, mbDvcTutorialFlag) == 85948);
		static_assert(offsetof(ScriptVars, mbDvcTopMenuFlag) == 85952);
		static_assert(offsetof(ScriptVars, heliMusicType) == 85956);
		static_assert(offsetof(ScriptVars, heliMusicTrackID) == 85960);
		static_assert(offsetof(ScriptVars, radioReadFlagGlobalScoped) == 85964);
		static_assert(offsetof(ScriptVars, markerLocatorInfos) == 86764);
		static_assert(offsetof(ScriptVars, tipsReadFlag) == 87164);
		static_assert(offsetof(ScriptVars, userMarkerPosX) == 87364);
		static_assert(offsetof(ScriptVars, userMarkerPosY) == 87384);
		static_assert(offsetof(ScriptVars, userMarkerPosZ) == 87404);
		static_assert(offsetof(ScriptVars, userMarkerAddFlag) == 87424);
		static_assert(offsetof(ScriptVars, userMarkerGameObjId) == 87434);
		static_assert(offsetof(ScriptVars, userMarkerLocationId) == 87444);
		static_assert(offsetof(ScriptVars, userMarkerSaveCount) == 87446);
		static_assert(offsetof(ScriptVars, fobResultRankingDateTime) == 87448);
		static_assert(offsetof(ScriptVars, fobResultRankingPrevRanks) == 87452);
		static_assert(offsetof(ScriptVars, fobEventResultRankingDateTime) == 87468);
		static_assert(offsetof(ScriptVars, fobEventResultRankingPrevRanks) == 87472);
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
					void* (__fastcall* GetEquipIdSlot)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int, int, unsigned int);
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
					void* (__fastcall* GetEquipIdSlot)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int, int, unsigned int);
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
					int id;
					int mask;
				};

				struct vtable
				{

				};

				vtable* __vftable;
				char __pad0[32];
				int mask;
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

	namespace gn
	{
		struct swapchain
		{
			char __pad0[24];
			IDXGISwapChain* swapChain;
		};

	}

	namespace fox
	{
		struct ApplicationSystem
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
			void* uiSystem;
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
			void* __system_24;
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
				void(__fastcall* meth_20)(Ruleset_unk1*);
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

		struct Ruleset
		{
			char __pad0[88];
			Ruleset_unk1 unk1;
			char __pad1[64];
			int numTeams;
			TeamInfo** teams;
			char __pad2[160];
			int a1;
			char __pad3[88];
			int state;
			char __pad4[60];
			unsigned char currentRound;
			char __pad5[9];
			unsigned char localPlayerSessionIndex;
			char __pad6[961];
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

#pragma pack(push, 1)
	struct mgo_match_t
	{
		char __pad0[13];
		char is_joining_invite;
		steam_id invite_lobby_id;
		char __pad1[2];
		match_settings_t match_settings;
		char __pad2[48];
		short lobby_member_limit;
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
		char __pad7[1604];
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
