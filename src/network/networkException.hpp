#ifndef NETWORK_EXCEPTION_H_
#define NETWORK_EXCEPTION_H_

#include <stdexcept>
#include <string>

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

#endif  // NETWORK_EXCEPTION_H_
