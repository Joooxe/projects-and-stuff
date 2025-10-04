#include "Headers/Enemy.h"
#include <cmath>

float distan(const sf::Vector2f& first, const sf::Vector2f& second);


sf::Vector2f Enemy::getPosition() const {
    return position_;
}

void Enemy::setPosition(sf::Vector2f new_position) {
    position_ = new_position;
}

void Enemy::draw(sf::RenderWindow& window, Player& player) {
    if (now() >= hit_time_ + Enemy::hit_delay_) {
        enemy_sprite_.setColor(sf::Color::White);
    }
    sf::Vector2f player_position = player.getPosition();
    sf::Vector2f player_sprite_position = player.hero_sprite.getPosition();
    enemy_sprite_.setPosition(player_sprite_position + position_ - player_position);
    window.draw(enemy_sprite_);
}

float Enemy::getHealth() const {
    return health_;
}




void Enemy::hit(Player& player) {
    if(distan(player.getPosition(), position_) <= hit_distance) {
        //std::cout << "HIT\n";
    }
}

void Enemy::setTexture(sf::Texture& new_texture) {
    enemy_sprite_.setTexture(new_texture);
    enemy_sprite_.setTextureRect(sf::IntRect(1, 9, 29, 15));
    enemy_sprite_.scale(sf::Vector2f(4, 4));
}

Enemy::Enemy(Animator& animator) {
    setTexture(animator.getEnemyTexture());
}
//void Enemy::update(const Player& player, const float& delta_time) {
//    sf::Vector2f delta = (player.getPosition() - position_) / distan(player.getPosition(), position_);
//    position_ += delta * delta_time * static_cast<float>(0.1);
//}

void EnemyContainer::spawnNew(Animator& animator) {
    if(container_.size() == kMaxContainerSize_) {
        return;
    }
    time_t timer;
    time(&timer);
    if(timer - last_spawned_ >= 1) {
        last_spawned_ = timer;
        container_.push_back(new Enemy(animator));
    }
}

void EnemyContainer::drawAll(sf::RenderWindow& window, Player& player) const {
    for(Enemy* enemy : container_) {
        enemy->draw(window, player);
    }
}

std::vector<Enemy*>& EnemyContainer::getEnemyDeque(int index) {
    return container_;
}

void EnemyContainer::update(Player& player, Animator& animator, const float& delta_time) {
    spawnNew(animator);
    size_t swap_index = size();
    for(size_t enemy_index = 0; enemy_index < size(); ) {
        container_[enemy_index]->update(player, animator, delta_time);
        if (!container_[enemy_index]->alive()) {
            std::swap(container_[enemy_index], container_[--swap_index]);
            container_.pop_back();
            sound.playEnemyDie();
        } else {
            ++enemy_index;
        }
    }
}

void Enemy::update(Player& player, Animator& animator, float const& delta_time) {
    int animInd = 0;
    sf::Vector2f target = player.getPosition();
    sf::Vector2f correction_change((float)rand() / (float)RAND_MAX / 2.5f - 0.2, (float)rand() / (float)RAND_MAX / 2.5f - 0.2);
    correction_ += correction_change;
    target += correction_;
    sf::Vector2f delta = (target - position_) / distan(target, position_);
    position_ += delta * delta_time * static_cast<float>(0.1);
    if(position_.x > target.x) {
        animInd = 1;
    } else {
        animInd = 2;
    }
    animEnemy.animateEnemy(delta_time, animInd, enemy_sprite_);

}

void Enemy::getDamage(const float &damage) {
    health_ -= damage;
    std::cout << "health after damage " << health_ << "\n";
    hit_time_ = now();
    enemy_sprite_.setColor(sf::Color::Red);
}

bool Enemy::alive() const {
    return health_ > 0;
}

sf::Vector2f EnemyContainer::getNearest(Player& player) {
    float distance = 10000;
    float calc_dist = 0;
    size_t nearest_index = 0;
    sf::Vector2f player_pos = player.getPosition();
    for(size_t i = 0; i < container_.size(); ++i) {
        if (distance > (calc_dist = distan(container_[i]->position_, player_pos))) {
            distance = calc_dist;
            nearest_index = i;
        }
    }
    return container_[nearest_index]->position_;
}


size_t EnemyContainer::size() const {
    return container_.size();
}

Enemy* EnemyContainer::operator[](size_t index) {
    return container_[index];
}
