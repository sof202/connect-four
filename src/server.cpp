#include <netinet/in.h>
#include <sys/socket.h>

#include <cstring>
#include <iostream>

#include "network/serverSocket.hpp"

auto main() -> int {
   try {
      ServerSocket server_socket{AF_INET, SOCK_STREAM, 0, true};
      struct sockaddr_in server_address{};
      std::memset(&server_address, 0, sizeof(server_address));
      server_address.sin_family = AF_INET;
      server_address.sin_addr.s_addr = INADDR_ANY;
      server_address.sin_port = htons(8080);  // listen on port 8080

      server_socket.bindToAddress(server_address);
      server_socket.listen(5);

      std::cout << "Server listening on port " << 8080 << "...\n";

      while (true) {
         sockaddr_in client_address{};
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
