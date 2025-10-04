#include "Headers/Wave.h"

// this constructor is used only for the first wave
Wave::Wave() : Wave(1) {}

Wave::Wave(int number) : wave_number(number), planned_enemy_amount(number * 13 / 9),
                         cur_position(number % start_positions.size()),
                         delta_position(number % 5) {}

Wave* Wave::update(sf::Vector2f player_coords, bool is_any_enemy) {
  if (cur_enemy_amount < planned_enemy_amount && time_without_spawn >= spawn_step) {
    // spawn enemy on start_positions[cur_positions]
    cur_position = (cur_position + delta_position) % start_positions.size();
    time_without_spawn = 0;
    ++cur_enemy_amount;
  }

  if (cur_enemy_amount == planned_enemy_amount && !is_any_enemy) {
    // WAVE COMPLETED, MAYBE GIVE SOME TEXT ON THE SCREEN AND MODIFIER TO THE PLAYER
    return new Wave(wave_number + 1); // We need somehow delete old wave
  }

  return this;
}
