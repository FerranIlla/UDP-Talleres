#include "ClientMap.h"

 Walls::Walls() {
		sf::RectangleShape* topRect = new sf::RectangleShape(sf::Vector2f(800, 5));
		topRect->setPosition(0, 0);
		topRect->setFillColor(sf::Color::Red);
		rectangles.push_back(topRect);

		sf::RectangleShape* botRect = new sf::RectangleShape(sf::Vector2f(800, 5));
		botRect->setPosition(0, 600-5);
		botRect->setFillColor(sf::Color::Red);
		rectangles.push_back(botRect);

		sf::RectangleShape* rigthRect = new sf::RectangleShape(sf::Vector2f(5, 800));
		rigthRect->setPosition(800-5, 0);
		rigthRect->setFillColor(sf::Color::Red);
		rectangles.push_back(rigthRect);

		sf::RectangleShape* leftRect = new sf::RectangleShape(sf::Vector2f(5, 800));
		leftRect->setPosition(0, 0);
		leftRect->setFillColor(sf::Color::Red);
		rectangles.push_back(leftRect);

}

 void Walls::draw(sf::RenderWindow* window) {
	 for (std::list<sf::RectangleShape*>::iterator it = rectangles.begin(); it != rectangles.end(); ++it) {
		 window->draw(**it);
	 }

 }