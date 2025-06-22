#ifndef NETWORK_EXCEPTION_H_
#define NETWORK_EXCEPTION_H_

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

#include "network/address.hpp"

class NetworkException : public std::runtime_error {
  public:
   explicit NetworkException(const std::string& message) :
       std::runtime_error("[Network exception]: " + message) {}
};

class SocketCreationException : public NetworkException {
  public:
   explicit SocketCreationException(int error_code) :
       NetworkException("Socket creation failed " +
                        std::string(strerror(error_code))) {}
};

class SocketDisconnectException : public NetworkException {
  public:
   explicit SocketDisconnectException(int socket_descriptor) :
       NetworkException("Socket was disconnected"),
       m_socket_descriptor{socket_descriptor} {}
   explicit SocketDisconnectException(int socket_descriptor, int error_code) :
       NetworkException("Socket was disconnected " +
                        std::string(strerror(error_code))),
       m_socket_descriptor{socket_descriptor} {}
   [[nodiscard]] auto socketDescriptor() const -> int {
      return m_socket_descriptor;
   }

  private:
   int m_socket_descriptor{};
};

class SocketConnectionException : public NetworkException {
  public:
   explicit SocketConnectionException(const IPv4Address& address) :
       NetworkException("Socket failed to connect to " +
                        address.addressString()) {}
};

#endif  // NETWORK_EXCEPTION_H_
