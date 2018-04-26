#pragma once
#include <list>
#include <PlayerBase.h>
#include "utils.h"
class Player : public PlayerBase{
private:
	std::list<sf::CircleShape> body;
	float radius;
	float followDistance;
	sf::Vector2f target;
public:
	Player(sf::Vector2i,sf::Color, float, int);
	void draw(sf::RenderWindow*);
	void update(float delta);
	void movePlayer(sf::Vector2f position, float delta);
	void setTarget(sf::Vector2f);
};