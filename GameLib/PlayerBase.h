#pragma once
#include <SFML\Graphics.hpp>

class PlayerBase
{
protected:
	std::string name;
	sf::Vector2i position;
	float maxVelocity;
	int lives;
	int len;
	int id;
public:
	void setPosition(sf::Vector2i);
};
