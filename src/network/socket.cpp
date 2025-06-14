#include "network/socket.hpp"

#include <unistd.h>

void Socket::teardown() {
   if (isValid()) {
      close(m_socket_descriptor);
      m_socket_descriptor = -1;
   }
}
