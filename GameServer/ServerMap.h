#pragma once
#include <SFML\Graphics.hpp>
#include <list>
#include <iostream>
#include <Mapa.h>

class ServerMap:public Map {
private:
	
	
	sf::Time lastFoodCreation;
	sf::Time foodCreationInterval;
	
public:
	std::map<int, sf::Vector2f*> food;
	int foodId;
	ServerMap(sf::Vector2i);
	void spawnFood();

	bool update(sf::Time delta);

	
};