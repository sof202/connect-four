
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <exception>
#include <iostream>
#include <string>
#include <thread>

#include "game/gameSettings.hpp"
#include "network/address.hpp"
#include "network/clientSocket.hpp"

void ignoreLine() {
   std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

auto handleUserInput(const std::string& input_message) -> int {
   int user_response{};
   while (true) {
      std::cout << input_message;
      std::cin >> user_response;
      if (!std::cin) {
         std::cin.clear();
         ignoreLine();
         continue;
      }
      if (static_cast<std::size_t>(user_response) >
              ConnectFour::Settings::board_columns ||
          user_response < 0) {
         ignoreLine();
         std::cout << "Invalid value, index out of range.\n";
         continue;
      }
      return user_response;
   }
}

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
      while (true) {
         std::this_thread::sleep_for(std::chrono::milliseconds(100));
         std::string message{client_socket.receiveMessage(255)};
         if (message.empty()) continue;
         if (message.find(ConnectFour::Settings::background_character) !=
             std::string::npos)
            std::cout << message;
         if (message.find('!') != std::string::npos) {
            std::cout << message;
            return 0;
         }
         if (message.find("column") != std::string::npos) {
            int column_index{handleUserInput(message)};
            client_socket.sendMessage(std::to_string(column_index));
         }
      }

   } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
   } catch (...) {
      std::cerr << "An unknown error occurred\n";
   }
}
