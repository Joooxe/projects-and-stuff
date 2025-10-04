#include <iostream>
#include <cmath>
#include "Animator.h"


const float default_move_speed = 0.1;
const float default_diagonal_move_speed = 0.0707;

const float start_pos_x = 200;
const float start_pos_y = 200;

uint64_t now();

class Player {
public:
    float upper_bound_height; // граница прогружаемой области
    float upper_bound_width;
    Animator animPlayer;


    enum class playermove {
        UpPressed,
        UpRg,
        UpLf,
        DownPressed,
        DownRg,
        DownLf,
        LeftPressed,
        RightPressed,
        Dash,
        Stop
    };
    // конструктор
    Player();
    ~Player() {
        std::cout << "hero ended up being macarooned";
    }
    sf::Sprite hero_sprite;
    sf::Sprite load_area;
    // установка параметров по умолчанию
    void resetPlayer(); //const sf::RenderWindow& window);
    // снаружи нам говорят, какая клавиша была нажата
    void move(playermove move_direction); //delta_time here untill we setup this in update func
    // получение урона игроком
    void hit(int damage);
    // смерть игрока
    void die();
    // выполняется в каждом цикле Гейм Енджина
    void update(float const& deltaTime);
    // геттеры и сеттеры для всех полей, которым это нужно
    sf::Vector2f getPosition() const;
    void setPosition(float new_x, float new_y);
    float getMaxSpeed() const;
    void increaseMaxSpeed(float max_speed_change);
    float getSpeedX() const;
    void setSpeedX(float new_speed_x);
    float getSpeedY() const;
    void setSpeedY(float new_speed_y);
    sf::Sprite getSprite() const;
    Animator getAnimPlayer() const;
    int getMaxHealth() const;
    void increaseMaxHealth(int max_health_change);
    int getCurHealth() const;
    void increaseCurHealth(int cur_health_change);
    int getRegeneration() const;
    void increaseRegeneration(int regeneration_change);
    int getDamageModifier() const;
    void increaseDamageModifier(int damage_modifier_change);
    int getFirerateModifier() const;
    void increaseFirerateModifier(int firerate_modifier_change);
    void draw(sf::RenderWindow& window) const;
    void loadArea(sf::RenderWindow& window) const;
    void setCentre(const sf::RenderWindow& window);

    float getDiagonalSpeed();

    void addX(float add);
    void addY(float add);
private:
    sf::Vector2f position;
    playermove move_direction;
    bool isAnimMove = false;


    std::time_t last_dashed;
    // позиция по x и y
    // максимальная скорость
    float max_speed;
    // текущая скорость по x
    float speed_x = 1;
    // текущая скорость по y
    float speed_y = 1;

    float speed_modifier = 1.5;


    // максимум здоровья
    int max_health;
    // текущее здоровье
    int cur_health;
    // регенерация
    int regeneration;
    // бонус к урону
    int damage_modifier;
    // бонус к скорострельности
    int firerate_modifier;
};
