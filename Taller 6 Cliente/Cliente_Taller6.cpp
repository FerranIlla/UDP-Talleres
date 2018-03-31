#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <iostream>
#include <map>
#include "Mapa.h"
#include "player.h"
#include "utils.h"
#include <queue>

int main() {
	sf::Vector2i screenDimensions(800, 600);

	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "MegaGame");

	sf::Font font;
	if (!font.loadFromFile("calibril.ttf")) {
		std::cout << "No se puede leer la font\n" << std::endl;
	}
	//network
	//sirve para guardar los mensajes que recivamos del server
	std::queue<std::string> serverMessages;
	//lista con los mensajes salientes, se mantienen aqui hasta que nos aseguramos de que lo ha recibido el server
	std::map<int, std::string> outMsg;
	


	//gameplay 
	Map mapa(window.getSize(), sf::Vector2i(8,6));
	std::map <int, Player*>players;

	while (window.isOpen()) {
		sf::Event evento;
		while (window.pollEvent(evento)) {
			switch (evento.type){
			case sf::Event::Closed:
				break;
			case sf::Event::MouseMoved:
				break;
			case sf::Event::KeyPressed:
				if (evento.key.code== sf::Keyboard::Return) {
					std::string msg = "newP_";
					msg = msg+std::to_string(rand() % 4);
					msg = msg +"_"+ std::to_string(rand() % mapa.getSize().x);
					msg = msg + "_" + std::to_string(rand() % mapa.getSize().y);

					serverMessages.emplace(msg);
				}
				break;
			default:
				break;
			}
		}
		while (!serverMessages.empty()) {
			std::vector<std::string> words = commandToWords(serverMessages.front());
			if (words[0] == "newP") {
				int id = std::stoi(words[1]);
				if (players.find(id) != players.end()) {
					Player* player = new Player(sf::Vector2i(std::stoi(words[2]), std::stoi(words[3])), sf::Color(255, 155, 0, 255), mapa.getRectSize(), id);
					players.emplace(id, player);
				}
			}
			serverMessages.pop();
		}
		

		mapa.draw(&window);

		for (std::map <int, Player*>::iterator it = players.begin(); it != players.end(); ++it) {
			(*it).second->draw(&window);
		}

		window.display();
		window.clear();
	}

}