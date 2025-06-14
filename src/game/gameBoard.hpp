#ifndef GAME_BOARD_H_
#define GAME_BOARD_H_

#include <array>
#include <cstddef>
#include <string>

#include "game/gameSettings.hpp"

namespace ConnectFour {
using Board = std::array<std::array<char, Settings::board_rows>,
                         Settings::board_columns>;

using BoardColumn = std::array<char, Settings::board_rows>;

class Game {
  public:
   Game() { setup(); }
   auto getBoard() -> std::string;
   void addPiece(std::size_t column_index, char symbol);
   auto checkWin(std::size_t column_index, char symbol) -> bool;
   auto isFull() -> bool;

  private:
   Board m_game_board{};
   void setup();
   auto isValidMove(std::size_t column_index) -> bool;
   auto checkRowWin(std::size_t column_index, char symbol) -> bool;
   auto checkColumnWin(std::size_t column_index, char symbol) -> bool;
   auto checkDiagonalWin(std::size_t column_index, char symbol) -> bool;
};

}  // namespace ConnectFour

#endif  // GAME_BOARD_H_
