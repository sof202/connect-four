#include <netinet/in.h>
#include <sys/socket.h>

#include <cstring>
#include <iostream>

#include "network/address.hpp"
#include "network/serverSocket.hpp"

auto main() -> int {
   try {
      ServerSocket server_socket{AF_INET, SOCK_STREAM, 0, true};
      IPv4Address server_address{8080};

      server_socket.bindToAddress(server_address);
      server_socket.listen(5);

      std::cout << "Server listening on port " << 8080 << "...\n";

      while (true) {
         IPv4Address client_address{};
         ClientSocket client_socket{
             server_socket.acceptClient(client_address)};
         std::cout << "Recieved message from client: "
                   << client_socket.receiveMessage(255) << '\n';
         client_socket.sendMessage("Hello from server");
      }
   } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
   } catch (...) {
      std::cerr << "An unknown error occurred\n";
   }
}
