#pragma once
#include <d3d11.h>

extern "C"
{
#include "lapi.h"
}

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

	struct /*VFT*/ ISteamFriends_vtbl
	{
		const char* (__fastcall* GetPersonaName)(ISteamFriends* this_);
		unsigned __int64(__fastcall* SetPersonaName)(ISteamFriends* this_, const char* pchPersonaName);
		int(__fastcall* GetPersonaState)(ISteamFriends* this_);
		int(__fastcall* GetFriendCount)(ISteamFriends* this_, int eFriendFlags);
		steam_id(__fastcall* GetFriendByIndex)(ISteamFriends* this_, int iFriend, int iFriendFlags);
		int(__fastcall* GetFriendRelationship)(ISteamFriends* this_, steam_id steamIDFriend);
		int(__fastcall* GetFriendPersonaState)(ISteamFriends* this_, steam_id steamIDFriend);
		const char* (__fastcall* GetFriendPersonaName)(ISteamFriends* this_, steam_id steamIDFriend);
		bool(__fastcall* GetFriendGamePlayed)(ISteamFriends* this_, steam_id steamIDFriend, void* pFriendGameInfo);
		const char* (__fastcall* GetFriendPersonaNameHistory)(ISteamFriends* this_, steam_id steamIDFriend, int iPersonaName);
		const char* (__fastcall* GetPlayerNickname)(ISteamFriends* this_, steam_id steamIDFriend);
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
		steam_id(__fastcall* GetLobbyByIndex)(ISteamMatchmaking* this_, int iLobby);
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
		steam_id(__fastcall* GetLobbyOwner)(ISteamMatchmaking* this_, steam_id steamIDLobby);
		bool(__fastcall* SetLobbyOwner)(ISteamMatchmaking* this_, steam_id steamIDLobby, steam_id steamIDNewOwner);
		bool(__fastcall* SetLinkedLobby)(ISteamMatchmaking* this_, steam_id steamIDLobby, steam_id steamIDLobby2);
	};

	struct ISteamMatchmaking
	{
		ISteamMatchmaking_vtbl* __vftable /*VFT*/;
	};

	namespace tpp::ui::menu
	{
		struct UiCommonDataManager;
	}

	namespace Vectormath::Aos
	{
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
			void* a2;
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
			lua_State* state;
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

		namespace gr
		{
			namespace dg
			{
				struct StringFontMetricsCache
				{
					__int16 a1;
					__int16 count;
					int a3;
					__int16* buffer;
				};
			}

			struct Packet2DString
			{
				int a1;
				int a2;
				fox::gr::dg::StringFontMetricsCache* fontMetricsCache;
				__int16 a3;
				__int16 a4;
				__int16 a5;
				__int16 flags;
				int a7;
				__int16 a8;
				__int16 a9;
			};

			struct Packet2DBuffer
			{
				void* a1;
				void* a2;
				void* packet;
				void* a4;
				int packetSize;
				int flags;
				int a6;
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
				ModelNodeText_vtbl* __vftable;
				char __pad0[106];
				char f0;
				char __pad1[21];
				void* f1;
				short flags;
				char f3;
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
				short f13;
				char f14;
				char f15;
				short f16;
				char __pad4[2];
				void* f17;
				void* f18;
				__m128 f19;
				void* f20;
				int f21;
				char __pad5[4];
				void* f22;
				void* f23;
				int f24;
				char __pad6[4];
				void* f25;
				void* f26;
				TextUnit* textUnit;
				int f28;
				char __pad7[4];
				void* f29;
				void* f30;
				void* f31;
				void* f32;
				void* f33;
				void* f34;
				void* f35;
				void* f36;
				void* f37;
				void* f38;
				void* f39;
				void* f40;
				void* f41;
				void* f42;
				void* f43;
			};
#pragma pack(pop)

			static_assert(offsetof(fox::ui::ModelNodeText, __pad0) == 8);
			static_assert(offsetof(fox::ui::ModelNodeText, f0) == 114);
			static_assert(offsetof(fox::ui::ModelNodeText, __pad1) == 115);
			static_assert(offsetof(fox::ui::ModelNodeText, f1) == 136);
			static_assert(offsetof(fox::ui::ModelNodeText, flags) == 144);
			static_assert(offsetof(fox::ui::ModelNodeText, f3) == 146);
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
			static_assert(offsetof(fox::ui::ModelNodeText, f13) == 216);
			static_assert(offsetof(fox::ui::ModelNodeText, f14) == 218);
			static_assert(offsetof(fox::ui::ModelNodeText, f15) == 219);
			static_assert(offsetof(fox::ui::ModelNodeText, f16) == 220);
			static_assert(offsetof(fox::ui::ModelNodeText, __pad4) == 222);
			static_assert(offsetof(fox::ui::ModelNodeText, f17) == 224);
			static_assert(offsetof(fox::ui::ModelNodeText, f18) == 232);
			static_assert(offsetof(fox::ui::ModelNodeText, f19) == 240);
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
			static_assert(offsetof(fox::ui::ModelNodeText, f31) == 344);
			static_assert(offsetof(fox::ui::ModelNodeText, f32) == 352);
			static_assert(offsetof(fox::ui::ModelNodeText, f33) == 360);
			static_assert(offsetof(fox::ui::ModelNodeText, f34) == 368);
			static_assert(offsetof(fox::ui::ModelNodeText, f35) == 376);
			static_assert(offsetof(fox::ui::ModelNodeText, f36) == 384);
			static_assert(offsetof(fox::ui::ModelNodeText, f37) == 392);
			static_assert(offsetof(fox::ui::ModelNodeText, f38) == 400);
			static_assert(offsetof(fox::ui::ModelNodeText, f39) == 408);
			static_assert(offsetof(fox::ui::ModelNodeText, f40) == 416);
			static_assert(offsetof(fox::ui::ModelNodeText, f41) == 424);
			static_assert(offsetof(fox::ui::ModelNodeText, f42) == 432);
			static_assert(offsetof(fox::ui::ModelNodeText, f43) == 440);

			static_assert(sizeof(ModelNodeText) == 0x1C0);

			struct ModelNodeLine
			{

			};

			struct ModelNodeMesh
			{

			};

			struct Model;

			struct ComponentTraversalInfo;

			struct Model_vtbl
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

			struct Model
			{
				fox::ui::Model_vtbl* __vftable;
			};

			struct WindowInterface;
			struct TriggerPool;
			struct LayoutComponent;
			struct Layout;
			struct GraphState;
			struct ModelNodeCommon;
			struct Animation;
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
		};

		struct Entity
		{
			char __pad0[56];
			char team;
		};

		struct EntityPtrBase
		{
			Entity* entity;
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
				void* sppSocket;
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
					void*(__fastcall* __destructor)(SessionImpl2* this_);
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
					Group*(__fastcall* GetAllMembers)(SessionImpl2* this_);
				};

				union SessionImpl2_vtbl
				{
					SessionImpl2_vtbl_tpp tpp;
					SessionImpl2_vtbl_mgo mgo;
				};

				struct SessionImpl2_SessionInterface;

				struct SessionImpl2_SessionInterface_vtbl
				{
					void*(__fastcall* GetLocalMemberInterface)(SessionImpl2_SessionInterface* this_);
					void*(__fastcall* GetHostMemberInterface)(SessionImpl2_SessionInterface* this_);
					void*(__fastcall* GetMemberInterfaceAtIndex)(SessionImpl2_SessionInterface* this_, void*);
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
					void* sPtr5;
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

		struct SharedString
		{
			char* buffer;
		};
	}

	union Session
	{
		fox::nt::impl::SessionImpl2* SessionImpl2;
		fox::nt::impl::SessionIdle* sessionIdle;
	};

	namespace tpp::ui::menu::impl
	{
		struct MenuSystemImpl
		{

		};
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

		struct FobTarget
		{
			char __pad0[64];
			SessionConnectInfo* sessionConnectInfo;
		};

		struct CmdGetFobTargetDetailResult
		{
			char __pad0[7144];
			std::uint64_t steam_id;
			char __pad1[8];
			fox::SharedString* ip;
			std::uint32_t port;
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
					void* (__fastcall* GetCurrentWeapon)(tpp::gm::player::impl::EquipControllerImpl_tpp*, unsigned int);
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
					void* (__fastcall* GetCurrentWeapon)(tpp::gm::player::impl::EquipControllerImpl_mgo*, unsigned int);
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
			void* (__fastcall* SetAlpha1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetAlpha2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* GetAlpha3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			void* (__fastcall* GetAlpha4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
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
			void* (__fastcall* SetVertexTranslate)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::StringId, Vectormath::Aos::Vector3*, Vectormath::Aos::Vector3*);
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
			void* (__fastcall* GetPaletteColor)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
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
			void* pad[6];
			void* (__fastcall* SetVisible1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, bool);
			void* (__fastcall* SetVisible2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, bool);
			void* (__fastcall* IsVisible1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			void* (__fastcall* IsVisible2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
			void* (__fastcall* SetAlpha1)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*, float);
			void* (__fastcall* SetAlpha2)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*, float);
			void* (__fastcall* GetAlpha3)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNode*);
			void* (__fastcall* GetAlpha4)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::LayoutComponent*);
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
			void* (__fastcall* SetVertexTranslate)(fox::uix::impl::UixUtilityImpl* this_, fox::ui::ModelNodeMesh*, fox::StringId, Vectormath::Aos::Vector3*, Vectormath::Aos::Vector3*);
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
			void* (__fastcall* GetPaletteColor)(fox::uix::impl::UixUtilityImpl* this_, fox::StringId);
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

	namespace gn
	{
		struct swapchain
		{
			char __pad0[24];
			IDXGISwapChain* swapChain;
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
		char __pad4[12];
		int has_password;
		char __pad5[8];
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct mgo_match_t
	{
		char __pad0[13];
		char is_joining_invite;
		game::steam_id invite_lobby_id;
		char __pad1[2];
		match_settings_t match_settings;
		char __pad2[48];
		short lobby_member_limit;
		char __pad3[90];
		game::steam_id lobby_owner;
		char __pad4[76];
		game::steam_id lobby_id;
		char __pad5[1008];
		match_rules_t match_rules;
		game::steam_id lobby_id2;
		char __pad6[1604];
	};
#pragma pack(pop)

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
		char __pad3[26];
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
