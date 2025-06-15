#include <netinet/in.h>
#include <sys/socket.h>

#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "game/gameManager.hpp"
#include "network/address.hpp"
#include "network/clientSocket.hpp"
#include "network/serverSocket.hpp"

int connected_clients{};
std::mutex cout_mutex;
std::mutex clients_mutex;

void handleClient(ClientSocket client_socket, ConnectFour::GameManager& game) {
   std::cout << "New client connected.\n";

   client_socket.sendMessage("Welcome to the game.");
   {
      std::lock_guard<std::mutex> lock(cout_mutex);
      std::cout << client_socket.receiveMessage(255) << '\n';
   }
   game.addPlayer(std::move(client_socket));
   while (true) {
      {
         std::lock_guard<std::mutex> lock(clients_mutex);
         if (connected_clients >= 2) break;
      }
   }
   game.startGame();
}

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

      std::vector<std::thread> client_threads{};
      ConnectFour::GameManager game;

      for (int i{0}; i < 2; i++) {
         IPv4Address client_address{};
         ClientSocket client_socket{
             server_socket.acceptClient(client_address)};
         connected_clients++;
         client_threads.emplace_back(
             handleClient, std::move(client_socket), std::ref(game));
      }

      for (auto& thread : client_threads) {
         if (thread.joinable()) thread.join();
      }

      std::cout << "Game ended...\n";
   } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
   } catch (...) {
      std::cerr << "An unknown error occurred\n";
   }
}
