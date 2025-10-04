#include "Headers/Player.h"


Player::Player() {
    hero_sprite.setTexture(animPlayer.getPlayerTexture());
    hero_sprite.setTextureRect(sf::IntRect(24, 18, 9, 24));
    hero_sprite.scale(sf::Vector2f(4, 4));

    load_area.setTexture(animPlayer.getMapTexture());
    upper_bound_height = load_area.getGlobalBounds().height - 2 * start_pos_y;
    upper_bound_width = load_area.getGlobalBounds().width - 2 * start_pos_x;
    load_area.setPosition(start_pos_x, start_pos_y);
    resetPlayer();
}

void Player::resetPlayer() {
    // position = /*проставить базу*/;
    //anim_player = Animator(hero_sprite);
    max_health = 10000;
    cur_health = 10000;
    regeneration = 500;
    max_speed = 10;
    speed_x = 0;
    speed_y = 0;
    damage_modifier = 0;
    firerate_modifier = 0;
    //position = {window.getSize().x / 2 - static_cast<float>(hero_sprite.getTextureRect().width) / 2 , window.getSize().y / 2 - static_cast<float>(hero_sprite.getTextureRect().height) / 2};
}


void
Player::move(playermove upd_move) {
    move_direction = upd_move;
}

void Player::draw(sf::RenderWindow& window) const { //*wwwww
    window.draw(hero_sprite);
}

void Player::loadArea(sf::RenderWindow& window) const {
    window.draw(load_area);
}

void Player::setCentre(const sf::RenderWindow& window) {
    hero_sprite.setPosition(window.getSize().x / 2, window.getSize().y / 2);
}

void Player::hit(int damage) {
    increaseCurHealth(-damage);
}

void Player::die() {
    // приводим здоровье ровно к 0 для корректного отображения
    increaseCurHealth(-getCurHealth());
}

void Player::update(float const& delta_time) {
    int animInd = 0;
    sf::Vector2f add;
    isAnimMove = true;
    float speed = speed_modifier * delta_time;
    switch(move_direction) {
        case playermove::DownLf:
        {
            add = {std::min(position.x, default_diagonal_move_speed * speed),
                                -std::min(upper_bound_height - position.y, default_diagonal_move_speed * speed)};
            animInd = 1;
            break;
        }
        case playermove::DownPressed:
        {
            add = {0,
                                -std::min(upper_bound_height - position.y, default_move_speed * speed)};
            animInd = 2;
            break;
        }
        case playermove::DownRg:
        {
            add = {-std::min(upper_bound_width - position.x, default_diagonal_move_speed * speed),
                                -std::min(upper_bound_height - position.y, default_diagonal_move_speed * speed)};
            animInd = 3;
            break;
        }
        case playermove::LeftPressed:
        {
            add = {std::min(position.x, default_move_speed * speed), 0};
            animInd = 4;
            break;
        }
        case playermove::RightPressed:
        {
            add = {-std::min(upper_bound_width - position.x, default_move_speed * speed), 0};
            animInd = 5;
            break;
        }
        case playermove::UpLf:
        {
            add = {std::min(position.x, default_diagonal_move_speed * speed),
                                std::min(position.y, default_diagonal_move_speed * speed)};
            animInd = 6;
            break;
        }
        case playermove::UpPressed:
        {
            add = {0,
                                std::min(position.y, default_move_speed * speed)};
            animInd = 7;
            break;
        }
        case playermove::UpRg:
        {
            add = {-std::min(upper_bound_width - position.x, default_diagonal_move_speed * speed),
                                std::min(position.y, default_diagonal_move_speed * speed)};
            animInd = 8;
            break;
        }
        case playermove::Stop:
        {
            animInd = 0;
            break;
        }
    }
    if(animInd != 0) {
        position -= add;
        load_area.move(add);
    }

    if(cur_health && isAnimMove) {
        animPlayer.animatePlayer(delta_time, animInd, hero_sprite);
    }
    move_direction = playermove::Stop;
    isAnimMove = false;
}

void Player::setPosition(float new_x, float new_y) {
    float left_limit = 0;
    float right_limit = 2999;
    new_x = std::max(left_limit, new_x);
    new_x = std::min(right_limit, new_x);
    new_y = std::max(left_limit, new_y);
    new_y = std::min(right_limit, new_y);
    hero_sprite.setPosition(new_x, new_y);
}

sf::Vector2f Player::getPosition() const {
    return position;
}

float Player::getMaxSpeed() const {
    return max_speed;
}

void Player::increaseMaxSpeed(float max_speed_change) {
    max_speed += max_speed_change;
}

float Player::getSpeedX() const {
    return speed_x;
}

void Player::setSpeedX(float new_speed_x) {
    speed_x = new_speed_x;
}

float Player::getSpeedY() const {
    return speed_y;
}

void Player::setSpeedY(float new_speed_y) {
    speed_y = new_speed_y;
}

sf::Sprite Player::getSprite() const {
    return hero_sprite;
}

//Animator Player::getAnimPlayer() {
//    return anim_player;
//}

int Player::getMaxHealth() const {
    return max_health;
}

void Player::increaseMaxHealth(int max_health_change) {
    max_health += max_health_change;
}

int Player::getCurHealth() const {
    return cur_health;
}

void Player::increaseCurHealth(int cur_health_change) {
    cur_health += cur_health_change;
    cur_health = std::min(cur_health, getMaxHealth());
    if(getCurHealth() <= 0) {
        die();
    }
}

int Player::getRegeneration() const {
    return regeneration;
}

void Player::increaseRegeneration(int regeneration_change) {
    regeneration += regeneration_change;
}

int Player::getDamageModifier() const {
    return damage_modifier;
}

void Player::increaseDamageModifier(int damage_modifier_change) {
    damage_modifier += damage_modifier_change;
}

int Player::getFirerateModifier() const {
    return firerate_modifier;
}

void Player::increaseFirerateModifier(int firerate_modifier_change) {
    firerate_modifier += firerate_modifier_change;
}

void Player::addX(float add) {
    position.x += add;
}

void Player::addY(float add) {
    position.y += add;
}

float Player::getDiagonalSpeed() {
    return std::sqrt(speed_x * speed_x + speed_y * speed_y);
}
