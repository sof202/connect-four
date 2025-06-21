#ifndef GAME_MANAGER_H_
#define GAME_MANAGER_H_

#include <cstddef>
#include <mutex>
#include <vector>

#include "game/gameBoard.hpp"
#include "network/clientSocket.hpp"

namespace ConnectFour {
class GameManager {
  public:
   [[nodiscard]] auto isGameActive() const -> bool { return m_game_active; }
   void addPlayer(ClientSocket player);
   auto connectedPlayers() -> int {
      return static_cast<int>(m_players.size());
   }
   void startGame();
   void executePlayerMove();
   void log(const std::string& message);
   void broadcastGameState();

  private:
   bool m_game_active{false};
   std::size_t m_player_turn{0};
   std::vector<char> m_player_pieces{'0', 'x'};
   Game m_game{};
   std::vector<ClientSocket> m_players{};
   std::mutex m_logging_mutex{};
   std::mutex m_player_mutex{};

   auto getPlayerMove() -> int;
   void endGame(bool draw);
};

}  // namespace ConnectFour

#endif  // GAME_MANAGER_H_
