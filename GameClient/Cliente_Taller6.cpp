#define _USE_MATH_DEFINES
#include <SFML\Graphics.hpp>
#include <iostream>
#include "Mapa.h"
#include "PlayerClient.h"
#include <thread>
#include "NetworkFunctions.h"
#include "ClientMap.h"
#include "SceneManager.h"	
#include "SceneGame.h"

std::mutex mu;



int main() {
	srand(time(NULL));
	sf::Vector2i screenDimensions(800, 600);

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


	std::cout << "Introduce un nickname:\n	";
	std::string playerNick;
	std::cin >> playerNick;
	sendNew(std::to_string(TypeOfMessage::Hello) + "_" + playerNick, &socket, msgId, &outMessages);
	std::cout << "nick enviado al servidor\n";


	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "MegaGame");


	//gameplay______________________________________________________________________________________________
	//Map mapa(window.getSize(), sf::Vector2i(8, 6));
	//std::map <int, Player*>players;
	sf::Clock timer;
	sf::Time deltaTime;
	sf::Time lastFrameTime = sf::milliseconds(0);
	sf::Time timeLastResend = sf::milliseconds(0);
	int myId = -1;
	SceneManager::Instance().scene = new SceneGame(&socket, &serverMessages, &outMessages, myId, msgId, playerNick);
	/*
	sf::Vector2f mousePos(0,0);
	
	bool gameStarted = false;
	
	//food
	std::map<int, sf::CircleShape*> foods;
	//borders
	Walls walls;
	*/
	std::thread thread = std::thread(&receive, &socket, &serverMessages, &window); //abrimos el thread para el receive

	while (window.isOpen()) {
		deltaTime = timer.restart() - lastFrameTime;
		
#pragma region  Input
		SceneManager::Instance().scene->checkInput(&window, deltaTime);
#pragma endregion

#pragma region Mensajes recibidos
		SceneManager::Instance().scene->checkReceivedMsg(&window);
#pragma endregion

#pragma region CheckearReenvio de mensajes
		timeLastResend += deltaTime;
		if (timeLastResend>ResendTime) {
			for (std::map<int, outMsg>::iterator it = outMessages.begin(); it != outMessages.end(); ++it) {
				std::cout << "Reenviando un mensaje" << std::endl;
				sendNormal(it->second.msg, &socket);
			}
			timeLastResend -= ResendTime;
		}
#pragma endregion
#pragma region UpdatePlayers
		SceneManager::Instance().scene->Update(deltaTime);
#pragma endregion

#pragma region Draw
		SceneManager::Instance().scene->draw(&window);


		window.display();
		window.clear();
#pragma endregion


	}
	thread.join();

}