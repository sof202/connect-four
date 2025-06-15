#include "game/gameManager.hpp"

#include <exception>
#include <iostream>
#include <string>

#include "network/clientSocket.hpp"

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
   m_players.push_back(std::move(player));
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
      m_players[i].sendMessage(m_game.getBoard());
      m_players[i].sendMessage("You are playing with the " +
                               std::string(1, m_player_pieces[i]) +
                               " pieces.\n");
   }
}

auto GameManager::getPlayerMove() -> int {
   int move{};
   while (true) {
      try {
         m_players[m_player_turn].sendMessage(
             "Provide a column to add your piece to\n");
         move = std::stoi(m_players[m_player_turn].receiveMessage(255));
      } catch (const std::exception& e) {
         std::cerr << "Value entered was not an integer (" << e.what()
                   << ")\n";
         continue;
      }
   }
   return move;
}

void GameManager::executePlayerMove() {
   int move{};
   while (true) {
      move = getPlayerMove();
      if (m_game.isValidMove(static_cast<std::size_t>(move))) break;
      m_players[m_player_turn].sendMessage("That move is invalid.\n");
   }
   m_game.addPiece(static_cast<std::size_t>(move),
                   m_player_pieces[m_player_turn]);
   broadcastGameState();
   if (m_game.checkWin(move, m_player_pieces[m_player_turn])) endGame();
   m_player_turn = (m_player_turn + 1) % 2;
}

void GameManager::endGame() {
   std::size_t winning_player{m_player_turn};
   std::size_t losing_player{(m_player_turn + 1) % 2};
   m_players[winning_player].sendMessage("You won!\n");
   m_players[losing_player].sendMessage("You lost!\n");
   m_game_active = false;
}

}  // namespace ConnectFour
