#ifndef CLIENT_SOCKET_H_
#define CLIENT_SOCKET_H_

#include <netinet/in.h>

#include <array>
#include <cstdint>
#include <string_view>

#include "network/address.hpp"
#include "network/socket.hpp"

namespace MessageType {
enum Type : std::uint8_t {
   move,
   board,
   requestInput,
   end,
   info,
   maxMessageType,
};
using namespace std::string_view_literals;
constexpr std::array message_type_names{
    "move"sv,
    "board"sv,
    "requestInput"sv,
    "end"sv,
    "info"sv,
};
static_assert(message_type_names.size() ==
              static_cast<std::size_t>(maxMessageType));
constexpr auto getTypeName(const Type& type) -> std::string_view {
   return message_type_names.at(static_cast<std::size_t>(type));
}
constexpr auto getTypeFromName(std::string_view type_name) -> Type {
   for (std::size_t idx{0}; idx < maxMessageType; ++idx) {
      if (message_type_names.at(idx) == type_name)
         return static_cast<Type>(idx);
   }
   return info;
}
}  // namespace MessageType

class ClientSocket : public Socket {
  public:
   ClientSocket(int domain, int socket_type, int protocol = 0) :
       Socket(domain, socket_type, protocol) {}
   explicit ClientSocket(int socket, bool is_connected = false) :
       Socket(socket),
       m_is_connected{is_connected} {}

   ~ClientSocket() override = default;
   ClientSocket(const ClientSocket&) = delete;
   auto operator=(const ClientSocket&) -> ClientSocket& = delete;
   ClientSocket(ClientSocket&& other) noexcept = default;
   auto operator=(ClientSocket&& other) noexcept -> ClientSocket& = default;

   [[nodiscard]] auto isConnected() const -> bool { return m_is_connected; }
   void connectToServer(const IPv4Address& server_address);

   [[nodiscard]] auto receiveMessage(std::size_t max_size, int flags = 0) const
       -> std::pair<MessageType::Type, std::string>;
   void sendMessage(MessageType::Type message_type,
                    const std::string& message,
                    int flags = 0) const;

  protected:
   void teardown() override;

  private:
   bool m_is_connected{false};
};

#endif  // CLIENT_SOCKET_H_
