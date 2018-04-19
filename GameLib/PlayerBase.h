#pragma once
#include <SFML\Graphics.hpp>

class PlayerBase
{
	std::string name;
	sf::Vector2i position;
	int lives;
public:
	PlayerBase();
	~PlayerBase();
};
