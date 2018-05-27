#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <iostream>
class Button {
public: enum buttonState { pressed, over, unselected, blocked };
private:
	sf::RectangleShape rect;
	sf::Text text;
	sf::Font font;

	sf::Color mainColor;
	sf::Color overColor;
	sf::Color selectedColor;
	sf::Color blockColor;

	float textSize;
	std::string msjToSend;
	buttonState state;

public:

	Button(std::string s, std::string msj, sf::Vector2f rectPos, sf::Vector2f rectSize, sf::Font& f, float textSize = 14, sf::Color mainColor = sf::Color(200, 200, 200, 255), sf::Color overColor = sf::Color(100, 100, 100, 255), sf::Color presedColor = sf::Color(0, 200, 0, 255));
	//~button();
	void update(sf::Vector2f mousePosition);
	void draw(sf::RenderWindow* renderer);

	bool checkClick(sf::Vector2f mousePosition);
	void unselect();

	void block();
	void unblock();

	std::string isClicked();

};
