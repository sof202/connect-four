#include "network/clientSocket.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include "network/address.hpp"
#include "network/message.hpp"
#include "network/networkException.hpp"

void ClientSocket::connectToServer(const IPv4Address& server_address,
                                   int max_attempts,
                                   bool reconnect) {
   if (isConnected() && reconnect) {
      teardown();
      setSocketDescriptor(socket(AF_INET, SOCK_STREAM, 0));
   }
   for (int attempt{}; attempt < max_attempts; ++attempt) {
      if (connect(socketDescriptor(),
                  server_address,
                  server_address.length()) == 0) {
         m_is_connected = true;
         return;
      }
      if (attempt < max_attempts) {
         std::cerr << "Could not connect to server, retrying... (attempt: "
                   << attempt + 1 << ")\n";
         teardown();
         setSocketDescriptor(socket(AF_INET, SOCK_STREAM, 0));

         constexpr int max_delay_seconds{30};
         int delay_seconds{std::min(max_delay_seconds,
                                    static_cast<int>(std::pow(2, attempt)))};
         std::this_thread::sleep_for(std::chrono::seconds(delay_seconds));
      }
   }
   throw SocketConnectionException(server_address);
}

[[nodiscard]] auto ClientSocket::receiveMessage(std::size_t max_size,
                                                int flags) const -> Message {
   if (!isConnected()) {
      throw std::runtime_error("Socket is not connected.");
   }

   uint32_t length{};
   ssize_t bytes_received{
       recv(socketDescriptor(), &length, sizeof(length), flags)};
   if (bytes_received == 0) throw SocketDisconnectException();
   if (bytes_received < 0) {
      throw std::runtime_error("An error occurred: " +
                               std::string(strerror(errno)));
   }
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
