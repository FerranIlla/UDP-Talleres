#include "NetworkFunctions.h"

void myReceiveFunction(sf::UdpSocket *sock, std::queue<InMsg>* msgList, bool* open) {
	//bool open = true;
	char data[MSG_LENGTH];
	std::size_t received;
	Address addr;
	while (open) {
		sf::Socket::Status status = sock->receive(data, MSG_LENGTH, received, addr.ip, addr.port);
		if (status != sf::Socket::Status::Done) {
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
		std::cout << "Paquete perdido. A" << std::endl;
	}

	if (errorInSend) std::cout << "Ha habido un problema al enviar\n";
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
		std::cout << "Paquete perdido. B" << std::endl;
	}
}