#include "game/gameManager.hpp"

#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "network/clientSocket.hpp"
#include "network/message.hpp"

namespace ConnectFour {
void GameManager::log(const std::string& message) {
   std::lock_guard<std::mutex> lock(m_logging_mutex);
   std::cout << message << '\n';
}

void GameManager::addPlayer(ClientSocket player) {
   if (m_game_active) {
      std::cerr << "Warning: Game already started.\n";
      return;
   }
   if (m_players.size() >= 2) {
      std::cerr << "Warning: Game already has 2 players, skipping...\n";
      return;
   }
   std::lock_guard<std::mutex> lock(m_player_mutex);
   m_players.push_back(std::move(player));
   log("New client connected.");
}

void GameManager::startGame() {
   if (m_game_active) {
      std::cerr << "Warning: Game already started.\n";
      return;
   }
   if (m_players.size() < 2) {
      std::cerr << "Warning: Game doesn't have enough players yet.\n";
      return;
   }
   m_game_active = true;
   broadcastGameState();
}

void GameManager::broadcastGameState() {
   if (!m_game_active) {
      std::cerr << "Warning: Game has not yet started.\n";
      return;
   }
   for (std::size_t i{}; i < m_players.size(); ++i) {
      m_players[i].sendMessage({MessageType::board, m_game.getBoard()});
      m_players[i].sendMessage({MessageType::info,
                                "You are playing with the " +
                                    std::string(1, m_player_pieces[i]) +
                                    " pieces.\n"});
   }
}

auto GameManager::getPlayerMove() -> int {
   m_players[m_player_turn].sendMessage(
       {MessageType::requestInput, "Provide a column to add your piece to\n"});
   int move{
       std::stoi(m_players[m_player_turn].receiveMessage(255).messageText())};
   return move;
}

void GameManager::executePlayerMove() {
   std::lock_guard<std::mutex> lock(m_move_mutex);
   int move{};
   while (true) {
      move = getPlayerMove();
      if (m_game.isValidMove(static_cast<std::size_t>(move))) break;
      m_players[m_player_turn].sendMessage(
          {MessageType::info, "That move is invalid.\n"});
   }
   m_game.addPiece(static_cast<std::size_t>(move),
                   m_player_pieces[m_player_turn]);
   broadcastGameState();
   if (m_game.isFull()) endGame(true);
   if (m_game.checkWin(move, m_player_pieces[m_player_turn])) endGame(false);
   m_player_turn = (m_player_turn + 1) % 2;
}

void GameManager::endGame(bool draw) {
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

}  // namespace ConnectFour
