#include "PlayerClient.h"
#include <iostream>

Player::Player(sf::Vector2i position, sf::Color col, float rad,int i) {
	radius = rad;
	//setup Parameters
	len = 50;
	radius = 15;
	followDistance = 10;
	maxVelocity = 60;
	maxForce = 3;
	velocity = sf::Vector2f(1.f, 0.f)*maxVelocity;
	oriLine.setSize(sf::Vector2f(40, 6));
	oriLine.setOrigin(0,oriLine.getSize().y/2);
	oriLine.setFillColor(sf::Color::Blue);

	//head
	sf::CircleShape head = sf::CircleShape(radius);
	head.setOrigin(radius, radius);
	head.setPosition(position.x, position.y);
	head.setFillColor(sf::Color::Red);// col);
	body.push_back(head);
	//body
	for (int i = 1; i < len; i++) {
		sf::CircleShape circle(radius);
		circle.setOrigin(radius, radius);
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

	window->draw(oriLine);
}

void Player::setTarget(sf::Vector2f tar) {
	target = tar - body.begin()->getPosition(); //target direction
	//calcular angulo entre direcciones
	float cross = velocity.x*target.y - velocity.y*target.x;
	float dot = velocity.x*target.x + velocity.y*target.y;
	float angle = atan2(cross, dot) * 180 / 3.14;

	//check constraints
	float maxAngle = 60;
	float maxAngleRad = maxAngle * 3.14 / 180;
	if (angle > maxAngle) {
		target.x = cos(maxAngleRad)*velocity.x - sin(maxAngleRad)*velocity.y;
		target.y = sin(maxAngleRad)*velocity.x + cos(maxAngleRad)*velocity.y;
	}
	else if (angle < -maxAngle) {
		target.x = cos(-maxAngleRad)*velocity.x - sin(-maxAngleRad)*velocity.y;
		target.y = sin(-maxAngleRad)*velocity.x + cos(-maxAngleRad)*velocity.y;
	}
	//normalize target
	//target = target / sqrt((pow(target.x, 2.f) + pow(target.y, 2.f)));

	
}

void Player::update(float delta) {
	movePlayer(target, delta);
	//body.begin()->rotate(delta*10);
	updateOriLine();
}

void Player::movePlayer(sf::Vector2f targetVelocity, float delta) {
	std::list<sf::CircleShape>::iterator head = body.begin();
	if (targetVelocity != sf::Vector2f(0,0)) {
		sf::Vector2f desiredVel = normalize(targetVelocity)*maxVelocity;
		sf::Vector2f steeringForce = desiredVel - velocity;
		velocity = velocity + normalize(steeringForce)*maxForce;
		velocity = normalize(velocity)*maxVelocity;


		//target = normalize(target)*maxVelocity * delta;
		//maximo de angulo
		/*std::list<sf::CircleShape>::iterator second = ++body.begin();
		sf::Vector2f Direction = first->getPosition() - second->getPosition();
		*/
	}
	head->setPosition(head->getPosition() + velocity*delta);
	

}

//void Player::movePlayer(sf::Vector2f target, float delta) {
//	std::list<sf::CircleShape>::iterator first = body.begin();
//	sf::Vector2f desiredVel = target;// -first->getPosition();
//	
//	float Length = length(desiredVel);
//	float maxDistance= maxVelocity * delta;
//	//maximo de velocidad
//	if (Length > maxDistance) {
//		desiredVel = normalize(desiredVel)*maxDistance;
//	}
//	//target = normalize(target)*maxVelocity * delta;
//	//maximo de angulo
//	/*std::list<sf::CircleShape>::iterator second = ++body.begin();
//	sf::Vector2f Direction = first->getPosition() - second->getPosition();
//	*/
//	first->setPosition(first->getPosition() + desiredVel);
//	//first->setPosition(first->getPosition() + target);
//	orientation = target*delta;
//
//	//for (std::list<sf::CircleShape>::iterator second = ++body.begin(); second != body.end(); ++second) {
//	//	desiredVel = first->getPosition() - second->getPosition();
//	//	sf::Vector2f followTarget = first->getPosition() - (normalize(desiredVel)*followDistance);
//	//	desiredVel = followTarget - second->getPosition();
//	//	Length = length(desiredVel);
//	//	//no queremos que se sobrepongan
//	//	if (Length > maxDistance) {
//	//		desiredVel = normalize(desiredVel)*maxDistance;
//	//	}
//	//		
//	//		second->setPosition(second->getPosition() + desiredVel);
//	//	++first;
//	//}
//	
//
//
//}

void Player::updateOriLine() {
	oriLine.setPosition(body.begin()->getPosition().x, body.begin()->getPosition().y);
	float angle = atan2(velocity.y, velocity.x)*180/3.14;
	//std::cout << "angle: " << angle << std::endl;
	oriLine.setRotation(angle);
}