#include "game/gameBoard.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>

#include "game/gameSettings.hpp"

namespace ConnectFour {
auto Game::getBoard() -> std::string {
   std::string board{};
   // +1 due to newline characters
   board.reserve(Settings::board_rows * (Settings::board_columns + 1));
   for (int row{Settings::board_rows - 1}; row >= 0; --row) {
      for (std::size_t column{0}; column < Settings::board_columns; column++) {
         board += m_game_board.at(column).at(static_cast<std::size_t>(row));
      }
      board += '\n';
   }
   return board;
}

void Game::addPiece(std::size_t column_index, char symbol) {
   if (!isValidMove(column_index)) return;
   for (auto& cell : m_game_board.at(column_index)) {
      if (cell == Settings::background_character) {
         cell = symbol;
         return;
      }
   }
}

auto Game::checkWin(std::size_t column_index, char symbol) -> bool {
   return checkColumnWin(column_index, symbol) ||
          checkRowWin(column_index, symbol);
}

void Game::setup() {
   for (auto& column : m_game_board) {
      column.fill(Settings::background_character);
   }
}

auto Game::isValidMove(std::size_t column_index) -> bool {
   return column_index < Settings::board_columns &&
          m_game_board.at(column_index).back() ==
              Settings::background_character;
}

auto Game::checkRowWin(std::size_t column_index, char symbol) -> bool {
   BoardColumn column{m_game_board.at(column_index)};
   auto first_match{
       std::ranges::find(std::ranges::reverse_view(column), symbol)};
   if (first_match == column.rend()) return false;
   std::size_t row_index{
       static_cast<size_t>(std::distance(first_match, column.rend())) - 1};

   int count{1};
   for (int col{static_cast<int>(column_index) - 1};
        col >= 0 &&
        m_game_board.at(static_cast<std::size_t>(col)).at(row_index) ==
            symbol &&
        count < Settings::winning_vector_length;
        --col) {
      count++;
   }
   for (std::size_t col{column_index + 1};
        col < Settings::board_columns &&
        m_game_board.at(col).at(row_index) == symbol &&
        count < Settings::winning_vector_length;
        ++col) {
      count++;
   }
   return count >= Settings::winning_vector_length;
}

auto Game::checkColumnWin(std::size_t column_index, char symbol) -> bool {
   BoardColumn column{m_game_board.at(column_index)};
   auto first_match{
       std::ranges::find(std::ranges::reverse_view(column), symbol)};
   if (std::distance(first_match, column.rend()) <
       Settings::winning_vector_length)
      return false;
   return std::all_of(first_match + 1,
                      first_match + Settings::winning_vector_length,
                      [symbol](char cell) { return cell == symbol; });
}

}  // namespace ConnectFour
