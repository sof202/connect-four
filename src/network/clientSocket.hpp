#ifndef CLIENT_SOCKET_H_
#define CLIENT_SOCKET_H_

#include <netinet/in.h>

#include "network/address.hpp"
#include "network/message.hpp"
#include "network/socket.hpp"

class ClientSocket : public Socket {
  public:
   ClientSocket(int domain, int socket_type, int protocol = 0) :
       Socket(domain, socket_type, protocol) {}
   explicit ClientSocket(int socket, bool is_connected = false) :
       Socket(socket),
       m_is_connected{is_connected} {}

   ~ClientSocket() override = default;
   ClientSocket(const ClientSocket&) = delete;
   auto operator=(const ClientSocket&) -> ClientSocket& = delete;
   ClientSocket(ClientSocket&& other) noexcept = default;
   auto operator=(ClientSocket&& other) noexcept -> ClientSocket& = default;

   [[nodiscard]] auto isConnected() const -> bool { return m_is_connected; }
   void connectToServer(const IPv4Address& server_address,
                        int max_attempts = 1,
                        bool reconnect = false);

   [[nodiscard]] auto receiveMessage(std::size_t max_size, int flags = 0) const
       -> Message;
   void sendMessage(const Message& message, int flags = 0) const;

  protected:
   void teardown() override;

  private:
   bool m_is_connected{false};
};

#endif  // CLIENT_SOCKET_H_
