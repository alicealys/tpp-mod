#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game_socket.hpp"
#include "command.hpp"

#include <utils/hook.hpp>

namespace game_socket
{
	namespace
	{
#pragma pack(push, 1)
		struct message_header_t
		{
			std::uint16_t game_object_id;
			std::uint8_t peer_type;
			std::uint16_t a2;
			std::uint8_t client;
			std::uint64_t type;
		};
#pragma pack(pop)

		std::unordered_map<std::uint64_t, message_callback_t> handlers;
		std::vector<session_observer_t> session_observers;

		void handle_message(game::fox::nt::impl::SessionImpl2* session, const message_info_t& info, const std::string_view& buffer)
		{
			const auto handler = handlers.find(info.type);
			if (handler == handlers.end())
			{
				return;
			}

			handler->second(session, info, buffer);
		}

		void handle_message(unsigned short object_id, int peer_type, unsigned char sender, unsigned char target, const char* buffer, unsigned short buffer_size)
		{
			const auto session = *game::s_pSession;
			if (session == nullptr || buffer == nullptr || buffer_size < 8)
			{
				return;
			}

			const auto self = session->sessionInterface.__vftable->GetLocalMemberInterface(&session->sessionInterface);
			if (self->__vftable->GetIndex(self) != target && target != 0xFF)
			{
				return;
			}

			message_info_t info{};
			info.type = *reinterpret_cast<const std::uint64_t*>(buffer);
			info.peer_type = static_cast<std::uint8_t>(peer_type);
			info.sender = sender;
			info.target = target;
			info.game_object_id = object_id;

			const auto buffer_view = std::string_view{buffer + 8, buffer + buffer_size};
			handle_message(session, info, buffer_view);
		}

		utils::hook::detour game_message_system_forward_signal_hook;
		int* game_message_system_forward_signal_stub(game::fox::gm::impl::GameObjectMessageSystemImpl* inst, int* result, unsigned short object_id,
			int peer_type, const char* buffer, unsigned short buffer_size, int a7, unsigned char sender, unsigned char target)
		{
			handle_message(object_id, peer_type, sender, target, buffer, buffer_size);
			return game_message_system_forward_signal_hook.invoke<int*>(inst, result, object_id, peer_type, buffer, buffer_size, a7, sender, target);
		}

		utils::hook::detour session_observer_notify_hook;
		void session_observer_notify_stub(game::fox::nt::ObserverBase<game::fox::nt::Session>* this_, game::fox::nt::impl::SessionImpl2* session, 
			int event, unsigned char* arg)
		{
			session_observer_notify_hook.invoke<void>(this_, session, event, arg);

			for (auto& observer : session_observers)
			{
				if (observer.event == event)
				{
					observer.callback(session, arg);
				}
			}
		}
	}

	void send_raw(const std::uint64_t type, const void* buffer, const std::size_t size, const std::int8_t client, const std::uint8_t peer_type)
	{
		const auto session = *game::s_pSession;
		auto message_system = *game::fox::gm::impl::g_messagesystem;
		if (session == nullptr || message_system == nullptr)
		{
			return;
		}

		const auto socket = message_system->sockets[0];
		auto lock = 0;
		game::fox::Mutex_::SpinLock(&message_system->mutex, &lock);
		const auto _0 = gsl::finally([&]
		{
			if (lock >= 0)
			{
				game::fox::Mutex_::Unlock(&message_system->mutex, &lock);
			}
		});

		std::string message;
		constexpr const auto header_size = sizeof(message_header_t);
		message.resize(size + header_size);

		const auto self = session->sessionInterface.__vftable->GetLocalMemberInterface(&session->sessionInterface);
		const auto object_id = static_cast<std::uint16_t>(self->__vftable->GetIndex(self));

		const auto header = reinterpret_cast<message_header_t*>(message.data());
		header->game_object_id = _byteswap_ushort(object_id);
		header->peer_type = peer_type;
		header->a2 = 0;

		if (peer_type == game::fox::gm::PEER_DIRECT)
		{
			header->client = 0;
			header->type = _byteswap_uint64(type);
			std::memcpy(message.data() + header_size, buffer, size);
		}
		else
		{
			*reinterpret_cast<std::uint64_t*>(&header->client) = _byteswap_uint64(type);
			std::memcpy(message.data() + header_size - 1, buffer, size);
		}

		if (session->sessionInterface.__vftable->IsHost(&session->sessionInterface))
		{
			for (auto i = 0u; i < session->allMembers.size; i++)
			{
				const auto member = session->sessionInterface.__vftable->GetMemberInterfaceAtIndex(&session->sessionInterface, i);
				if (member == nullptr)
				{
					continue;
				}

				const auto index = member->__vftable->GetIndex(member);
				if (client == index || client == -1)
				{
					if (peer_type == game::fox::gm::PEER_DIRECT)
					{
						if (client != -1)
						{
							header->client = client;
						}
						else
						{
							header->client = static_cast<char>(index);
						}
					}

					if (self == member)
					{
						message_info_t info{};
						info.peer_type = header->peer_type;
						info.sender = static_cast<std::int8_t>(index);
						info.target = client;
						info.type = type;
						info.game_object_id = object_id;
						const auto buffer_view = std::string_view{reinterpret_cast<const char*>(buffer),
							reinterpret_cast<const char*>(buffer) + size};
						handle_message(session, info, buffer_view);
						continue;
					}

					socket->__vftable->RequestToSendToMember(socket, static_cast<char>(index), 0, message.data(), 
						static_cast<unsigned int>(message.size()));

					if (client != -1)
					{
						break;
					}
				}
			}
		}
		else
		{
			const auto host = session->sessionInterface.__vftable->GetHostMemberInterface(&session->sessionInterface);
			const auto index = host->__vftable->GetIndex(host);

			if (client != -1)
			{
				if (peer_type == game::fox::gm::PEER_DIRECT)
				{
					header->client = client;
				}

				socket->__vftable->RequestToSendToMember(socket, static_cast<char>(index), 0,
					message.data(), static_cast<unsigned int>(message.size()));
				return;
			}

			for (auto i = 0u; i < session->allMembers.size; i++)
			{
				const auto member = session->sessionInterface.__vftable->GetMemberInterfaceAtIndex(&session->sessionInterface, i);
				if (member == nullptr)
				{
					continue;
				}

				if (peer_type == game::fox::gm::PEER_DIRECT)
				{
					header->client = static_cast<char>(member->__vftable->GetIndex(member));
				}
				
				socket->__vftable->RequestToSendToMember(socket, static_cast<char>(index), 0, 
					message.data(), static_cast<unsigned int>(message.size()));
			}
		}
	}

	void send(const std::uint64_t message_type, const std::string_view& buffer, const std::int8_t client, const std::uint8_t peer_type)
	{
		send_raw(message_type, buffer.data(), buffer.size(), client, peer_type);
	}

	void send(const std::string& message_type, const std::string_view& buffer, const std::int8_t client, const std::uint8_t peer_type)
	{
		const auto hash = game::fox::FoxStrHash32(message_type.data(), message_type.size());
		send_raw(hash.id, buffer.data(), buffer.size(), client, peer_type);
	}

	void on(const std::string& message_type, const message_callback_t& handler)
	{
		const auto hash = game::fox::FoxStrHash32(message_type.data(), message_type.size());
		handlers.insert(std::make_pair(hash.id, handler));
	}

	void on_session_notify(const std::int32_t event, const session_callback_t callback)
	{
		auto& observer = session_observers.emplace_back();
		observer.event = event;
		observer.callback = callback;
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			game_message_system_forward_signal_hook.create(SELECT_VALUE(0x140BFCAE0, 0x1408E9F20, 0x0, 0x0), game_message_system_forward_signal_stub);
		}

		void on_game_initialized() override
		{
			const auto message_system = *game::fox::gm::impl::g_messagesystem;
			if (message_system == nullptr)
			{
				return;
			}

			session_observer_notify_hook.create(message_system->sessionObserver.__vftable->NotifyImpl, session_observer_notify_stub);
		}
	};
}

REGISTER_COMPONENT(game_socket::component)
