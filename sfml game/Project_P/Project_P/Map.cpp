#include <iostream>
#include <SFML/Graphics.hpp>
#include "Headers/Map.h"
#include "Headers/Player.h"

#include <filesystem>
#include <string>

/*
Map::Map(): Map("../../Resourses/map/map.png") {
    std::cout << std::filesystem::current_path().generic_string() << "\n";

}

Map::Map(const std::string& link) {
    map_texture_.loadFromFile(link);
    map_width = static_cast<float>(map_texture_.getSize().x);
    map_height = static_cast<float>(map_texture_.getSize().y);
    map_.setTexture(map_texture_);
}


void Map::getRectMap(sf::Vector2f coords, sf::Vector2u sizes) {
    float normal_x = std::min(std::max(-coords.x + sizes.x / 2, -map_width + sizes.x - 10.0f), 10.0f);
    float normal_y = std::min(std::max(-coords.y + sizes.y / 2, -map_height + sizes.y - 10.0f), 10.0f);
    map_.setPosition(normal_x, normal_y);
}



void Map::draw(sf::RenderWindow& window) const { //*
    window.draw(map_);
}

const sf::Sprite& Map::getMap() const {
    return map_;
}

void Map::move(mapmove move_direction, float const& delta_time) { //delta_time here untill we setup this in update func
    switch(move_direction) {
        case mapmove::DownLf:
            map_.move(-default_diagonal_move_speed * delta_time, -default_diagonal_move_speed * delta_time);
            //speed_x = 0;
            //speed_y = -max_speed;
            break;
            break;
        case mapmove::DownPressed:
            map_.move(0, -default_move_speed * delta_time);
            //speed_x = 0;
            //speed_y = -max_speed;
            break;
        case mapmove::DownRg:
            map_.move(default_diagonal_move_speed * delta_time, -default_diagonal_move_speed * delta_time);
            //speed_x = max_speed;
            //speed_y = -max_speed;
            break;
        case mapmove::LeftPressed:
            map_.move(0.1 * delta_time, 0);
            //speed_x = -max_speed;
            //speed_y = 0;
            break;
        case mapmove::RightPressed:
            map_.move(-0.1 * delta_time, 0);
            //speed_x = max_speed;
            //speed_y = 0;
            break;
        case mapmove::Stop:
            //speed_x = 0;
            //speed_y = 0;
            break;
        case mapmove::UpLf:
            map_.move(-default_diagonal_move_speed * delta_time, default_diagonal_move_speed * delta_time);
            //speed_x = -max_speed;
            //speed_y = max_speed;
            break;
        case mapmove::UpPressed:

            map_.move(0, 0.1 * delta_time);
            //speed_x = 0;
            //speed_y = max_speed;
            break;
        case mapmove::UpRg:
            map_.move(default_diagonal_move_speed * delta_time, default_diagonal_move_speed * delta_time);
            //speed_x = max_speed;
            //speed_y = max_speed;
            break;
    }
}
*/