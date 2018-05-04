#pragma once
#define _USE_MATH_DEFINES
#include <list>
#include <PlayerBase.h>
#include "utils.h"
#include <cmath>


class Player : public PlayerBase{
private:
	std::list<sf::CircleShape*> body;
	sf::CircleShape*  tail;
	sf::Color col;
	sf::RectangleShape oriLine;
public:
	bool isAlive;

	Player(sf::Vector2i,sf::Color, float, int, sf::Vector2f);
	void draw(sf::RenderWindow*);
	void update(float delta);
	void movePlayer(sf::Vector2f position, float delta);
	void setTarget(sf::Vector2f);

	void grow();

	void updateOriLine();
};