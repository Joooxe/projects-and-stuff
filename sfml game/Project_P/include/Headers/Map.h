//class Map {
//public:
//    float map_width;
//    float map_height;
//    sf::Texture map_texture_;
//    sf::Sprite map_;
//
//public:
//    enum class mapmove {
//        UpPressed,
//        UpRg,
//        UpLf,
//        DownPressed,
//        DownRg,
//        DownLf,
//        LeftPressed,
//        RightPressed,
//        Stop
//    };
//    Map();
//    Map(const std::string& link);
//    void move(mapmove move_direction, float const& delta_time); //delta_time here untill we setup this in update func
//    void getRectMap(sf::Vector2f coords, sf::Vector2u sizes);
//    void draw(sf::RenderWindow& window) const;
//    const sf::Sprite& getMap() const;
//    bool isStaticX(float coord_x, unsigned int window_width);
//    bool isStaticY(float coord_y, unsigned int window_height);
//};
