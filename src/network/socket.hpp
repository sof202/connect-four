#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/socket.h>

#include <stdexcept>
#include <utility>

class Socket {
  public:
   Socket(int domain, int socket_type, int protocol = 0) :
       m_socket_descriptor{socket(domain, socket_type, protocol)} {
      if (!isValid()) throw std::runtime_error("Error creating socket");
   }
   explicit Socket(int socket) : m_socket_descriptor{socket} {
      if (!isValid()) throw std::runtime_error("Invalid socket descriptor.");
   }

   ~Socket() { teardown(); }

   Socket(const Socket&) = delete;
   auto operator=(const Socket&) -> Socket& = delete;
   Socket(Socket&& other) noexcept :
       m_socket_descriptor(std::exchange(other.m_socket_descriptor, -1)) {}
   auto operator=(Socket&& other) noexcept -> Socket& {
      if (this != &other) {
         teardown();
         m_socket_descriptor = std::exchange(other.m_socket_descriptor, -1);
      }
      return *this;
   }

   [[nodiscard]] auto isValid() const -> bool {
      return m_socket_descriptor >= 0;
   }
   [[nodiscard]] auto socketDescriptor() const -> int {
      return m_socket_descriptor;
   }

  private:
   int m_socket_descriptor{};
   void teardown();
};

#endif  // SOCKET_H_
