
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cstring>
#include <exception>
#include <iostream>

#include "network/address.hpp"
#include "network/clientSocket.hpp"

auto main() -> int {
   try {
      ClientSocket client_socket{AF_INET, SOCK_STREAM};
      struct hostent* server =
          gethostbyname("localhost");  // resolve host name
      if (server == nullptr) {
         std::cerr << "Error resolving hostname\n";
         exit(1);
      }
      IPv4Address server_address{8080};

      client_socket.connectToServer(server_address);
      client_socket.sendMessage("Hello from client.");

      std::cout << client_socket.receiveMessage(255) << '\n';

   } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
   } catch (...) {
      std::cerr << "An unknown error occurred\n";
   }
}
