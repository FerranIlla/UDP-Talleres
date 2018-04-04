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

#define MSG_LENGTH 128
#define ResendTime 250
std::mutex mu;
std::mutex muOut;




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
	float timeSinceAction;
	int id;

	ClientProxy(Map* map, Address ad, std::string name, int idPlayer) {
		pos.x = rand() % map->getSize().x;
		pos.y = rand() % map->getSize().y;
		address = ad;
		nickname = name;
		timeSinceAction = 0.f;
		id = idPlayer;
	};
};

struct InMsg {
	std::string msg;
	Address addr;

	InMsg(std::string s, Address ad) {
		msg = s;
		addr = ad;
	}
};


void myReceiveFunction(sf::UdpSocket *sock, std::map<Address, ClientProxy> *clients,std::queue<InMsg>* msgList ,bool* open) {
	//bool open = true;
	char data[MSG_LENGTH];
	std::size_t received;
	Address addr;
	while (open) {
		if (sock->receive(data, MSG_LENGTH, received, addr.ip, addr.port) != sf::Socket::Done) {
			//error al recivir
			std::cout << "error al recivir" << std::endl;
		}
		else {
			//procesar el mensaje
			data[received] = '\0';
			std::string s = data;
			InMsg msg(s, addr);
			mu.lock();
			std::cout << data;
			msgList->push(msg);
			mu.unlock();
		}
		
	}
};

void sendNew(std::string s, sf::UdpSocket* socket, int &id,Address addr, std::map<int, outMsg>* outMessages) {
	if (socket->send(&s[0], s.length(), addr.ip, addr.port) != sf::Socket::Done) {
		std::cout << "Ha habido un problema al enviar\n";
	}
	else {
		outMsg msg(s);
		msg.to = addr;
		muOut.lock();
		outMessages->emplace(id, msg);
		muOut.unlock();
		id++;
	}
	//socket->send()
}

void reSend(outMsg msg, sf::UdpSocket* socket, std::string ip, int port) {
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
	std::map<Address, ClientProxy> clients;
	std::thread myThread;
	std::queue<InMsg> msgList;
	std::map<int, outMsg> outMessages;
	int idOutMsg = 0;
	//Vinculamos este socket al puerto 50000
	if (socket.bind(50000) != sf::Socket::Done) {
		//No se puede vincular al puerto 50000
		std::cout << "No se puede vincular al puerto 50000.\n";
		return 0;
	}
	//else...

	sf::Time timer;
	INT32 deltaTime;
	INT32 lastFrameTime = timer.asMilliseconds();
	
	bool open=true;
	myThread = std::thread(&myReceiveFunction, &socket, &clients,&msgList,&open); //abrimos el thread para el receive

	Map mapa(sf::Vector2i(8, 6));

	//aqui iría el bucle del juego
	while (open) {
		deltaTime = timer.asMilliseconds() - lastFrameTime;
		mu.lock();
		while (!msgList.empty()) {
			InMsg msg = msgList.front();
			std::vector<std::string> words = commandToWords(msg.msg);
			if (clients.find(msg.addr) != clients.end()) { //si el cliente existe
				/*if (std::stoi(words[0]) == TypeOfMessage::Hello) {
					
				}*/
				if (std::stoi(words[0]) == TypeOfMessage::Ack) {
					outMessages.erase(std::stoi(words[1])); //borramos el mensaje de los outs
				}
			}
			else {//si no existe el cliente
				if (std::stoi(words[0]) == TypeOfMessage::Hello) {
					ClientProxy newClient(&mapa, msg.addr, msg.msg, clients.size());
					//
					for (std::map<Address, ClientProxy>::iterator it = clients.begin(); it != clients.end(); ++it) {
						std::string s = std::to_string(TypeOfMessage::NewPlayer)+"_"+std::to_string(idOutMsg)+"_" +std::to_string(clients.size())+ "_" +std::to_string(newClient.pos.x) + "_" +std::to_string(newClient.pos.y);
						sendNew(s, &socket, idOutMsg, (*it).first, &outMessages);
						s = std::to_string(TypeOfMessage::NewPlayer) + "_" + std::to_string(idOutMsg) + "_" + std::to_string((*it).second.id) + "_" + std::to_string((*it).second.pos.x) + "_" + std::to_string((*it).second.pos.y);
						sendNew(s,&socket, idOutMsg, newClient.address, &outMessages);
					}
					
					std::string s= std::to_string(TypeOfMessage::Hello) + "_" + std::to_string(clients.size()) + "_" + std::to_string(newClient.pos.x) + "_" + std::to_string(newClient.pos.y);
					sendNormal(s, &socket, newClient.address);
					clients.emplace(msg.addr, newClient);
				}
			}
			msgList.pop();
		}
		mu.unlock();

		//reenviar mensajes
		muOut.lock();
		for (std::map<int, outMsg>::iterator it = outMessages.begin(); it != outMessages.end(); ++it) {
			if (it->second.hasToResend(deltaTime, ResendTime)) {
				reSend(it->second, &socket, it->second.to.ip.toString(),it->second.to.port);
			}
		}
		muOut.unlock();

	}

	

	
	std::cout << "menaje final del programa\n";
	myThread.join();
	
	system("pause");
}