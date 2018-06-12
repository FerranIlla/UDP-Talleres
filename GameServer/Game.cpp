#include "Game.h"

Game::Game(int numPlayers, sf::UdpSocket* sock, ClientProxy* creator, std::string n, int id, float foodS) : mapa(sf::Vector2i(800, 600),foodS) {
	socket = sock;
	addPlayer(creator);
	name = n;
	creatorName = creator->nickname;
	numberOfPlayers = numPlayers;
	idGame = id;
	gameEnded = false;
	gameStarted = false;
	
	timeLastSendMoveMsg = sf::milliseconds(0);
	timeLastResend = sf::milliseconds(0);
	timeLastPing = sf::milliseconds(0);
}
Game::~Game() {

}

void Game::endGame() {

}
int Game::getNumberOfPlayers() {
	return clients.size();
}

int Game::getMaxNumberOfPlayers() {
	return numberOfPlayers;
}

std::string Game::getCreatorName() {
	return creatorName;
}

bool Game::isEnded() {
	return gameEnded;
}
void Game::checkMsgList() {
	bool msgOnList = !msgList.empty();

	while (msgOnList) {
		mu.lock();
		InMsg msg = msgList.front();
		mu.unlock();
		std::vector<std::string> words = commandToWords(msg.msg);
#pragma region Existing Client
		std::map<Address, ClientProxy*>::iterator sendingClient = clients.find(msg.addr);
		if (sendingClient != clients.end()) {
			TypeOfMessage type = (TypeOfMessage)std::stoi(words[0]);

			if (type == TypeOfMessage::Ack) {
				sendingClient->second->outMessages.erase(std::stoi(words[1]));//borramos el mensaje de los outs
			}
			else if (type == TypeOfMessage::Ping) {
				sendingClient->second->resetPing();
			}
			else if (type == TypeOfMessage::Move) {
				InMsg moveMsg = msg;
				moveMsgList.push_back(moveMsg);
				sendingClient->second->setTarget(sf::Vector2f(std::stoi(words[1]), std::stoi(words[2])));
			}
			else if (type == TypeOfMessage::Disconnect) {
				int id = clients.find(msg.addr)->second->id;
				clients.erase(msg.addr);
				std::cout << "Jugador expulsado\n";

				for (std::map<Address, ClientProxy*>::iterator it = clients.begin(); it != clients.end(); ++it) {
					std::string s = std::to_string(Disconnect);
					s = s + "_" + std::to_string(id) + "_" + std::to_string(idOutMsg);
					sendNew(s, socket, idOutMsg, (*it).second->address, &it->second->outMessages);
				}
			}
		}


#pragma endregion
		mu.lock();
		msgList.pop();
		msgOnList = !msgList.empty();
		mu.unlock();
	}
}
void Game::checkMoveSend(sf::Time deltaTime) {
	//enviar moveMsg
	timeLastSendMoveMsg += deltaTime;
	if (!moveMsgList.empty() && timeLastSendMoveMsg > SendMoveMsgTime) {
		//send last move msg (target)
		std::vector<std::string> words = commandToWords(moveMsgList.back().msg);
		std::map<Address, ClientProxy*>::iterator sendingClient = clients.find(moveMsgList.back().addr);
		for (std::map<Address, ClientProxy*>::iterator it = clients.begin(); it != clients.end(); ++it) {
			if (it->second->id != sendingClient->second->id) {
				std::string s = std::to_string(TypeOfMessage::Move) + "_" + std::to_string(sendingClient->second->id) + "_" + words[1] + "_" + words[2];
				sendNormal(s, socket, it->first);
			}
		}
		moveMsgList.clear();
	}
}
void Game::checkResend(sf::Time deltaTime) {
	//reenviar mensajes
	timeLastResend += deltaTime;
	if (timeLastResend > ResendTime) {
		for (std::map<Address, ClientProxy*>::iterator it = clients.begin(); it != clients.end(); ++it) {
			for (std::map<int, std::string>::iterator itMsg = it->second->outMessages.begin(); itMsg != it->second->outMessages.end(); ++itMsg) {
				std::cout << "ResendingMsg:	" + itMsg->second + "\n";
				sendNormal(itMsg->second, socket, it->first);
			}
		}

	}
}

void Game::update(sf::Time deltaTime) {
	if (gameStarted) {
		for (std::map <Address, ClientProxy*>::iterator it = clients.begin(); it != clients.end(); ++it) {
			if (it->second->isAlive) {
				it->second->movePlayer(deltaTime.asSeconds());
				int idFoodEaten = it->second->checkFoodCollision(mapa.food);
				if (idFoodEaten != -1) {
					for (std::map <Address, ClientProxy*>::iterator itSend = clients.begin(); itSend != clients.end(); ++itSend) {
						std::string s = std::to_string(TypeOfMessage::Grow) + "_" + std::to_string(idOutMsg) + "_" + std::to_string(it->second->id) + "_" + std::to_string(idFoodEaten);
						sendNew(s, socket, idOutMsg, itSend->first, &itSend->second->outMessages);
					}
					mapa.food.erase(idFoodEaten);
					it->second->grow();
				}
				//si se sale del mapa o choca con otro jugador
				if (mapa.isPlayerOutside(it->second->getHeadPos(), it->second->getRadius()) || it->second->checkPlayersCollision(clients)) {
					it->second->isAlive = false;
					for (std::map <Address, ClientProxy*>::iterator it2 = clients.begin(); it2 != clients.end(); ++it2) {
						std::string s = std::to_string(TypeOfMessage::Kill) + "_" + std::to_string(idOutMsg) + "_" + std::to_string(it->second->id);
						sendNew(s, socket, idOutMsg, it2->first, &it2->second->outMessages);

					}
				}
			}
		}

		if (mapa.update(deltaTime)) {
			for (std::map <Address, ClientProxy*>::iterator it = clients.begin(); it != clients.end(); ++it) {
				std::string s = std::to_string(TypeOfMessage::Food) + "_" + std::to_string(idOutMsg) + "_" + std::to_string(mapa.foodId) + "_" + std::to_string(mapa.food.find(mapa.foodId)->second->x) + "_" + std::to_string(mapa.food.find(mapa.foodId)->second->y);
				sendNew(s, socket, idOutMsg, it->first, &it->second->outMessages);
			}
		}
	}

}

void Game::addPlayer(ClientProxy* newPlayer) {
	if (!gameStarted) {
		clients.emplace(newPlayer->address, newPlayer);
		newPlayer->idPartida = idGame;
		//enviar al resto


		if (clients.size() == numberOfPlayers) {
			//start game
			gameStarted = true;
			std::string positions=std::to_string(clients.size());
			for (std::map<Address, ClientProxy*>::iterator it = clients.begin(); it != clients.end(); ++it) {
				positions += "_" + std::to_string(it->second->id);
				sf::Vector2f pos;
				pos.x = rand() % (mapa.getSize().x - 200) + 100;
				pos.y = rand() % (mapa.getSize().y - 200) + 100;
				it->second->SetPosition(pos);
				positions += "_" + std::to_string(it->second->pos.x);
				
				positions += "_" + std::to_string(it->second->pos.y);
			}
			for (std::map<Address, ClientProxy*>::iterator it = clients.begin(); it != clients.end(); ++it) {
				sendNew(std::to_string(TypeOfMessage::GameStart) + "_" + std::to_string(idOutMsg)+"_"+positions,socket,idOutMsg,it->first,&it->second->outMessages);
			}
		}
	}
}

std::string Game::getName() {
	return name;
}

void Game::addMsg(InMsg msg) {
	msgList.push(msg);
}

