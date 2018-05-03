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
	float radius;
	sf::Vector2f target;
	float followDistance;
	float maxAngle;
	sf::Vector2f velocity;
public:
	void setPosition(sf::Vector2i);
};
