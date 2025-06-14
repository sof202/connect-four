#include "network/serverSocket.hpp"

#include <netinet/in.h>
#include <sys/socket.h>

#include <cstring>
#include <stdexcept>
#include <string>

#include "network/address.hpp"
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

void ServerSocket::bindToAddress(const IPv4Address& address) const {
   if (bind(socketDescriptor(), address, address.length()) < 0)
      throw std::runtime_error("Error binding socket: " +
                               std::string(strerror(errno)));
}

auto ServerSocket::acceptClient(IPv4Address& client_address) const
    -> ClientSocket {
   socklen_t client_address_size{client_address.length()};
   int client_socket{};
   while (true) {
      client_socket =
          accept(socketDescriptor(), client_address, &client_address_size);
      if (client_socket >= 0) break;
      if (errno != EINTR) {
         throw std::runtime_error("Error accepting socket: " +
                                  std::string(strerror(errno)));
      }
   }
   return ClientSocket(client_socket, true);
}
