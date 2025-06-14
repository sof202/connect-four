#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <stdexcept>
#include <string>

class IPv4Address {
  public:
   IPv4Address() = default;
   explicit IPv4Address(const std::string& ip_address, uint16_t port) {
      if (inet_pton(static_cast<sa_family_t>(AF_INET),
                    ip_address.c_str(),
                    &m_address.sin_addr) != 1)
         throw std::runtime_error("Invalid IPv4 address: " + ip_address);
      m_address.sin_port = htons(port);
   }
   explicit IPv4Address(uint16_t port) { m_address.sin_port = htons(port); }

   // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
   // reinterpret cast is expected behaviour here (Thanks POSIX)
   operator sockaddr*() { return reinterpret_cast<sockaddr*>(&m_address); }
   operator const sockaddr*() const {
      return reinterpret_cast<const sockaddr*>(&m_address);
   }
   // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

   [[nodiscard]] auto length() const -> socklen_t { return sizeof(m_address); }

  private:
   sockaddr_in m_address{
       .sin_family = AF_INET, .sin_port = 0, .sin_addr = {}, .sin_zero = {}};
};

#endif  // ADDRESS_H_
