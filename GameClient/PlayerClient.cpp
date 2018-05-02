#include "PlayerClient.h"


Player::Player(sf::Vector2i position, sf::Color col, float rad,int i) {
	radius = rad;
	//setup Parameters
	len = 50;
	radius = 15;
	followDistance = 10;
	maxVelocity = 200;
	maxForce = 3;
	maxAngle = 10*DEG2RAD;
	
	//velocity = sf::Vector2f(1.f, 0.f)*maxVelocity;
	oriLine.setSize(sf::Vector2f(40, 6));
	oriLine.setOrigin(0,oriLine.getSize().y/2);
	oriLine.setFillColor(sf::Color::Blue);

	//head
	sf::CircleShape head = sf::CircleShape(radius);
	head.setOrigin(radius, radius);
	head.setPosition(position.x, position.y);
	head.setFillColor(col);
	body.push_back(head);
	//body
	for (int i = 1; i < len; i++) {
		sf::CircleShape circle(radius);
		circle.setOrigin(radius, radius);
		circle.setPosition(position.x, position.y);
		circle.setFillColor(sf::Color(col.r / 2, col.g / 2, col.b / 2, 255));
		body.push_back(circle);
	}
	id = i;
}

void Player::draw(sf::RenderWindow* window) {
	for (std::list<sf::CircleShape>::iterator i = body.begin()++; i != body.end(); ++i) {
		window->draw(*i);
	}
	window->draw(*body.begin());

	//window->draw(oriLine);
}

void Player::setTarget(sf::Vector2f tar) {
	/*target = tar - body.begin()->getPosition(); //target direction
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
	//target = target / sqrt((pow(target.x, 2.f) + pow(target.y, 2.f)));*/
	target = tar;
	
}

void Player::update(float delta) {
	movePlayer(target, delta);
	//updateOriLine();
}


void Player::movePlayer(sf::Vector2f target, float delta) {
	sf::Vector2f desiredPos = target;

	std::list<sf::CircleShape>::iterator it = body.begin();
	//comprovamos la distancia hasta el target
	sf::Vector2f myPos = it->getPosition();
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
		//lo limitamos respecto al angulo
		/*velocity = (body.begin()++)->getPosition() - it->getPosition();

		float anglebetween = atan2(desiredVel.y,desiredVel.x) - atan2(velocity.y, velocity.x);
		if (maxAngle > M_PI) {
			maxAngle = M_PI - maxAngle;
		}
		if (abs(anglebetween) > maxAngle) {
			float angleBetweenSign = anglebetween / abs(anglebetween);
			desiredVel = rotate(desiredVel, anglebetween+ (maxAngle*angleBetweenSign));
		}*/

		it->setPosition(myPos + desiredVel);
		desiredPos = it->getPosition();
		++it;
	}

    while (it != body.end()) {
		//comprovamos la distancia hasta el target
		sf::Vector2f myPos = it->getPosition();
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
			it->setPosition(myPos + desiredVel);
			desiredPos = it->getPosition();
		}
		++it;
	}
}

void Player::updateOriLine() {
	oriLine.setPosition(body.begin()->getPosition().x, body.begin()->getPosition().y);
	float angle = atan2(velocity.y, velocity.x)*180/3.14;
	//std::cout << "angle: " << angle << std::endl;
	oriLine.setRotation(angle);
}