#include "game/gameManager.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "network/address.hpp"
#include "network/clientSocket.hpp"
#include "network/message.hpp"
#include "network/networkException.hpp"

namespace ConnectFour {

void GameManager::addPlayer(ClientSocket player) {
   if (m_game_active) {
      std::cerr << "Warning: Game already started.\n";
      return;
   }
   if (m_players.size() >= 2) {
      std::cerr << "Warning: Game already has 2 players, skipping...\n";
      return;
   }
   player.sendMessage(
       {MessageType::info, "Welcome, waiting for other player.\n"});
   m_players.push_back(std::move(player));
   std::cout << "New client connected.\n";
}

void GameManager::initialise() {
   if (m_game_active) {
      std::cerr << "Warning: Game already started.\n";
      return;
   }
   while (m_players.size() < 2) {
      IPv4Address client_address{};
      ClientSocket player{m_server.acceptClient(client_address)};
      addPlayer(std::move(player));
   }
   m_game_active = true;
}

void GameManager::broadcastGameState() {
   if (!m_game_active) {
      std::cerr << "Warning: Game has not yet started.\n";
      return;
   }
   for (std::size_t i{}; i < m_players.size(); ++i) {
      m_players[i].sendMessage({MessageType::info, m_game.getBoard()});
      m_players[i].sendMessage({MessageType::info,
                                "You are playing with the " +
                                    std::string(1, m_player_pieces[i]) +
                                    " pieces.\n"});
      if (i != m_player_turn)
         m_players[i].sendMessage(
             {MessageType::info, "Waiting for other player...\n"});
   }
}

auto GameManager::getPlayerMove() -> int {
   int move{};
   while (true) {
      m_players[m_player_turn].sendMessage(
          {MessageType::requestInput,
           "Provide a column to add your piece to\n"});
      move = std::stoi(
          m_players[m_player_turn].receiveMessage(255).messageText());
      if (m_game.isValidMove(static_cast<std::size_t>(move))) break;
      m_players[m_player_turn].sendMessage(
          {MessageType::info, "That move is invalid.\n"});
   }
   return move;
}

void GameManager::checkGameEnd(int move) {
   if (m_game.isFull()) endGame(true);
   if (m_game.checkWin(move, m_player_pieces[m_player_turn])) endGame(false);
}

void GameManager::endGame(bool draw) {
   broadcastGameState();
   if (draw) {
      for (const auto& player : m_players) {
         player.sendMessage({MessageType::end, "You drew!\n"});
      }
   } else {
      std::size_t winning_player{m_player_turn};
      std::size_t losing_player{(m_player_turn + 1) % 2};
      m_players[winning_player].sendMessage({MessageType::end, "You won!\n"});
      m_players[losing_player].sendMessage({MessageType::end, "You lost!\n"});
   }
   m_game_active = false;
}

void GameManager::handleDisconnect(int disconnected_socket_descriptor,
                                   int reconnect_wait_time_seconds) {
   auto disconnected_player{
       std::ranges::find(m_players,
                         disconnected_socket_descriptor,
                         &ClientSocket::socketDescriptor)};
   if (disconnected_player == m_players.end()) {
      throw NetworkException(
          "Could not find disconnected player in player list");
   }

   std::size_t player_index{static_cast<std::size_t>(
       std::distance(m_players.begin(), disconnected_player))};
   std::cout << "removing player " << player_index << '\n';
   m_players.erase(disconnected_player);

   std::cout << "Attempting to re-establish connection with client (timeout: "
             << reconnect_wait_time_seconds << "s)\n";
   try {
      IPv4Address client_address{};
      ClientSocket player{
          m_server.acceptClient(client_address, reconnect_wait_time_seconds)};
      player.sendMessage({MessageType::info, "Welcome back\n"});
      if (player_index <= m_players.size()) {
         m_players.insert(disconnected_player, std::move(player));
      } else {
         m_players.push_back(std::move(player));
      }
      std::cout << "Client reconnected\n";
   } catch (const NetworkException& e) {
      std::cerr << "Unable to reconnect client: " << e.what() << '\n';
   }
}

void GameManager::gameLoop() {
   try {
      broadcastGameState();
      int move{getPlayerMove()};
      m_game.addPiece(static_cast<std::size_t>(move),
                      m_player_pieces[m_player_turn]);
      checkGameEnd(move);
      updatePlayer();
   } catch (const SocketDisconnectException& e) {
      std::cerr << "Player with socket descriptor " << e.socketDescriptor()
                << " disconnected.\n";
      handleDisconnect(e.socketDescriptor());
   }
}

}  // namespace ConnectFour
