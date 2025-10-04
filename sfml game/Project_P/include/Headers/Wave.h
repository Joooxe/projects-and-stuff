#include <iostream>
#include <vector>
//Also include enemy/enemycontainer (what we need to spawn enemy)

class Wave {
private:
  int wave_number;
  int planned_enemy_amount;
  int cur_enemy_amount = 0;
  int cur_position;
  int delta_position;
  int time_without_spawn = 0;
  int spawn_step = 10;

  static std::vector<sf::Vector2f> start_positions; //FILL THIS WITH ALL AVAILABLE ENEMY SPAWN POSITIONS

  Wave(int number);
public:
  Wave();
  Wave* update(sf::Vector2f player_coords, bool is_any_enemy); // CALL IT FROM GAME ENGINE, THIS RETURNS ACTUAL WAVE, SO
  // DO SMTH LIKE THIS: "current_wave = current_wave.update();"
};
