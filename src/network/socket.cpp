#include "network/socket.hpp"

#include <asm-generic/socket.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>
#include <string>

void Socket::teardown() {
   if (isValid()) {
      close(m_socket_descriptor);
      m_socket_descriptor = -1;
   }
}

void Socket::setAddressReusage(bool enable) const {
   int option_value{enable ? 1 : 0};
   if (setsockopt(m_socket_descriptor,
                  SOL_SOCKET,
                  SO_REUSEADDR,
                  &option_value,
                  sizeof(option_value)) < 0) {
      throw std::runtime_error("setsockopt(SO_REUSEADDR) failed: " +
                               std::string(strerror(errno)));
   }
}
