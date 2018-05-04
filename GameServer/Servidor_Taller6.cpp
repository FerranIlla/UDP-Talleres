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

#define MSG_LENGTH 512

#define MaxIdMsg 255;
std::mutex mu;

typedef outMsgServer outMsg;


struct InMsg {
	std::string msg;
	Address addr;

	InMsg(std::string s, Address ad) {
		msg = s;
		addr = ad;
	}
};


void myReceiveFunction(sf::UdpSocket *sock, std::queue<InMsg>* msgList, bool* open) {
	//bool open = true;
	char data[MSG_LENGTH];
	std::size_t received;
	Address addr;
	while (open) {
		sf::Socket::Status status = sock->receive(data, MSG_LENGTH, received, addr.ip, addr.port);
		if (status!=sf::Socket::Status::Done) {
			//error al recivir
			mu.lock();
			std::cout << "Error al recivir" << std::endl;
			mu.unlock();
		}
		else {
			//procesar el mensaje
			data[received] = '\0';
			std::string s = data;
			InMsg msg(s, addr);
			mu.lock();
			//std::cout << data;
			msgList->push(msg);
			mu.unlock();
		}

	}
};

void sendNew(std::string s, sf::UdpSocket* socket, int &id, Address addr, std::map<int, std::string>* outMessages) {
	bool errorInSend = false;
	if (percentageGate(PERCENT_PACKETLOST)) {//si pasa
		errorInSend = (socket->send(&s[0], s.length(), addr.ip, addr.port) != sf::Socket::Done);
	}
	else {
		std::cout << "Paquete perdido. " << std::endl;
	}

	if(errorInSend) std::cout << "Ha habido un problema al enviar\n";
	else {
		outMessages->emplace(id, s);
		id = (id + 1) % MaxIdMsg;
	}
	
}

void sendNormal(std::string msg, sf::UdpSocket* socket, Address addr) {
	if (percentageGate(PERCENT_PACKETLOST)) {//si pasa
		if (socket->send(&msg[0], msg.length(), addr.ip, addr.port) != sf::Socket::Done) {
			std::cout << "Ha habido un problema al enviar\n";
		}
	}
	else {
		std::cout << "Paquete perdido." << std::endl;
	}
}



int main() {
	srand(time(NULL));
	sf::UdpSocket socket;
	sf::UdpSocket receiveSocket;
	std::map<Address, ClientProxy> clients;
	std::thread myThread;
	std::queue<InMsg> msgList;
	int idOutMsg = 0;
	//Vinculamos este socket al puerto 50000
	if (receiveSocket.bind(50000) != sf::Socket::Done) {
		//No se puede vincular al puerto 50000
		std::cout << "No se puede vincular al puerto 50000.\n";
		return 0;
	}

	std::queue<int> disponiblePlayerIds;
	disponiblePlayerIds.push(0);
	disponiblePlayerIds.push(1);
	disponiblePlayerIds.push(2);
	disponiblePlayerIds.push(3);

	sf::Clock timer;
	sf::Time deltaTime;
	sf::Time lastFrameTime = sf::milliseconds(0);

	sf::Time timeLastResend = sf::milliseconds(0);
	sf::Time timeLastPing = sf::milliseconds(0);

	bool open = true;
	bool gameStarted = false;
	//cambiar esta variable para cambiar el numero de jugadores necesarios para empezar una partida
	int numberOfPlayers = 2;
	myThread = std::thread(&myReceiveFunction, &receiveSocket, &msgList, &open); //abrimos el thread para el receive

	ServerMap mapa(sf::Vector2i(800, 600));


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
				else if (type == TypeOfMessage::Move) {
					sendingClient->second.setTarget(sf::Vector2f(std::stoi(words[1]), std::stoi(words[2])));
					for (std::map<Address, ClientProxy>::iterator it = clients.begin(); it != clients.end(); ++it) {
						if (it->second.id !=sendingClient->second.id ) {
							std::string s = std::to_string(TypeOfMessage::Move) + "_" + std::to_string(sendingClient->second.id) + "_" + words[1] + "_" + words[2];
							sendNormal(s, &socket, it->first);
						}
					}
				}
				else if (type == TypeOfMessage::Disconnect) {
					int id = clients.find(msg.addr)->second.id;
					disponiblePlayerIds.push(id);
					clients.erase(msg.addr);
					std::cout << "Jugador expulsado\n";

					for (std::map<Address, ClientProxy>::iterator it = clients.begin(); it != clients.end(); ++it) {
						std::string s = std::to_string(Disconnect);
						s = s + "_" + std::to_string(id) + "_" + std::to_string(idOutMsg);
						sendNew(s, &socket, idOutMsg, (*it).second.address, &it->second.outMessages);
					}
				}
				else if (type == TypeOfMessage::Hello) {
					std::string s = std::to_string(TypeOfMessage::Hello) + "_" + std::to_string(clients.find(msg.addr)->second.id) + "_" + std::to_string(clients.find(msg.addr)->second.pos.x) + "_" + std::to_string((clients.find(msg.addr)->second.pos.y));
					sendNormal(s, &socket, msg.addr);
				}
				//to do responder a un hello que ya tenemos
			}


#pragma endregion
#pragma region Non Existing Client
			else {//si no existe el cliente
				if (std::stoi(words[0]) == TypeOfMessage::Hello) {
					if (!gameStarted) {
						ClientProxy newClient(&mapa, msg.addr, msg.msg, disponiblePlayerIds.front());
						//
						for (std::map<Address, ClientProxy>::iterator it = clients.begin(); it != clients.end(); ++it) {
							std::string s = std::to_string(TypeOfMessage::NewPlayer) + "_" + std::to_string(idOutMsg) + "_" + std::to_string(disponiblePlayerIds.front()) + "_" + std::to_string(newClient.pos.x) + "_" + std::to_string(newClient.pos.y);
							sendNew(s, &socket, idOutMsg, (*it).first, &it->second.outMessages);
							s = std::to_string(TypeOfMessage::NewPlayer) + "_" + std::to_string(idOutMsg) + "_" + std::to_string((*it).second.id) + "_" + std::to_string((*it).second.pos.x) + "_" + std::to_string((*it).second.pos.y);
							sendNew(s, &socket, idOutMsg, newClient.address, &newClient.outMessages);
						}

						std::string s = std::to_string(TypeOfMessage::Hello) + "_" + std::to_string(disponiblePlayerIds.front()) + "_" + std::to_string(newClient.pos.x) + "_" + std::to_string(newClient.pos.y);
						disponiblePlayerIds.pop();
						sendNormal(s, &socket, newClient.address);
						clients.emplace(msg.addr, newClient);
						if (clients.size() == numberOfPlayers) {
							gameStarted = true;
							std::cout << "Empieza la partida\n";					
							for (std::map<Address, ClientProxy>::iterator it = clients.begin(); it != clients.end(); ++it) {
								s = std::to_string(TypeOfMessage::GameStart) + "_" + std::to_string(idOutMsg);
								sendNew(s, &socket, idOutMsg, it->first, &it->second.outMessages);
								//enviamos la primera comida
								s = std::to_string(TypeOfMessage::Food) + "_" + std::to_string(idOutMsg) + "_" + std::to_string(mapa.foodId) + "_" + std::to_string(mapa.food.find(mapa.foodId)->second->x) + "_" + std::to_string(mapa.food.find(mapa.foodId)->second->y);
								sendNew(s, &socket, idOutMsg, it->first, &it->second.outMessages);
							}
						}

					}
				}
			}
#pragma endregion
			mu.lock();
			msgList.pop();
			msgOnList = !msgList.empty();
			mu.unlock();
		}
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
				clients.erase(temp);
				disponiblePlayerIds.push(id);
				for (std::map<Address, ClientProxy>::iterator itB = clients.begin(); itB != clients.end(); ++itB) {
					std::string s = std::to_string(Disconnect);
					s = s + "_" + std::to_string(id) + "_" + std::to_string(idOutMsg);
					sendNew(s, &socket, idOutMsg, (*it).second.address, &it->second.outMessages);
				}
			}
			else {
				++it;
			}
		}
#pragma endregion

#pragma region Update
		if (gameStarted) {
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
		}
#pragma endregion

	}


	std::cout << "menaje final del programa\n";
	clients.clear();
	myThread.join();

	system("pause");
}