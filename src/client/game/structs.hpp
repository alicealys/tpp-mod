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

	struct ISteamNetworking;

	struct /*VFT*/ ISteamNetworking_vtbl
	{
		bool(__fastcall* SendP2PPacket)(ISteamNetworking* this_, steam_id steamIDRemote, const void* pubData, unsigned int cubData, int eP2PSendType);
		bool(__fastcall* IsP2PPacketAvailable)(ISteamNetworking* this_, unsigned int* pcubMsgSize, int idk);
		bool(__fastcall* ReadP2PPacket)(ISteamNetworking* this_, void* pubDest, unsigned int cubDest, unsigned int* pcubMsgSize, steam_id* psteamIDRemote);
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

	namespace tpp::ui::hud::CommonDataManager
	{
		struct CommonDataManager
		{
			char __pad0[4440];
			void* announceLogViewer;
		};
	}

	namespace tpp::ui::menu::UiCommonDataManager
	{
		struct UiCommonDataManager;
	}

	namespace fox
	{
		namespace ui
		{
			struct SoundControl;

			struct TextUnit
			{
				char* a1;
				unsigned int a2;
				unsigned int flags;
				unsigned __int16 a4;
				unsigned __int16 a5;
				float f1;
				float f2;
				unsigned int a8;
			};

			static_assert(offsetof(TextUnit, f1) == 20);
			static_assert(offsetof(TextUnit, f2) == 24);
			static_assert(offsetof(TextUnit, a8) == 28);
		}

#pragma pack(push, 1)
		struct unk1_unk1
		{
			char __pad0[13];
			char is_joining_invite;
			game::steam_id invite_lobby_id;
			char __pad1[410];
			game::steam_id lobby_id;
			char __pad2[2704];
		};
#pragma pack(pop)

		static_assert(offsetof(unk1_unk1, is_joining_invite) == 13);
		static_assert(offsetof(unk1_unk1, invite_lobby_id) == 14);
		static_assert(offsetof(unk1_unk1, lobby_id) == 432);
		static_assert(sizeof(unk1_unk1) == 3144);

		struct unk1
		{
			char __pad0[8];
			unk1_unk1* unk1;
			void* unk2;
		};

		static_assert(sizeof(unk1) == 24);

		struct unk2
		{
			char __pad0[5531];
			char unk1;
		};

		struct StringId
		{
			std::uint32_t l;
			std::uint32_t h;
		};

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

				struct SessionImpl2_vtbl
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
					void(__fastcall* Close)(SessionImpl2* this_);
					void(__fastcall* SetHeartBeat)(SessionImpl2* this_, bool, float, float);
					void(__fastcall* PrepareP2PConnection)(SessionImpl2* this_, void* sessionReserveManager);
					void(__fastcall* Start)(SessionImpl2* this_);
					void(__fastcall* EnableAccept)(SessionImpl2* this_, bool);
					void(__fastcall* DisconnectPreparingMembers)(SessionImpl2* this_);
					void*(__fastcall* GetP2pConnectionManager)(SessionImpl2* this_);
					void(__fastcall* SetP2pConnectionManager)(SessionImpl2* this_, void*);
					void(__fastcall* FixFrameTime)(SessionImpl2* this_);
					void(__fastcall* Update)(SessionImpl2* this_);
					unsigned __int64(__fastcall* GetResult)(SessionImpl2* this_);
					void(__fastcall* InitAcceptedMember)(SessionImpl2* this_, void*, void*);
					Group*(__fastcall* GetAllMembers)(SessionImpl2* this_);
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
					void* a10;
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

	namespace gn
	{
		struct swapchain
		{
			char __pad0[24];
			IDXGISwapChain* swapChain;
		};

	}

	namespace lua
	{
		struct lua_state_container
		{
			void* unk;
			lua_State* state;
			// ...
		};
	}
}
