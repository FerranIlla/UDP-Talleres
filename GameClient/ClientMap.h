#pragma once
#include <SFML\Graphics.hpp>
#include <list>
#include <iostream>
#include <Mapa.h>
class Walls {
private:
	std::list<sf::RectangleShape*> rectangles;
	//sf::CircleShape food;
public:
	Walls();
	void draw(sf::RenderWindow*);
};