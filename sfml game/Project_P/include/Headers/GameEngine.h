// Project_P.h : включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта.
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Bullet.h"


/*class GameEngine {
public:
    GameEngine();
    void run();
private:
    enum class GameState {
        paused,
        run,
        victory_screen,
    };
    *//*float width_;
    float height_;*//*
    sf::RenderWindow window;
    Player player;
    sf::View main_view = sf::View(sf::FloatRect(0, 0, 800, 600));
    sf::IntRect playground_size;
    sf::VertexArray background;
  EnemyContainer enemyContainer;
    void input(float const& delta_time); //delta_time here untill we setup this in update func
    void update(float const& delta_time);
    void draw();
    void restart();
};*/


class GameEngine {
public:
    GameEngine();

    void run();

private:
    enum class GameState {
        paused,
        run,
        victory_screen,
    };
    Animator animator;
    sf::RenderWindow window;
    Player player = Player();
    sf::View main_view = sf::View(sf::FloatRect(0, 0, 800, 600));
    sf::IntRect playground_size;
    sf::VertexArray background;
    EnemyContainer enemyContainer;
    BulletContainer bulletContainer;
    float currentFrame;

    void input(); //delta_time here untill we setup this in update func
    void update(float const& delta_time);
    sf::Vector2f coords_translator();

    void draw();

    void restart();
};