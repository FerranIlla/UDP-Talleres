#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <map>
#include <queue>
#include "utils.h"
#include "ServerMap.h"
#include "ClientProxy.h"
#include "timeDefines.h"
#include "NetworkFunctions.h"
#include "DBManager.h"
#include "GameList.h"


#define connexion "tcp://192.168.1.59:3306"
#define username "root"
#define pwd "linux123"

std::mutex mu;




int main() {
	srand(time(NULL));
	sf::UdpSocket socket;
	sf::UdpSocket receiveSocket;
	std::map<Address, ClientProxy> clients;
	std::thread myThread;
	std::queue<InMsg> msgList;
	std::vector<InMsg> moveMsgList;
	int idOutMsg = 0;
	//Vinculamos este socket al puerto 50000
	if (receiveSocket.bind(50000) != sf::Socket::Done) {
		//No se puede vincular al puerto 50000
		std::cout << "No se puede vincular al puerto 50000.\n";
		return 0;
	}

	DBManager DBman(connexion,username,pwd);

	sf::Clock timer;
	sf::Time deltaTime;
	sf::Time lastFrameTime = sf::milliseconds(0);


	sf::Time timeLastResend = sf::milliseconds(0);
	sf::Time timeLastPing = sf::milliseconds(0);

	bool open = true;

	GameList listGame;

	myThread = std::thread(&myReceiveFunction, &receiveSocket, &msgList, &open); //abrimos el thread para el receive



	//bucle del juego
	while (open) {
		deltaTime = timer.restart() - lastFrameTime;
#pragma region Received Messages
		mu.lock();
		bool msgOnList = !msgList.empty();
		mu.unlock();
		while (msgOnList) {
			mu.lock();
			InMsg msg = msgList.front();
			mu.unlock();
			std::vector<std::string> words = commandToWords(msg.msg);
#pragma region Existing Client
			std::map<Address, ClientProxy>::iterator sendingClient = clients.find(msg.addr);
			if ( sendingClient!= clients.end()) {
				TypeOfMessage type = (TypeOfMessage)std::stoi(words[0]);

				if (type == TypeOfMessage::Ack) {
					sendingClient->second.outMessages.erase(std::stoi(words[1]));//borramos el mensaje de los outs
				}
				else if (type == TypeOfMessage::Ping) {
					sendingClient->second.resetPing();
				}
				else if (type == TypeOfMessage::Chat) {
					//enviar solo a los demas jugadores
					for (std::map<Address, ClientProxy>::iterator it = clients.begin(); it != clients.end(); ++it) {
						if ((it->first == msg.addr) == false) {
							//sendNew(msg.msg, &socket, idOutMsg, (*it).second.address, &it->second.outMessages);
							sendNormal(msg.msg, &socket, (*it).second.address);
						}

					}
				}
				else if (type == TypeOfMessage::ListGames) {
					sendNormal(std::to_string(TypeOfMessage::ListGames) + listGame.getNames(),&socket,msg.addr);
					sendNormal(std::to_string(TypeOfMessage::Ack) +"_"+ words[1], &socket,msg.addr);
				}
				else if (type == TypeOfMessage::JoinGame) {
					listGame.addPlayerToGame(std::stoi(words[2]),&clients.find(msg.addr)->second);
				}
				else if (type == TypeOfMessage::CreateGame) {
					listGame.AddGame(&clients.find(msg.addr)->second, &socket, std::stoi(words[2]),std::stof(words[3]));
					sendNormal(std::to_string(TypeOfMessage::Ack) +"_"+words[1], &socket, msg.addr);
					std::cout << "partidaCreada\n";
				}
				else if (type == TypeOfMessage::Login) {
					std::cout << "Login recibido\n";
					if (clients.find(msg.addr) != clients.end()) {
						ClientProxy* logClient =&clients.find(msg.addr)->second;
						if (logClient->id == -1) {
							if (DBman.login(words[2], words[3])) {
								int idLogPlayer=DBman.getPlayerId(words[2]);
								logClient->id = idLogPlayer;
								logClient->nickname = words[2];
								std::string s = std::to_string(TypeOfMessage::Login) + "_" + std::to_string(idOutMsg) + "_" + std::to_string(idLogPlayer) + "_" + words[2];
								sendNew(s, &socket, idOutMsg, msg.addr, &logClient->outMessages);
								s = std::to_string(TypeOfMessage::Ack) + "_" + words[1];
								sendNormal(s, &socket, msg.addr);
							}
							else {
								std::string s = std::to_string(TypeOfMessage::ErrorLogin) + "_"+std::to_string(idOutMsg);
								sendNew(s, &socket, idOutMsg, msg.addr, &logClient->outMessages);
								s = std::to_string(TypeOfMessage::Ack) + "_" + words[1];
								sendNormal(s, &socket, msg.addr);
							}
						}
						else {
							std::string s = std::to_string(TypeOfMessage::Ack) + "_" + words[1];
							sendNormal(s, &socket, msg.addr);
						}
					}


				}
				else if (type == TypeOfMessage::Register) {
					std::cout << "intento de registro recibido\n";
					if (clients.find(msg.addr) != clients.end()) {
						ClientProxy* logClient = &clients.find(msg.addr)->second;
						if (logClient->id == -1) {
							if (DBman.altaCuenta(words[2], words[3])) {
								int idRegisterPlayer = DBman.getPlayerId(words[2]);
								logClient->id = idRegisterPlayer;
								logClient->nickname = words[2];
								std::string s = std::to_string(TypeOfMessage::Login) + "_" + std::to_string(idOutMsg) + "_" + std::to_string(idRegisterPlayer)+"_"+words[2];
								sendNew(s, &socket, idOutMsg, msg.addr,&logClient->outMessages);
								s = std::to_string(TypeOfMessage::Ack) + "_" + words[1];
								sendNormal(s, &socket, msg.addr);
							}
							else {
								std::string s = std::to_string(TypeOfMessage::ErrorRegister) + "_" + std::to_string(idOutMsg);
								sendNew(s, &socket, idOutMsg, msg.addr, &logClient->outMessages);
								s = std::to_string(TypeOfMessage::Ack) + "_" + words[1];
								sendNormal(s, &socket, msg.addr);
							}
						}
						else {
							std::string s = std::to_string(TypeOfMessage::Ack) + "_" + words[1];
							sendNormal(s, &socket, msg.addr);
						}
					}

				}
				else if (type == TypeOfMessage::Disconnect) {
					int id = clients.find(msg.addr)->second.id;
					//disponiblePlayerIds.push(id);
					clients.erase(msg.addr);
					std::cout << "Jugador expulsado\n";

					for (std::map<Address, ClientProxy>::iterator it = clients.begin(); it != clients.end(); ++it) {
						std::string s = std::to_string(Disconnect);
						s = s + "_" + std::to_string(id) + "_" + std::to_string(idOutMsg);
						sendNew(s, &socket, idOutMsg, (*it).second.address, &it->second.outMessages);
					}
				}
				else if (type == TypeOfMessage::Hello) {
					std::string s = std::to_string(TypeOfMessage::Hello);
					sendNormal(s, &socket, msg.addr);
				}
				//to do responder a un hello que ya tenemos
			}


#pragma endregion
#pragma region Non Existing Client
			else {//si no existe el cliente
				if (std::stoi(words[0]) == TypeOfMessage::Hello) {
					ClientProxy newClient(msg.addr, msg.msg, -1);
					clients.emplace(msg.addr, newClient);
					std::string s = std::to_string(TypeOfMessage::Hello);
					sendNormal(s, &socket, msg.addr);
					std::cout << "Hello recibido\n";
				}
			}
#pragma endregion
			mu.lock();
			msgList.pop();
			msgOnList = !msgList.empty();
			mu.unlock();
		}
#pragma endregion

#pragma region

#pragma endregion
#pragma region Resend

		//reenviar mensajes
		timeLastResend += deltaTime;
		if (timeLastResend > ResendTime) {
			for (std::map<Address, ClientProxy>::iterator it = clients.begin(); it != clients.end(); ++it) {
				for (std::map<int, std::string>::iterator itMsg = it->second.outMessages.begin(); itMsg != it->second.outMessages.end(); ++itMsg) {
					std::cout << "ResendingMsg:	" + itMsg->second+"\n";
					sendNormal(itMsg->second, &socket, it->first);
				}
			}
			timeLastResend -= ResendTime;
		}
#pragma endregion
#pragma region Ping
		timeLastPing += deltaTime;
		if (timeLastPing > PingTime) {
			//std::cout << "Enviando Ping\n";
			for (std::map<Address, ClientProxy>::iterator it = clients.begin(); it != clients.end(); ++it) {
				std::string msg = std::to_string(TypeOfMessage::Ping);
				sendNormal(msg, &socket, it->first);
			}
			timeLastPing -= PingTime;
		}

		for (std::map<Address, ClientProxy >::iterator it = clients.begin(); it != clients.end(); ) {
			if ((*it).second.isDisconected(deltaTime)) {
				std::cout << "Jugador expulsado\n";
				int id = it->second.id;
				std::map<Address, ClientProxy >::iterator temp = it;
				it++;
				sendNormal(std::to_string(Disconnect),&socket,temp->first);
				clients.erase(temp);
				//disponiblePlayerIds.push(id);
				/*for (std::map<Address, ClientProxy>::iterator itB = clients.begin(); itB != clients.end(); ++itB) {
					std::string s = std::to_string(Disconnect);
					s = s + "_" + std::to_string(id) + "_" + std::to_string(idOutMsg);
					sendNew(s, &socket, idOutMsg, (*it).second.address, &it->second.outMessages);
				}*/
			}
			else {
				++it;
			}
		}
#pragma endregion

#pragma region Update
		listGame.update(deltaTime);




		/*if (gameStarted) {
			for (std::map <Address, ClientProxy>::iterator it = clients.begin(); it != clients.end(); ++it) {
				if (it->second.isAlive) {
					it->second.movePlayer(deltaTime.asSeconds());
					int idFoodEaten = it->second.checkFoodCollision(mapa.food);
					if (idFoodEaten != -1) {
						for (std::map <Address, ClientProxy>::iterator itSend = clients.begin(); itSend != clients.end(); ++itSend) {
							std::string s = std::to_string(TypeOfMessage::Grow) + "_" + std::to_string(idOutMsg)+"_"+std::to_string(it->second.id)+"_"+ std::to_string(idFoodEaten);
							sendNew(s, &socket, idOutMsg, itSend->first, &itSend->second.outMessages);
						}
						mapa.food.erase(idFoodEaten);
						it->second.grow();
					}
					//si se sale del mapa o choca con otro jugador
					if (mapa.isPlayerOutside(it->second.getHeadPos(), it->second.getRadius()) || it->second.checkPlayersCollision(clients)) {
						it->second.isAlive = false;
						for (std::map <Address, ClientProxy>::iterator it2 = clients.begin(); it2 != clients.end(); ++it2) {
							std::string s = std::to_string(TypeOfMessage::Kill) + "_" + std::to_string(idOutMsg) + "_" + std::to_string(it->second.id);
							sendNew(s, &socket, idOutMsg, it2->first, &it2->second.outMessages);
							
						}
					}
				}
			}
		
			if (mapa.update(deltaTime)) {
				for (std::map <Address, ClientProxy>::iterator it = clients.begin(); it != clients.end(); ++it) {
					std::string s = std::to_string(TypeOfMessage::Food) + "_" + std::to_string(idOutMsg) + "_" + std::to_string(mapa.foodId)+"_"+std::to_string(mapa.food.find(mapa.foodId)->second->x)+"_"+ std::to_string(mapa.food.find(mapa.foodId)->second->y);
					sendNew(s, &socket, idOutMsg, it->first, &it->second.outMessages);
				}
			}
		}

		if (clients.size() == 0 && gameStarted) {
			gameStarted = false;
			std::cout << "Partida acabada, serverReiniciado\n";
		}*/
#pragma endregion

	}


	std::cout << "menaje final del programa\n";
	clients.clear();
	myThread.join();

	system("pause");
}