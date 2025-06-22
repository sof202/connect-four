#ifndef SERVER_SOCKET_H_
#define SERVER_SOCKET_H_

#include <netinet/in.h>

#include "network/address.hpp"
#include "network/clientSocket.hpp"
#include "network/socket.hpp"

class ServerSocket : public Socket {
  public:
   ServerSocket(int domain,
                int socket_type,
                int protocol = 0,
                bool reuse_address = false) :
       Socket(domain, socket_type, protocol) {
      setAddressReusage(reuse_address);
   }

   ~ServerSocket() override = default;
   ServerSocket(const ServerSocket&) = delete;
   auto operator=(const ServerSocket&) -> ServerSocket& = delete;
   ServerSocket(ServerSocket&& other) noexcept = default;
   auto operator=(ServerSocket&& other) noexcept -> ServerSocket& = default;

   void bindToAddress(const IPv4Address& address) const;
   void listen(int backlog) const;
   auto acceptClient(IPv4Address& client_address,
                     int timeout_seconds = -1) const -> ClientSocket;
};

#endif  // SERVER_SOCKET_H_
