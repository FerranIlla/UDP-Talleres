#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <iostream>
#include <map>
#include <queue>
#include <mutex>
#include "Mapa.h"
#include "player.h"
#include "utils.h"
#include <thread>


#define MaxDataRecived 100
#define ServerAdress "127.0.0.1"
#define ResendTime 250
std::mutex mu;

void sendNew (std::string s, sf::UdpSocket* socket, int &id, std::map<int, outMsg>* outMessages) {
	if (socket->send(&s[0], s.length(), ServerAdress, 50000) != sf::Socket::Done) {
		std::cout << "Ha habido un problema al enviar\n";
	}
	else {
		outMsg msg(s);
		outMessages->emplace(id,msg);
		id++;
	}
	//socket->send()
}

void sendAck(int id, sf::UdpSocket* socket) {
	std::string s = std::to_string(TypeOfMessage::Ack) + "_" + std::to_string(id);
	if (socket->send(&s[0], s.length(), ServerAdress, 50000) != sf::Socket::Done) {
		std::cout << "Ha habido un problema al enviar\n";
	}
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
		if (socket->receive(data, MaxDataRecived, received, adress, port)==sf::Socket::Done) {
			if (adress == ServerAdress) {
				mu.lock();
				std::cout << "Recivido: " << data << std::endl;
				msgList->emplace(data);
				mu.unlock();
			}
		}
		else {
			mu.lock();
			std::cout << "Error al recivir\n";
			mu.unlock();
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
	int msgId = 0;
	
	float timeLastResend = 0;

	
	std::cout << "Introduce un nickname:\n	";
	std::string playerNick;
	std::cin >> playerNick;
	sendNew(std::to_string(TypeOfMessage::Hello)+"_"+std::to_string(msgId)+"_"+playerNick,&socket,msgId,&outMessages);
	std::cout << "nick enviado al servidor\n";

	//gameplay______________________________________________________________________________________________
	Map mapa(window.getSize(), sf::Vector2i(8,6));
	std::map <int, Player*>players;
	sf::Time timer;
	INT32 deltaTime;
	INT32 lastFrameTime=timer.asMilliseconds();
	int myId;

	std::thread thread = std::thread(&receive, &socket, &serverMessages, &window); //abrimos el thread para el receive

	while (window.isOpen()) {
		deltaTime = timer.asMilliseconds() - lastFrameTime;
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
		mu.lock();
		while (!serverMessages.empty()) {
			
			std::vector<std::string> words = commandToWords(serverMessages.front());
			
			if (std::stoi(words[0]) == TypeOfMessage::NewPlayer) {
				std::cout << "Otro player conectado\n";
				int id = std::stoi(words[2]);
				if (players.find(id) == players.end()) {
					Player* player = new Player(sf::Vector2i(std::stoi(words[3]), std::stoi(words[4])), sf::Color(0, 50, 255, 255), mapa.getRectSize(), id);
					players.emplace(id, player);
					sendAck(std::stoi(words[1]), &socket);
				}
				else {
					sendAck(std::stoi(words[1]), &socket);
				}
			}
			else if (std::stoi(words[0])==TypeOfMessage::Hello) {
				std::cout << "Welcome recivido\n";
					myId = std::stoi(words[1]);
					Player* player = new Player(sf::Vector2i(std::stoi(words[2]), std::stoi(words[3])), sf::Color(255, 155, 0, 255), mapa.getRectSize(), myId);
					players.emplace(myId, player);
					outMessages.erase(0); //borramos el Hello
				
			}
			serverMessages.pop();
		}
		mu.unlock();
#pragma endregion

#pragma region CheckearReenvio de mensajes
		timeLastResend += deltaTime;
		if (timeLastResend>ResendTime) {
			for (std::map<int, outMsg>::iterator it = outMessages.begin(); it != outMessages.end(); ++it) {
				reSend(it->second, &socket);
			}
			timeLastResend -= ResendTime;
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