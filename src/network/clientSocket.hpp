#ifndef CLIENT_SOCKET_H_
#define CLIENT_SOCKET_H_

#include <netinet/in.h>

#include "network/socket.hpp"

class ClientSocket : public Socket {
  public:
   ClientSocket(int domain, int socket_type, int protocol = 0) :
       Socket(domain, socket_type, protocol) {}
   explicit ClientSocket(int socket) : Socket(socket) {}

   [[nodiscard]] auto isConnected() const -> bool { return m_is_connected; }
   void connectToServer(sockaddr_in server_address);
   [[nodiscard]] auto receiveMessage(std::size_t max_size, int flags = 0) const
       -> std::string;
   void sendMessage(const std::string& message, int flags = 0) const;

  private:
   bool m_is_connected{false};
};

#endif  // CLIENT_SOCKET_H_
