#include <netinet/in.h>
#include <sys/socket.h>

#include <cstring>
#include <iostream>
#include <string>

#include "network/address.hpp"
#include "network/serverSocket.hpp"

auto main(int argc, char** argv) -> int {
   if (argc != 3) {
      std::cerr << "Usage: " << argv[0] << "ip_address port\n";
      return 1;
   }
   std::string ip_address{argv[1]};
   auto port{static_cast<uint16_t>(std::stoi(argv[2]))};
   try {
      ServerSocket server_socket{AF_INET, SOCK_STREAM, 0, true};
      IPv4Address server_address{ip_address, port};

      server_socket.bindToAddress(server_address);
      server_socket.listen(5);

      std::cout << "Server (ip: " << ip_address << ") listening on port "
                << port << "...\n";

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
