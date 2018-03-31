#pragma once
#include <SFML\Graphics.hpp>
#include <list>
#include <iostream>
class Map {
private:
	std::list<sf::RectangleShape*> lines;
	sf::Vector2f rectSize;
	sf::Vector2i size;
public:
	Map(sf::Vector2u,sf::Vector2i);
	void draw(sf::RenderWindow*);
	sf::Vector2i getSize();
	sf::Vector2f getRectSize();
};