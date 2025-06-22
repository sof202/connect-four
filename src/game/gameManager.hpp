#ifndef GAME_MANAGER_H_
#define GAME_MANAGER_H_

#include <cstddef>
#include <vector>

#include "game/gameBoard.hpp"
#include "network/clientSocket.hpp"
#include "network/serverSocket.hpp"

namespace ConnectFour {
class GameManager {
  public:
   GameManager(ServerSocket server) : m_server{std::move(server)} {}
   [[nodiscard]] auto isGameActive() const -> bool { return m_game_active; }
   void addPlayer(ClientSocket player);
   auto connectedPlayers() -> int {
      return static_cast<int>(m_players.size());
   }
   void initialise();
   void gameLoop();

  private:
   ServerSocket m_server;
   bool m_game_active{false};
   std::size_t m_player_turn{0};
   std::vector<char> m_player_pieces{'0', 'x'};
   Game m_game;
   std::vector<ClientSocket> m_players;

   void endGame(bool draw);
   void broadcastGameState();
   void checkGameEnd(int move);
   void updatePlayer() { m_player_turn = (m_player_turn + 1) % 2; }
   auto getPlayerMove() -> int;
   void handleDisconnect(int disconnected_socket_descriptor,
                         int reconnect_wait_time_seconds = 30);
};

}  // namespace ConnectFour

#endif  // GAME_MANAGER_H_
