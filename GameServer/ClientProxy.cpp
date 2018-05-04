#include "ClientProxy.h"

ClientProxy::ClientProxy(ServerMap* map, Address ad, std::string name, int idPlayer) {
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
	pos.x = rand() % (map->getSize().x - 200) + 100;
	pos.y = rand() % (map->getSize().y - 200) + 100;
	for (int i = 0; i < len; i++) {
		sf::Vector2f* point=new sf::Vector2f(pos.x, pos.y);
		body.push_back(point);
	}
	target = pos;
}

void ClientProxy::resetPing() {
	timeSincePing = sf::milliseconds(0);
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

sf::Vector2f ClientProxy::getHeadPos() {
	return **body.begin();
}

int ClientProxy::getRadius() {
	return radius;
}

void ClientProxy::setTarget(sf::Vector2f t) {
	target = t;
}