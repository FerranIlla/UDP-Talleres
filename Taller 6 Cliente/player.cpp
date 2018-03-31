#include "player.h"

Player::Player(sf::Vector2i position, sf::Color col, sf::Vector2f rectSize,int i) {
	float radius = std::min(rectSize.x, rectSize.y) / 2;
	body = sf::CircleShape(radius);
	body.setPosition(position.x*rectSize.x, position.y*rectSize.y);
	body.setFillColor(col);
	id = i;
}

void Player::draw(sf::RenderWindow* window) {
	window->draw(body);
}