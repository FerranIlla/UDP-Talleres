#include "PlayerClient.h"


Player::Player(sf::Vector2i position, sf::Color c, float rad,int i, sf::Vector2f tar) {
	//setup Parameters
	len = 5;
	radius = 15;
	followDistance = 10;
	maxVelocity = 150;
	maxForce = 3;
	maxAngle = 20*DEG2RAD;
	col = c;
	target = tar;
	isAlive = true;
	
	//velocity = sf::Vector2f(1.f, 0.f)*maxVelocity;
	oriLine.setSize(sf::Vector2f(40, 6));
	oriLine.setOrigin(0,oriLine.getSize().y/2);
	oriLine.setFillColor(sf::Color::Blue);

	//head
	sf::CircleShape* head =new sf::CircleShape(radius);
	head->setOrigin(radius, radius);
	head->setPosition(position.x, position.y);
	head->setFillColor(col);
	body.push_back(head);
	//body
	for (int i = 1; i < len; i++) {
		sf::CircleShape* circle= new sf::CircleShape(radius);
		circle->setOrigin(radius, radius);
		circle->setPosition(position.x, position.y);
		circle->setFillColor(sf::Color(col.r / 2, col.g / 2, col.b / 2, 255));
		tail = circle;
		body.push_back(circle);
	}

	id = i;
}

void Player::draw(sf::RenderWindow* window) {
	if (isAlive) {
		for (std::list<sf::CircleShape*>::iterator i = body.begin()++; i != body.end(); ++i) {
			window->draw(**i);
		}
		window->draw(**body.begin());
	}
	//window->draw(oriLine);
}

void Player::setTarget(sf::Vector2f tar) {

	target = tar;
	
}
/*
sf::Vector2f Player::getTarget(){
	return target;
}*/

void Player::update(float delta) {
	movePlayer(target, delta);
	//updateOriLine();
}

void Player::grow() {
	len += 3;
	for (int i = 0; i < 3; i++) {
		sf::CircleShape* circle = new sf::CircleShape(radius);
		circle->setOrigin(radius, radius);
		circle->setPosition(tail->getPosition().x, tail->getPosition().y);
		circle->setFillColor(sf::Color(col.r / 2, col.g / 2, col.b / 2, 255));
		tail = circle;
		body.push_back(circle);
	}
}

void Player::movePlayer(sf::Vector2f target, float delta) {
	sf::Vector2f desiredPos = target;

	std::list<sf::CircleShape*>::iterator it = body.begin();
	//comprovamos la distancia hasta el target
	sf::Vector2f myPos = (*it)->getPosition();
	sf::Vector2f desiredVel = desiredPos - myPos;
	float Distance = length(desiredVel);
	//miramos si se tiene que mover la bola
	if (Distance > followDistance) {
		float maxDistance = maxVelocity * delta;
		//lo limitamos respecto a la velocidad
		if (length(desiredVel)>maxDistance) {
			desiredVel = normalize(desiredVel)*maxDistance;
		}
		//lo limitamos respecto al angulo
		velocity =  (*it)->getPosition() - (*(++body.begin()))->getPosition() ;

		float anglebetween = atan2(desiredVel.y,desiredVel.x) - atan2(velocity.y, velocity.x);
		if (anglebetween > M_PI) {
			anglebetween = (M_PI*2)-anglebetween;
		}
		else if (anglebetween <-M_PI) {
			anglebetween = (M_PI * 2) + anglebetween;
		}

		if (abs(anglebetween) > maxAngle) {
			float angleBetweenSign = anglebetween / abs(anglebetween);
			desiredVel = rotate(desiredVel,  (maxAngle*angleBetweenSign)-anglebetween);
		}

		(*it)->setPosition(myPos + desiredVel);
		desiredPos = (*it)->getPosition();
	}
	++it;
    while (it != body.end()) {
		//comprovamos la distancia hasta el target
		sf::Vector2f myPos = (*it)->getPosition();
		sf::Vector2f desiredVel = desiredPos - myPos;
		float Distance = length(desiredVel);
		//miramos si se tiene que mover la bola
		if (Distance > followDistance) {
			//si esta muy lejos nos movemos hacia el punto mas cercano (sin pasarse de f)
			desiredPos = desiredPos - (normalize(desiredVel)*followDistance);
			desiredVel = desiredPos - myPos;
			float maxDistance = maxVelocity * delta;
			//lo limitamos respecto a la velocidad
			if (length(desiredVel)>maxDistance) {
				desiredVel = normalize(desiredVel)*maxDistance;
			}
			(*it)->setPosition(myPos + desiredVel);
			desiredPos = (*it)->getPosition();
		}
		++it;
	}
}

void Player::updateOriLine() {
	oriLine.setPosition((*body.begin())->getPosition().x, (*body.begin())->getPosition().y);
	float angle = atan2(velocity.y, velocity.x)*180/3.14;
	//std::cout << "angle: " << angle << std::endl;
	oriLine.setRotation(angle);
}