#include "ClientMap.h"

 ClientMap::ClientMap(sf::Vector2u screenS,sf::Vector2i s):Map(s) {
		sf::RectangleShape* TopLine = new sf::RectangleShape(sf::Vector2f(5, 800));
		TopLine->setPosition(0, 0);		
		lines.push_back(TopLine);


}

 void ClientMap::draw(sf::RenderWindow* window) {
	 for (std::list<sf::RectangleShape*>::iterator it = lines.begin(); it != lines.end(); ++it) {
		 window->draw(**it);
	 }

 }