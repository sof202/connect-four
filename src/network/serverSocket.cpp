#include "network/serverSocket.hpp"

#include <netinet/in.h>
#include <sys/socket.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

#include "network/clientSocket.hpp"

void ServerSocket::listen(int backlog) const {
   if (!isValid()) {
      throw std::runtime_error("Server socket not intialised properly.");
   }
   if (::listen(socketDescriptor(), backlog) < 0) {
      throw std::runtime_error("Error listening on socket: " +
                               std::string(strerror(errno)));
   }
}

void ServerSocket::bindToAddress(sockaddr_in address) const {
   if (bind(socketDescriptor(),
            reinterpret_cast<sockaddr *>(&address),
            sizeof(address)) < 0)
      throw std::runtime_error("Error binding socket: " +
                               std::string(strerror(errno)));
}

auto ServerSocket::acceptClient(sockaddr_in &client_address) const
    -> ClientSocket {
   socklen_t client_address_size{sizeof(client_address)};
   int client_socket{};
   while (true) {
      client_socket = accept(socketDescriptor(),
                             reinterpret_cast<sockaddr *>(&client_address),
                             &client_address_size);
      if (client_socket >= 0) break;
      if (errno != EINTR) {
         throw std::runtime_error("Error accepting socket: " +
                                  std::string(strerror(errno)));
      }
   }
   return ClientSocket(client_socket, true);
}
