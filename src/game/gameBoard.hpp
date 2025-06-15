#ifndef GAME_BOARD_H_
#define GAME_BOARD_H_

#include <array>
#include <cstddef>
#include <optional>
#include <string>

#include "game/gameSettings.hpp"

namespace ConnectFour {
using Board = std::array<std::array<char, Settings::board_rows>,
                         Settings::board_columns>;

using BoardColumn = std::array<char, Settings::board_rows>;

struct Cell {
   int row;
   int col;
};

struct Direction {
   int x;
   int y;
};

class Game {
  public:
   Game() { setup(); }
   auto getBoard() -> std::string;
   void addPiece(std::size_t column_index, char symbol);
   auto checkWin(int column_index, char symbol) -> bool;
   auto isValidMove(std::size_t column_index) -> bool;
   auto isFull() -> bool;

  private:
   Board m_game_board{};
   void setup();
   auto getTopCellWithSymbol(int column_index, char symbol)
       -> std::optional<Cell>;
   auto countInDirection(Cell cell, Direction direction, char symbol) -> int;
   auto checkRowWin(int column_index, char symbol) -> bool;
   auto checkColumnWin(int column_index, char symbol) -> bool;
   auto checkDiagonalWin(int column_index, char symbol) -> bool;
};

}  // namespace ConnectFour

#endif  // GAME_BOARD_H_
