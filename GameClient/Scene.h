#pragma once
#include "SFML\Graphics.hpp"

class Scene {
public:
	virtual void Update(sf::Time)=0;
	virtual void checkInput(sf::RenderWindow*, sf::Time) = 0;
	virtual void checkReceivedMsg(sf::RenderWindow*) = 0;
	virtual void draw(sf::RenderWindow*)=0;
};