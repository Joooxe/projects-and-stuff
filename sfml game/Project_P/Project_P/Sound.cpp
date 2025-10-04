#include "Headers/Sound.h"

Sound::Sound() {
    #ifdef _WIN32
    enemy_die_buffer.loadFromFile("Resourses/enemy/slime/sound/slime_sound.mp3");
    enemy_die_sound.setBuffer(enemy_die_buffer);
    bullet_buffer.loadFromFile("Resourses/bullet/sound/bullet_sound.mp3");
    bullet_sound.setBuffer(bullet_buffer);
    #elif __linux__
    map_texture_.loadFromFile("../../Resourses/map/map.png");
    side_movement.loadFromFile("../../Resourses/hero/Side Movement.png");
    front_movement.loadFromFile("../../Resourses/hero/Front Movement.png");
    back_movement.loadFromFile("../../Resourses/hero/Back Movement.png");
    slime_texture_.loadFromFile("../../Resourses/enemy/slime/slime-Sheet.png");
    bullet_texture_.loadFromFile("../../Resourses/bullet/default_bullet.png");

    currentPlayerTexture = front_movement;
    #endif
}

void Sound::playEnemyDie() {
    //sf::Sound enemy_die_sound;
    enemy_die_sound.play();
}

void Sound::playShoot() {
    //sf::Sound enemy_die_sound;
    bullet_sound.play();
}


