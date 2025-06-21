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
   return Message::fromString(buffer);
}

void ClientSocket::sendMessage(const Message& message, int flags) const {
   if (!isConnected()) {
      throw std::runtime_error("Socket is not connected.");
   }
   std::string to_send{message.asString()};
   if (send(socketDescriptor(), to_send.c_str(), to_send.size(), flags) < 0) {
      throw std::runtime_error("Error writing to socket + " +
                               std::string(strerror(errno)));
   }
}

void ClientSocket::teardown() {
   m_is_connected = false;
   Socket::teardown();
}
