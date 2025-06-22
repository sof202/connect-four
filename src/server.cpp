#include <netinet/in.h>
#include <sys/socket.h>

#include <cstring>
#include <iostream>
#include <string>

#include "game/gameManager.hpp"
#include "network/address.hpp"
#include "network/clientSocket.hpp"
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

      ConnectFour::GameManager game{std::move(server_socket)};

      game.initialise();
      while (game.isGameActive()) {
         game.gameLoop();
      }
      game.log("Game finished.");

   } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
   } catch (...) {
      std::cerr << "An unknown error occurred\n";
   }
}
