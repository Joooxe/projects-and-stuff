#include "Enemy.h"
#include <chrono>

using namespace std::chrono;


class Bullet {
public:
    constexpr static const uint64_t life_time_ = 10000;
    constexpr static const uint64_t delay_ = 300;
    constexpr static const float hit_distance_ = 100;
    constexpr static const float hit_damage_ = 34;


    uint64_t spawn_time_;
    sf::Vector2f position_;
    sf::Vector2f target_position_;
    sf::Sprite bullet_sprite_;


    Bullet(Player& spawn_coords, const sf::Vector2f& target);

    void setSprite(Animator& animator);

    void update(Player& player_position, float const& delta_time);

    bool alive();
};




class BulletContainer {
public: // *
    std::vector<Bullet*> container_;
    time_t last_spawned_ = 0;
    Sound sound;
public:
    void spawnNew(Animator& animator, Player& player, const sf::Vector2f& target);
    void drawAll(sf::RenderWindow& window, Player& player) const;
    std::vector<Bullet*>& getEnemyList(int index);
    void update(float const& delta_time, Player& player, EnemyContainer& enemies);
};
