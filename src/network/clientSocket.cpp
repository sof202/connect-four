#include "network/clientSocket.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

void ClientSocket::connectToServer(sockaddr_in server_address) {
   if (connect(socketDescriptor(),
               reinterpret_cast<sockaddr*>(&server_address),
               sizeof(server_address)) < 0) {
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
   buffer.resize(bytes_received);
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
