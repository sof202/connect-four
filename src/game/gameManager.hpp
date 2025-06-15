#ifndef GAME_MANAGER_H_
#define GAME_MANAGER_H_

#include <cstddef>
#include <vector>

#include "game/gameBoard.hpp"
#include "network/clientSocket.hpp"

namespace ConnectFour {
class GameManager {
  public:
   GameManager() = default;

   [[nodiscard]] auto isGameActive() const -> bool { return m_game_active; }
   void addPlayer(ClientSocket player);
   void startGame();
   void broadcastGameState();
   void executePlayerMove();

  private:
   bool m_game_active{false};
   std::size_t m_player_turn{0};
   std::vector<char> m_player_pieces{'0', 'x'};
   Game m_game;
   std::vector<ClientSocket> m_players;

   auto getPlayerMove() -> int;
   void endGame();
};

}  // namespace ConnectFour

#endif  // GAME_MANAGER_H_
