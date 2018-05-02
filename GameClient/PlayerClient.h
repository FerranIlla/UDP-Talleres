#pragma once
#define _USE_MATH_DEFINES
#include <list>
#include <PlayerBase.h>
#include "utils.h"
#include <cmath>


class Player : public PlayerBase{
private:
	std::list<sf::CircleShape> body;
	float radius;
	float followDistance;
	sf::Vector2f target;
	sf::Vector2f velocity;
	float maxAngle;

	sf::RectangleShape oriLine;
public:
	Player(sf::Vector2i,sf::Color, float, int);
	void draw(sf::RenderWindow*);
	void update(float delta);
	void movePlayer(sf::Vector2f position, float delta);
	void setTarget(sf::Vector2f);

	void updateOriLine();
};