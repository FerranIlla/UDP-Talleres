#include "SceneGame.h"

SceneGame::SceneGame(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f)  {
	socket = sock;
	serverMessages = sMsg;
	outMessages = outMsg;
	msgId = msgid;
	myId = pID;
	playerNick = nick;
	font = f;

	timeLastMoveSend = sf::microseconds(0);
	mousePos = sf::Vector2f(0, 0);

	gameStarted = false;

}
void SceneGame::Update(sf::Time deltaTime) {
	if (gameStarted) {
		for (std::map <int, Player*>::iterator it = players.begin(); it != players.end(); ++it) {
			//it->second->setTarget(sf::Vector2f(0, 0));
			(*it).second->update(deltaTime.asSeconds());
		}
	}
}


void SceneGame::checkInput(sf::RenderWindow* window, sf::Time deltaTime) {
	sf::Event evento;
	timeLastMoveSend += deltaTime;
	while (window->pollEvent(evento)) {
		switch (evento.type) {
		case sf::Event::Closed:
			sendNormal(std::to_string(TypeOfMessage::Disconnect), socket);
			window->close();
			break;
		case sf::Event::MouseMoved:
			mousePos = sf::Vector2f(evento.mouseMove.x, evento.mouseMove.y);
			if (myId != -1) {
				if (gameStarted) {
					std::map <int, Player*>::iterator p = players.find(myId);
					p->second->setTarget(mousePos);
					if ((timeLastMoveSend > sendMovementTime)) {
						std::string s = std::to_string(TypeOfMessage::Move) + "_" + std::to_string((int)evento.mouseMove.x) + "_" + std::to_string((int)evento.mouseMove.y);
						sendNormal(s, socket);
						timeLastMoveSend -= sendMovementTime;
					}
				}
			}
			break;
		case sf::Event::KeyPressed:
			if (evento.key.code == sf::Keyboard::Return) {
				/*std::string msg = "newP_";
				msg = msg + std::to_string(rand() % 4);
				msg = msg + "_" + std::to_string(rand() % mapa.getSize().x);
				msg = msg + "_" + std::to_string(rand() % mapa.getSize().y);

				serverMessages.emplace(msg);*/
				//players.find(myId)->second->grow();
				SceneManager::Instance().changeToMenu(socket, serverMessages, outMessages, myId, msgId, playerNick, font);
			}
			break;
		default:
			break;
		}
	}
}

void SceneGame::checkReceivedMsg(sf::RenderWindow* window) {
	while (!serverMessages->empty()) {
		mu.lock();
		std::vector<std::string> words = commandToWords(serverMessages->front());
		mu.unlock();
		TypeOfMessage type = (TypeOfMessage)std::stoi(words[0]);
		if (type == TypeOfMessage::Ack) {
			outMessages->erase(std::stoi(words[1]));
		}
		else if (type == TypeOfMessage::Ping) {
			//std::cout << "Ping Recivido";
			sendNormal(std::to_string(TypeOfMessage::Ping), socket);
		}
		else if (type == TypeOfMessage::Move) {
			int idPlayerMove = std::stoi(words[1]);

			std::map<int, Player*>::iterator p = players.find(idPlayerMove);
			if (p != players.end()) {
				p->second->setTarget(sf::Vector2f(std::stoi(words[2]), std::stoi(words[3])));
			}

		}
		else if (type == TypeOfMessage::Food) {
			sf::CircleShape* newFood = new sf::CircleShape(8);
			newFood->setFillColor(sf::Color(255, 255, 255, 200));
			newFood->setPosition(std::stoi(words[3]), std::stoi(words[4]));
			foods.emplace(std::stoi(words[2]), newFood);
			sendAck(std::stoi(words[1]), socket);
		}
		else if (type == TypeOfMessage::Grow) {
			if (foods.find(std::stoi(words[3])) != foods.end()) {
				players.find(std::stoi(words[2]))->second->grow();
				foods.erase(std::stoi(words[3]));
				sendAck(std::stoi(words[1]),socket);
			}
			else {
				sendAck(std::stoi(words[1]), socket);
			}
		}
		else if (type == TypeOfMessage::NewPlayer) {
			std::cout << "Otro player conectado\n";
			int id = std::stoi(words[2]);
			if (players.find(id) == players.end()) {
				Player* player = new Player(sf::Vector2i(std::stoi(words[3]), std::stoi(words[4])), sf::Color(0, 50, 255, 255), 10, id, sf::Vector2f(std::stoi(words[3]), std::stoi(words[4])));
				players.emplace(id, player);
				sendAck(std::stoi(words[1]), socket);
			}
			else {
				std::cout << "Es un reenvio\n";
				sendAck(std::stoi(words[1]),socket);
			}
		}
		else if (type == TypeOfMessage::Disconnect) {
			if (std::stoi(words[1]) == myId) {
				std::cout << "Has sido desconectado\n";
				window->close();
			}
			else {
				players.erase(std::stoi(words[1]));
				sendAck(std::stoi(words[2]), socket);
			}
		}
		else if (type == TypeOfMessage::Hello) {
			std::cout << "Welcome recivido\n";
			myId = std::stoi(words[1]);
			Player* player = new Player(sf::Vector2i(std::stoi(words[2]), std::stoi(words[3])), sf::Color(255, 155, 0, 255), 10, myId, sf::Vector2f(std::stoi(words[2]), std::stoi(words[3])));
			players.emplace(myId, player);
			outMessages->erase(0); //borramos el Hello

		}
		else if (type == TypeOfMessage::GameStart) {
			std::cout << "Empieza la partida\n";
			gameStarted = true;
			sendAck(std::stoi(words[1]), socket);
		}
		else if (type == TypeOfMessage::Kill) {
			int idPlayerKill = std::stoi(words[2]);
			std::map<int, Player*>::iterator p = players.find(idPlayerKill);
			if (p != players.end()) {
				if (p->second->isAlive) {
					p->second->isAlive = false;
					sendAck(std::stoi(words[1]), socket);
					if (idPlayerKill == myId) {
						std::cout << "Has perdido!\n";
					}
				}
				else {
					sendAck(std::stoi(words[1]), socket);
				}
			}
		}
		serverMessages->pop();
	}
}

void SceneGame::draw(sf::RenderWindow* window) {

	for (std::map <int, Player*>::iterator it = players.begin(); it != players.end(); ++it) {
		(*it).second->draw(window);
	}

	for (std::map<int, sf::CircleShape*>::iterator it = foods.begin(); it != foods.end(); ++it) {
		window->draw(*it->second);
	}

	walls.draw(window);
}
