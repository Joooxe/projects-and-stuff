#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include "Headers/GameEngine.h"



using key = sf::Keyboard;


GameEngine::GameEngine() {
    window.create(sf::VideoMode(800, 600), "Project_P");
    main_view.setSize(800, 600);
    restart();
}


void GameEngine::run() {
    sf::Clock clock;
    while(window.isOpen()) {
        float delta_time = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        delta_time = delta_time / 600;
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }
        }
        input();
        update(delta_time);
        draw();
    }
}

void GameEngine::input() {
    if(key::isKeyPressed(key::Escape)) {
        window.close();
    }
    if(key::isKeyPressed(key::Space)) {
        player.move(Player::playermove::Dash);
    } else if(key::isKeyPressed(key::A) && key::isKeyPressed(key::W)) {
        player.move(Player::playermove::UpLf);
    } else if(key::isKeyPressed(key::A) && key::isKeyPressed(key::S)) {
        player.move(Player::playermove::DownLf);
    } else if(key::isKeyPressed(key::W) && key::isKeyPressed(key::D)) {
        player.move(Player::playermove::UpRg);
    } else if(key::isKeyPressed(key::D) && key::isKeyPressed(key::S)) {
        player.move(Player::playermove::DownRg);
    } else if(key::isKeyPressed(key::A)) { //*
        player.move(Player::playermove::LeftPressed);
    } else if(key::isKeyPressed(key::D)) {
        player.move(Player::playermove::RightPressed);
    } else if(key::isKeyPressed(key::W)) {
        player.move(Player::playermove::UpPressed);
    } else if(key::isKeyPressed(key::S)) { //*
        player.move(Player::playermove::DownPressed);
    }
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2f a = coords_translator();
        bulletContainer.spawnNew(animator, player, coords_translator());
    }
}

void GameEngine::update(float const& delta_time) {
    player.update(delta_time);
    enemyContainer.update(player, animator, delta_time);
    bulletContainer.update(delta_time, player, enemyContainer);
}

void GameEngine::draw() {
    window.clear();
    player.loadArea(window);
    enemyContainer.drawAll(window, player);
    bulletContainer.drawAll(window, player);
    player.draw(window);
    window.display();
    window.setView(main_view);

}

void GameEngine::restart() {
    player.setCentre(window);
}

sf::Vector2f GameEngine::coords_translator() {
    sf::Vector2f otnos = sf::Vector2f(player.hero_sprite.getPosition().x, player.hero_sprite.getPosition().y) - sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(window).x) * 800 / window.getSize().x, static_cast<float>(sf::Mouse::getPosition(window).y) * 600 / window.getSize().y);
    return player.getPosition() - otnos;
}


int main() {
    std::unique_ptr<GameEngine> myGame = std::make_unique<GameEngine>();
    myGame->run();
}
