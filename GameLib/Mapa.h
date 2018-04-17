#pragma once
#include <SFML\Graphics.hpp>

class Map {
private:
	sf::Vector2i size;
	sf::Vector2i foodPos;
public: 
	Map(sf::Vector2i size);
	bool isPlayerOutside(sf::Vector2i pos, int radi);
};