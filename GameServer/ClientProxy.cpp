#include "ClientProxy.h"

ClientProxy::ClientProxy(Address ad, std::string name, int idPlayer) {
	address = ad;
	nickname = name;
	timeSincePing = sf::milliseconds(0);
	id = idPlayer;

	//setup Parameters
	len = 5;
	radius = 15;
	followDistance = 10;
	maxVelocity = 150;
	maxForce = 3;
	maxAngle = 20 * DEG2RAD;
	isAlive = true;

	for (int i = 0; i < len; i++) {
		sf::Vector2f* point=new sf::Vector2f(pos.x, pos.y);
		body.push_back(point);
		tail = point;
	}
	target = pos;
}

void ClientProxy::resetPing() {
	timeSincePing = sf::milliseconds(0);
}

void ClientProxy::SetPosition(sf::Vector2f aPos) {
	pos = aPos;
	for (std::list<sf::Vector2f*>::iterator it = body.begin(); it!=body.end();++it) {
		(*it)->x = aPos.x;
		(*it)->y = aPos.y;

	}
}


bool ClientProxy::isDisconected(sf::Time deltaTime) {
	timeSincePing += deltaTime;
	return timeSincePing > DisconnectTime;
}

void ClientProxy::addOutMsg(std::string s, int id) {
	outMessages.emplace(id, s);
}

void ClientProxy::movePlayer(float delta) {
	sf::Vector2f desiredPos = target;

	std::list<sf::Vector2f*>::iterator it = body.begin();
	//comprovamos la distancia hasta el target
	sf::Vector2f myPos = (**it);
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
		velocity = (**it) - (**(++body.begin()));

		float anglebetween = atan2(desiredVel.y, desiredVel.x) - atan2(velocity.y, velocity.x);
		if (anglebetween > M_PI) {
			anglebetween = (M_PI * 2) - anglebetween;
		}
		else if (anglebetween <-M_PI) {
			anglebetween = (M_PI * 2) + anglebetween;
		}

		if (abs(anglebetween) > maxAngle) {
			float angleBetweenSign = anglebetween / abs(anglebetween);
			desiredVel = rotate(desiredVel, (maxAngle*angleBetweenSign) - anglebetween);
		}

		(**it)=myPos + desiredVel;
		desiredPos = **it;
	}
	++it;
	while (it != body.end()) {
		//comprovamos la distancia hasta el target
		sf::Vector2f myPos = **it;
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
			**it=myPos + desiredVel;
			desiredPos = **it;
		}
		++it;
	}
}

int ClientProxy::checkFoodCollision(std::map<int, sf::Vector2f*>&foods) {
	for (std::map<int, sf::Vector2f*>::iterator it = foods.begin(); it != foods.end();++it) {
		sf::Vector2f distance = *it->second - this->getHeadPos();
		if (length(distance) < radius + 8) {
			return it->first;
		}
	}
	return -1;
}

bool ClientProxy::checkPlayersCollision(std::map<Address, ClientProxy*>&players) {
	for (std::map<Address, ClientProxy*>::iterator it = players.begin(); it != players.end(); ++it) {
		if (it->second->id!=id && it->second->isAlive) {
			sf::Vector2f distanceHeads = it->second->getHeadPos() - getHeadPos();
			
			if (length(distanceHeads) < (followDistance * 2* it->second->len)) {
				for (std::list<sf::Vector2f*>::iterator bodypoint = it->second->body.begin();bodypoint!=it->second->body.end() ; ++bodypoint) {
					sf::Vector2f distanceHeadPoint = **bodypoint - this->getHeadPos();
					//cambiar el 1.5 para cambiar el offset que se pueden atravesar 2 gusanos (mas bajo mas se pueden atravesar)
					if (length(distanceHeadPoint) < radius*1.5) {
						return true;
					}
				}
			}
		}

	}
	return false;
}


void ClientProxy::grow() {
	len += 3;
	for (int i = 0; i < 3; i++) {
		sf::Vector2f* point = new sf::Vector2f(tail->x,tail->y);
		tail = point;
		body.push_back(point);
	}
}

sf::Vector2f ClientProxy::getHeadPos() {
	return **body.begin();
}

int ClientProxy::getRadius() {
	return radius;
}

void ClientProxy::setTarget(sf::Vector2f t) {
	target = t;
}