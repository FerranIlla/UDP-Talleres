#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <map>

#define MSG_LENGTH 128
std::mutex mu;

struct Address {
	sf::IpAddress ip;
	unsigned short port;

	bool operator< (Address &rho) {
		if (ip < rho.ip) return true;
		else if (ip == rho.ip){
			if (port < rho.port) return true;
		}
		return false;
	}
};

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

	ClientProxy() {};
};

void myReceiveFunction(sf::UdpSocket *sock, std::map<Address, ClientProxy> *clients) {
	bool open = true;
	char data[MSG_LENGTH];
	std::size_t received;
	Address addr;
	while (open) {
		if (clients->size() == 0) {//aixo era momentani --------------------------------------------------<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			open = false; 
			continue;
		}
		if (sock->receive(data, MSG_LENGTH, received, addr.ip, addr.port) != sf::Socket::Done) {
			//error al recivir
			std::cout << "error al recivir" << std::endl;
		}
		else {
			//procesar el mensaje

			//if (data == disconnect and clients.size() == 0) open =false;
		}
		
	}
};

int main() {

	sf::UdpSocket socket;
	std::map<Address, ClientProxy> clients;
	std::thread myThread;

	//Vinculamos este socket al puerto 50000
	if (socket.bind(50000) != sf::Socket::Done) {
		//No se puede vincular al puerto 50000
		std::cout << "No se puede vincular al puerto 50000.\n";
		return 0;
	}
	//else...
	
	myThread = std::thread(&myReceiveFunction, &socket, &clients); //abrimos el therad para el receive

	//aqui iría el bucle del juego


	

	
	std::cout << "menaje final del programa\n";
	myThread.join();
	
	system("pause");
}