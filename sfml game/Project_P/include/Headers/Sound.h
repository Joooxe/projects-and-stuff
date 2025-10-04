#include<SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>

class Sound {
private:
    sf::SoundBuffer enemy_die_buffer;
    sf::SoundBuffer bullet_buffer;
    sf::Sound enemy_die_sound;
    sf::Sound bullet_sound;
public:
    Sound();
    void playEnemyDie();
    void playShoot();
};