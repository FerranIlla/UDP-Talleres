#include "ClientMap.h"

 ClientMap::ClientMap(sf::Vector2u screenS,sf::Vector2i s):Map(s) {
		sf::RectangleShape* TopLine = new sf::RectangleShape(sf::Vector2f(5, 800));
		TopLine->setPosition(0, 0);		
		lines.push_back(TopLine);

	for (int i = 0; i < size.y; i++) {
		sf::RectangleShape* line = new sf::RectangleShape(sf::Vector2f(screenSize.x, 5));
		line->setPosition(0, rectSize.y*i);
		lines.push_back(line);
	}

	FinalLine= new sf::RectangleShape(sf::Vector2f(screenSize.x,5));
	FinalLine->setPosition(0,(rectSize.y*size.y) - 5);
	lines.push_back(FinalLine);
}

void ClientMap::draw(sf::RenderWindow* window) {
	for (std::list<sf::RectangleShape*>::iterator it = lines.begin(); it != lines.end(); ++it) {
		window->draw(**it);
	}

}

sf::Vector2i ClientMap::getSize() {
	return size;
}

sf::Vector2f ClientMap::getRectSize() {
	return rectSize;
}