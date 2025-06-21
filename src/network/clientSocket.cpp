#include "network/clientSocket.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

#include "network/address.hpp"
#include "network/message.hpp"

void ClientSocket::connectToServer(const IPv4Address& server_address) {
   if (connect(socketDescriptor(), server_address, server_address.length()) <
       0) {
      throw std::runtime_error("Error connecting to server: " +
                               std::string(strerror(errno)));
   }
   m_is_connected = true;
}

[[nodiscard]] auto ClientSocket::receiveMessage(std::size_t max_size,
                                                int flags) const -> Message {
   if (!isConnected()) {
      throw std::runtime_error("Socket is not connected.");
   }

   uint32_t length{};
   ssize_t bytes_received{
       recv(socketDescriptor(), &length, sizeof(length), flags)};
   if (bytes_received < 0) throw std::runtime_error("Connection closed.");
   length = ntohl(length);
   if (length > max_size) throw std::runtime_error("Message too large.");

   std::string buffer(length, '\0');
   bytes_received =
       recv(socketDescriptor(), buffer.data(), buffer.size(), flags);
   if (bytes_received < 0) throw std::runtime_error("Connection closed.");

   return Message::fromString(buffer);
}

void ClientSocket::sendMessage(const Message& message, int flags) const {
   if (!isConnected()) {
      throw std::runtime_error("Socket is not connected.");
   }
   std::string to_send{message.asString()};
   uint32_t length{htonl(static_cast<uint32_t>(to_send.size()))};

   if (send(socketDescriptor(), &length, sizeof(length), flags) < 0) {
      throw std::runtime_error("Error writing length to socket + " +
                               std::string(strerror(errno)));
   }

   if (send(socketDescriptor(), to_send.c_str(), to_send.size(), flags) < 0) {
      throw std::runtime_error("Error writing message to socket + " +
                               std::string(strerror(errno)));
   }
}

void ClientSocket::teardown() {
   m_is_connected = false;
   Socket::teardown();
}
