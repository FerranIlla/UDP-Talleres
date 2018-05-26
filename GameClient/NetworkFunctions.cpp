#include "NetworkFunctions.h"



void sendNew(std::string s, sf::UdpSocket* socket, int &id, std::map<int, outMsg>* outMessages) {
	bool errorInSend = false;
	if (percentageGate(PERCENT_PACKETLOST)) {//si pasa
		errorInSend = (socket->send(&s[0], s.length(), ServerAdress, 50000) != sf::Socket::Done);
	}
	else {
		std::cout << "Paquete perdido. " << std::endl;
	}

	if (errorInSend) std::cout << "Ha habido un problema al enviar\n";
	else {
		outMsg msg(s);
		outMessages->emplace(id, msg);
		id = (id + 1) % MaxIdMsg;
	}
}

void sendAck(int id, sf::UdpSocket* socket) {
	if (percentageGate(PERCENT_PACKETLOST)) {//si pasa
		std::string s = std::to_string(TypeOfMessage::Ack) + "_" + std::to_string(id);
		if (socket->send(&s[0], s.length(), ServerAdress, 50000) != sf::Socket::Done) {
			std::cout << "Ha habido un problema al enviar\n";
		}
	}
	else {
		std::cout << "Paquete perdido. " << std::endl;
	}
}

void sendNormal(std::string msg, sf::UdpSocket* socket) {
	if (percentageGate(PERCENT_PACKETLOST)) {//si pasa
		if (socket->send(msg.c_str(), msg.length(), ServerAdress, 50000) != sf::Socket::Done) {
			std::cout << "Ha habido un problema al enviar\n";
		}
	}
	else {
		std::cout << "Paquete perdido." << std::endl;
	}
}

void receive(sf::UdpSocket* socket, std::queue<std::string>* msgList, sf::RenderWindow* window) {
	char data[MaxDataRecived];
	sf::IpAddress adress;
	unsigned short port;
	std::size_t received;
	while (window->isOpen()) {
		if (socket->receive(data, MaxDataRecived, received, adress, port) == sf::Socket::Done) {
			if (adress == ServerAdress) {
				data[received] = '\0';
				mu.lock();
				//std::cout << "Recivido: " << data << std::endl;
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
