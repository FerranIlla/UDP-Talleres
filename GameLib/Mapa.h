#pragma once
#include <SFML\Graphics.hpp>

class Map {
protected:
	sf::Vector2i size;

public: 
	Map(sf::Vector2i size);
	bool isPlayerOutside(sf::Vector2f pos, float radius);

	sf::Vector2i getSize();
};