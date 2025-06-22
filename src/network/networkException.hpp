#ifndef NETWORK_EXCEPTION_H_
#define NETWORK_EXCEPTION_H_

#include <stdexcept>
#include <string>

#include "network/address.hpp"

class NetworkException : public std::runtime_error {
  public:
   explicit NetworkException(const std::string& message) :
       std::runtime_error("[Network exception]: " + message) {}
};

class SocketDisconnectException : public NetworkException {
  public:
   explicit SocketDisconnectException() :
       NetworkException("Socket was disconnected") {}
};

class SocketConnectionException : public NetworkException {
  public:
   explicit SocketConnectionException(const IPv4Address& address) :
       NetworkException("Socket failed to connect to " +
                        address.addressString()) {}
};

#endif  // NETWORK_EXCEPTION_H_
