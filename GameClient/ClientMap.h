#pragma once
#include <SFML\Graphics.hpp>
#include <list>
#include <iostream>
#include <Mapa.h>
class ClientMap:Map {
private:
	std::list<sf::RectangleShape*> lines;
	sf::CircleShape food;
public:
	ClientMap(sf::Vector2u,sf::Vector2i):;
	void draw(sf::RenderWindow*);
	sf::Vector2i getSize();
	sf::Vector2f getRectSize();
};