#include "network/clientSocket.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>

#include "network/address.hpp"

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

void ClientSocket::connectToServer(const IPv4Address& server_address) {
   if (connect(socketDescriptor(), server_address, server_address.length()) <
       0) {
      throw std::runtime_error("Error connecting to server: " +
                               std::string(strerror(errno)));
   }
   m_is_connected = true;
}

[[nodiscard]] auto ClientSocket::receiveMessage(std::size_t max_size,
                                                int flags) const
    -> std::string {
   if (!isConnected()) {
      throw std::runtime_error("Socket is not connected.");
   }

   std::string buffer(max_size, '\0');
   ssize_t bytes_received{};
   while (true) {
      bytes_received =
          recv(socketDescriptor(), buffer.data(), buffer.size(), flags);
      if (bytes_received >= 0) break;

      if (errno != EINTR) {
         throw std::runtime_error("Error recieving from socket: " +
                                  std::string(strerror(errno)));
      }
   };
   buffer.resize(static_cast<std::size_t>(bytes_received));
   return buffer;
}

void ClientSocket::sendMessage(const std::string& message, int flags) const {
   if (!isConnected()) {
      throw std::runtime_error("Socket is not connected.");
   }
   if (send(socketDescriptor(), message.c_str(), message.size(), flags) < 0) {
      throw std::runtime_error("Error writing to socket + " +
                               std::string(strerror(errno)));
   }
}

void ClientSocket::teardown() {
   m_is_connected = false;
   Socket::teardown();
}
