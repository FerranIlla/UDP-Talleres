#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <iostream>
#include <map>
#include <queue>
#include <mutex>
#include "Mapa.h"
#include "player.h"
#include "utils.h"


#define MaxDataRecived 100
#define ServerAdress "127.0.0.1"
//#define ResendTime 500
std::mutex mu;

void sendNew (std::string s, sf::UdpSocket* socket, int id, std::map<int, outMsg>* outMessages) {
	if (socket->send(&s[0], s.length(), ServerAdress, 50000) != sf::Socket::Done) {
		std::cout << "Ha habido un problema al enviar\n";
	}
	else {
		outMsg msg(s);
		outMessages->emplace(id,msg);
	}
	//socket->send()
}

void reSend(outMsg msg, sf::UdpSocket* socket) {
	if (socket->send(&msg.msg[0], msg.msg.length(), ServerAdress, 50000) != sf::Socket::Done) {
		std::cout << "Ha habido un problema al enviar\n";
	}
}

void receive(sf::UdpSocket* socket,std::queue<std::string>* msgList, sf::RenderWindow* window){
	char data[MaxDataRecived];
	sf::IpAddress adress;
	unsigned short port;
	std::size_t received;
	while (window->isOpen()){
		if (socket->receive(data, MaxDataRecived, received, adress, port)) {
			if (adress == ServerAdress) {
				//std::string newMsg = data;
				mu.lock();
				msgList->emplace(data);
				mu.unlock();
			}
		}
	}

}

int main() {
	sf::Vector2i screenDimensions(800, 600);

	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "MegaGame");

	sf::Font font;
	if (!font.loadFromFile("calibril.ttf")) {
		std::cout << "No se puede leer la font\n" << std::endl;
	}
	//network_________________________________________________________________________________________
	//sirve para guardar los mensajes que recivamos del server
	std::queue<std::string> serverMessages;
	//lista con los mensajes salientes, se mantienen aqui hasta que nos aseguramos de que lo ha recibido el server
	std::map<int, outMsg> outMessages;
	//socketudp
	sf::UdpSocket socket;
	int randomId = rand();
	sendNew("Hi_" + std::to_string(randomId), &socket, randomId, &outMessages);

	//gameplay______________________________________________________________________________________________
	Map mapa(window.getSize(), sf::Vector2i(8,6));
	std::map <int, Player*>players;

	while (window.isOpen()) {
#pragma region  Input
		sf::Event evento;
		while (window.pollEvent(evento)) {
			switch (evento.type) {
			case sf::Event::Closed:
				break;
			case sf::Event::MouseMoved:
				break;
			case sf::Event::KeyPressed:
				if (evento.key.code == sf::Keyboard::Return) {
					/*std::string msg = "newP_";
					msg = msg + std::to_string(rand() % 4);
					msg = msg + "_" + std::to_string(rand() % mapa.getSize().x);
					msg = msg + "_" + std::to_string(rand() % mapa.getSize().y);

					serverMessages.emplace(msg);*/
				}
				break;
			default:
				break;
			}
		}

#pragma endregion
#pragma region Mensajes recibidos
		while (!serverMessages.empty()) {
			mu.lock();
			std::vector<std::string> words = commandToWords(serverMessages.front());
			mu.unlock();
			if (words[0] == "newP") {
				int id = std::stoi(words[1]);
				if (players.find(id) != players.end()) {
					Player* player = new Player(sf::Vector2i(std::stoi(words[2]), std::stoi(words[3])), sf::Color(255, 155, 0, 255), mapa.getRectSize(), id);
					players.emplace(id, player);
				}
			}
			serverMessages.pop();
		}
#pragma endregion

#pragma region CheckearReenvio de mensajes
		for (std::map<int, outMsg>::iterator it = outMessages.begin(); it != outMessages.end(); ++it) {

		}
#pragma endregion

#pragma region Draw
		mapa.draw(&window);

		for (std::map <int, Player*>::iterator it = players.begin(); it != players.end(); ++it) {
			(*it).second->draw(&window);
		}

		window.display();
		window.clear();
#pragma endregion

		
	}

}