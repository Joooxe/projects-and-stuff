#include <SFML/Graphics.hpp>
#include <cmath>


float distan(const sf::Vector2f& first, const sf::Vector2f& second) {
    return sqrt((first.x - second.x) * (first.x - second.x) + (first.y - second.y) * (first.y - second.y));
}

float modul(const sf::Vector2f &vect) {
    return std::sqrt(vect.x * vect.x + vect.y + vect.y);
}


uint64_t now() {
    return duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}


