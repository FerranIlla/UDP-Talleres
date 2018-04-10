#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <map>
#include <queue>
#include "utils.h"
#include "Mapa.h"

#define MSG_LENGTH 512
#define ResendTime sf::milliseconds(150)
#define PingTime sf::milliseconds(1000)
#define DisconnectTime sf::milliseconds(5000)
#define MaxIdMsg 255;
std::mutex mu;


struct Position {
	int x, y;
	bool operator==(Position &rho) {
		if (x == rho.x) {
			return y == rho.y;
		}
		return false;
	}
};

class ClientProxy {
public:
	std::string nickname;
	Position pos;
	Address address; //suposo que no cal perque hi ha el map
	sf::Time timeSincePing;
	int id;
	std::map<int, std::string> outMessages;

	ClientProxy(Map* map, Address ad, std::string name, int idPlayer) {
		pos.x = rand() % map->getSize().x;
		pos.y = rand() % map->getSize().y;
		address = ad;
		nickname = name;
		timeSincePing = sf::milliseconds(0);
		id = idPlayer;
	};

	void resetPing() {
		timeSincePing = sf::milliseconds(0);
	}

	bool isDisconected(sf::Time deltaTime) {
		timeSincePing += deltaTime;
		return timeSincePing > DisconnectTime;
	}

	void addOutMsg(std::string s, int id) {
		outMessages.emplace(id,s);
	}
};

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
	if (socket->send(&s[0], s.length(), addr.ip, addr.port) != sf::Socket::Done) {
		std::cout << "Ha habido un problema al enviar\n";
	}
	else {
		outMessages->emplace(id, s);
		id=(id+1)%MaxIdMsg;
	}
	//socket->send()
}

void reSend(outMsg msg, sf::UdpSocket* socket, sf::IpAddress ip, int port) {
	if (socket->send(&msg.msg[0], msg.msg.length(), ip, port) != sf::Socket::Done) {
		std::cout << "Ha habido un problema al enviar\n";
	}
}

void sendNormal(std::string msg, sf::UdpSocket* socket, Address addr) {
	if (socket->send(&msg[0], msg.length(), addr.ip, addr.port) != sf::Socket::Done) {
		std::cout << "Ha habido un problema al enviar\n";
	}
}



int main() {
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
	myThread = std::thread(&myReceiveFunction, &receiveSocket, &msgList, &open); //abrimos el thread para el receive

	Map mapa(sf::Vector2i(8, 6));

	//aqui iría el bucle del juego
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
			if (clients.find(msg.addr) != clients.end()) {
				TypeOfMessage type = (TypeOfMessage)std::stoi(words[0]);
				if (type == TypeOfMessage::Ack) {
					clients.find(msg.addr)->second.outMessages.erase(std::stoi(words[1]));//borramos el mensaje de los outs
				}
				else if (type == TypeOfMessage::Ping) {
					clients.find(msg.addr)->second.resetPing();
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
					std::cout << "ResendingMsg id: " + std::to_string(itMsg->first);
					reSend(itMsg->second, &socket, it->first.ip, it->first.port);
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
				clients.erase(it++);
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
	}


	std::cout << "menaje final del programa\n";
	clients.clear();
	myThread.join();

	system("pause");
}