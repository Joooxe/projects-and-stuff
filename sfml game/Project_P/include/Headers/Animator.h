#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Sound.h"

class Animator {
private:
    sf::Texture map_texture_;
    //sf::Sprite map_sprite_;


    sf::Texture side_movement;
    sf::Texture front_movement;
    sf::Texture back_movement;
    //std::vector<sf::Texture> player_textures_;
    //sf::Sprite player_sprite_;


    // Enemies textures
    sf::Texture slime_texture_;
    //sf::Sprite slime_sprite_;
    sf::Texture bullet_texture_;
public:
    Animator();
    float currentFrame = 0;
    sf::Texture currentPlayerTexture;
    void LoadPLayerTextures();

    sf::Texture& getMapTexture();
    sf::Texture& getPlayerTexture();
    sf::Texture& getEnemyTexture();
    sf::Texture& getBulletTexture();
    void animatePlayer(const float& delta_time, int animInd, sf::Sprite& hero_sprite);
    void animateEnemy(const float& delta_time, int animInd, sf::Sprite& hero);
};