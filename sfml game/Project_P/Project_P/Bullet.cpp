#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>

#include "Headers/Bullet.h"
#include "Headers/Math.h"


Bullet::Bullet(Player &spawn_coords, const sf::Vector2f &target) {
    spawn_time_ = now();
    position_ = spawn_coords.getPosition(); //   + sf::Vector2f(24, 30);
    target_position_ = (target - spawn_coords.getPosition());
    target_position_ /= modul(target_position_);
    bullet_sprite_.setPosition(-1000, -1000);
}

void Bullet::setSprite(Animator &animator) {
    bullet_sprite_.setTexture(animator.getBulletTexture());
    bullet_sprite_.setTextureRect(sf::IntRect(0, 0, 16, 16));
    bullet_sprite_.scale(sf::Vector2f(1.5, 1.5));
}

void Bullet::update(Player& player, float const &delta_time) {
    position_ += target_position_ * delta_time;
    bullet_sprite_.setPosition(player.hero_sprite.getPosition() + position_ - player.getPosition());
}

bool Bullet::alive() {
    return (now() < (spawn_time_ + life_time_));
}

void BulletContainer::spawnNew(Animator &animator, Player& player, const sf::Vector2f &target) {
    uint64_t now = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    if (now > last_spawned_ + Bullet::delay_) {
        last_spawned_ = now;
        container_.push_back(new Bullet(player, target));
        container_[container_.size() - 1]->setSprite(animator);
        sound.playShoot();
    }
}


void BulletContainer::drawAll(sf::RenderWindow &window, Player &player) const {
    for (auto bullet: container_) {
        window.draw(bullet->bullet_sprite_);
    }
}

void BulletContainer::update(const float &delta_time, Player &player, EnemyContainer& enemies) {
    if (container_.empty()) {
        return;
    }

    int live_index = container_.size() - 1;
    sf::Vector2f last_bullet_coords;
    //std::cout << "update bullets" << live_index <<  "\n";
    for (int bullet_index = 0; bullet_index < live_index; ) {
        last_bullet_coords = container_[bullet_index]->position_;
        container_[bullet_index]->update(player, delta_time);
        bool is_macarooned = !container_[bullet_index]->alive();
        for (int enemy_index = 0; enemy_index < enemies.size(); ++enemy_index) {
            if (distan(enemies[enemy_index]->position_, container_[bullet_index]->position_) < Bullet::hit_distance_) {
                enemies[enemy_index]->getDamage(Bullet::hit_damage_);
                is_macarooned = true;
                break;
            }
        }
        if (is_macarooned) {
            std::swap(container_[live_index], container_[bullet_index]);
            --live_index;
            continue;
        }
        ++bullet_index;

    }
    int dead_num = container_.size();
    for (size_t dead = live_index + 1; dead < dead_num; ++dead) {
        container_.pop_back();
    }
}
