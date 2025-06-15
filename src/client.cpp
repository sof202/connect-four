
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <exception>
#include <iostream>

#include "network/address.hpp"
#include "network/clientSocket.hpp"

auto main(int argc, char** argv) -> int {
   if (argc != 3) {
      std::cerr << "Usage: " << argv[0] << "ip_address port\n";
      return 1;
   }
   std::string ip_address{argv[1]};
   auto port{static_cast<uint16_t>(std::stoi(argv[2]))};
   try {
      ClientSocket client_socket{AF_INET, SOCK_STREAM};
      IPv4Address server_address{ip_address, port};

      client_socket.connectToServer(server_address);
      client_socket.sendMessage("Client connected.");
      std::cout << client_socket.receiveMessage(255) << '\n';

      while (true) {
         sleep(1);
         std::string message{client_socket.receiveMessage(255)};
         std::cout << message << '\n';
         if (message.find("-") != std::string::npos) {
            break;
         }
      }

   } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
   } catch (...) {
      std::cerr << "An unknown error occurred\n";
   }
}
