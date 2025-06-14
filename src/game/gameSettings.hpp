#ifndef GAME_SETTINGS_H_
#define GAME_SETTINGS_H_

#include <cstddef>

namespace ConnectFour::Settings {
static constexpr std::size_t board_rows{6};
static constexpr std::size_t board_columns{8};
static constexpr int winning_vector_length{4};
static constexpr char background_character{'-'};
}  // namespace ConnectFour::Settings

#endif  // GAME_SETTINGS_H_
