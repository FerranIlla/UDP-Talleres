#pragma once
#include <SFML\Graphics.hpp>
#include <list>
#include <iostream>
#include <Mapa.h>

class ServerMap:public Map {
private:
	
public:
	ServerMap(sf::Vector2i);
	void spawnFood();
	
};