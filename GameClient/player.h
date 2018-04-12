#pragma once
#include "playerBase.h"

class Player : public PlayerBase{
private:
	sf::CircleShape body;
	sf::Vector2i offset;
	sf::Vector2i cellHalfSize;
	
public:
	Player(sf::Vector2i,sf::Color, sf::Vector2f, int);
	void draw(sf::RenderWindow*);
	void update(float delta);
	void movePlayer(sf::Vector2f position);
};