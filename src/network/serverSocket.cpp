#include "network/serverSocket.hpp"

#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>

#include <cstring>
#include <stdexcept>
#include <string>

#include "network/address.hpp"
#include "network/clientSocket.hpp"
#include "network/networkException.hpp"

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

auto ServerSocket::acceptClient(IPv4Address& client_address,
                                int timeout_seconds) const -> ClientSocket {
   socklen_t client_address_size{client_address.length()};
   int client_socket{};
   if (timeout_seconds >= 0) {
      struct pollfd poll_descriptor{};
      poll_descriptor.fd = socketDescriptor();
      poll_descriptor.events = POLLIN;
      constexpr int milliseconds_in_second{1000};
      int poll_result =
          poll(&poll_descriptor, 1, timeout_seconds * milliseconds_in_second);
      if (poll_result == 0) {
         throw NetworkException("Accept timed out");
      }
      if (poll_result < 0) {
         throw NetworkException("Error when using poll: " +
                                std::string(strerror(errno)));
      }
   }
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
