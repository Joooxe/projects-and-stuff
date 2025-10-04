#include <iostream>
#include <vector>
#include <random>
#include <SFML/Graphics.hpp>
#include <ctime>
#include "Player.h"

struct Enemy {
    constexpr static size_t max_hit_frames_count = 100; // texture color reset after this count of frames
    constexpr static uint64_t hit_delay_ = 100;

    sf::Sprite enemy_sprite_;
    Animator animEnemy;

    sf::Vector2f position_ = {0, 0};
    sf::Vector2f correction_ = {0, 0};
    uint64_t hit_time_ = 0;


    float health_ = 100;
    float damage_ = 100;
    float shield_ = 100;
    float hit_distance = 5;

    //size_t hit_frames_count_ = 0; // number of frames when slime texture is damaged

    Enemy(Animator& animator);

    [[nodiscard]] sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f);
    void draw(sf::RenderWindow& window, Player& player);
    [[nodiscard]] float getHealth() const;
    void hit(Player& player);
    bool alive() const;
    void getDamage(const float& damage); // update health after getting damage
    void setTexture(sf::Texture& texture);
    void update(Player& player, Animator& animator, const float& delta_time);
};


class EnemyContainer {
private:
    static const size_t kMaxContainerSize_ = 100;
    std::vector<Enemy*> container_;
    time_t last_spawned_ = 0;
    Sound sound;
public:
    size_t size() const; // size of enemy container
    void spawnNew(Animator& animator);
    void drawAll(sf::RenderWindow& window, Player& player) const;
    std::vector<Enemy*>& getEnemyDeque(int index);
    void update(Player& player, Animator& animator, float const& delta_time);
    sf::Vector2f getNearest(Player& player);
    Enemy* operator[](size_t index);
};
