#pragma once
#include <SFML\Graphics.hpp>
#include <list>
#include <iostream>
class Map {
private:
	sf::Vector2i size;
public:
	Map(sf::Vector2i);
	sf::Vector2i getSize();
};