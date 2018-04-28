#pragma once
#include <SFML\Graphics.hpp>

class PlayerBase
{
protected:
	std::string name;
	float maxVelocity;
	float maxForce;
	int lives;
	int len;
	int id;
public:
	void setPosition(sf::Vector2i);
};
