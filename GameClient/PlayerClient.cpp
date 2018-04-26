#include "PlayerClient.h"

Player::Player(sf::Vector2i position, sf::Color col, float rad,int i) {
	float radius = rad;
	//setup Parameters
	len = 50;
	radius = 15;
	followDistance = 10;
	maxVelocity = 40;
	
	//head
	sf::CircleShape head = sf::CircleShape(radius);
	head.setPosition(position.x, position.y);
	head.setFillColor(col);
	body.push_back(head);
	//body
	for (int i = 1; i < len; i++) {
		sf::CircleShape circle(radius);
		circle.setPosition(position.x, position.y);
		circle.setFillColor(sf::Color(col.r / 2, col.g / 2, col.b / 2, 1));
		body.push_back(circle);
	}
	id = i;
}

void Player::draw(sf::RenderWindow* window) {
	for (std::list<sf::CircleShape>::iterator i = body.begin()++; i != body.end(); ++i) {
		window->draw(*i);
	}
	window->draw(*body.begin());
}

void Player::setTarget(sf::Vector2f tar) {
	target = tar;
}

void Player::update(float delta) {
	movePlayer(target, delta);
}

void Player::movePlayer(sf::Vector2f position, float delta) {
	std::list<sf::CircleShape>::iterator first = body.begin();
	sf::Vector2f desiredVel = position - first->getPosition() ;
	
	float Length = length(desiredVel);
	float maxDistance= maxVelocity * delta;
	//maximo de velocidad
	if (Length > maxDistance) {
		desiredVel = normalize(desiredVel)*maxDistance;
	}
	//maximo de angulo
	/*std::list<sf::CircleShape>::iterator second = ++body.begin();
	sf::Vector2f Direction = first->getPosition() - second->getPosition();
	*/
	first->setPosition(first->getPosition() + desiredVel);

	for (std::list<sf::CircleShape>::iterator second = ++body.begin(); second != body.end(); ++second) {
		desiredVel = first->getPosition() - second->getPosition();
		sf::Vector2f followTarget = first->getPosition() - (normalize(desiredVel)*followDistance);
		desiredVel = followTarget - second->getPosition();
		Length = length(desiredVel);
		//no queremos que se sobrepongan
		if (Length > maxDistance) {
			desiredVel = normalize(desiredVel)*maxDistance;
		}
			
			second->setPosition(second->getPosition() + desiredVel);
		++first;
	}
	


}