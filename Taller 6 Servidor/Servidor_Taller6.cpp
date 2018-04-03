#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <map>
#include <queue>

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

struct outMsg {
	std::string msg;
	float timeLastSend;
	Address address;

	outMsg(std::string txt, Address ad) {
		timeLastSend = 0;
		address = ad;
		msg = txt;
	}

	bool hasToResend(float delta, float waitTime) {
		waitTime += delta;
		if (timeLastSend >= waitTime) {
			timeLastSend -= waitTime;
			return true;
		}
		return false;
	}
};

void myReceiveFunction(sf::UdpSocket *sock, std::map<Address, ClientProxy> *clients,std::queue<std::string>* msgList ,bool* open) {
	//bool open = true;
	char data[MSG_LENGTH];
	std::size_t received;
	Address addr;
	while (open) {
		/*if (clients->size() == 0) {//aixo era momentani --------------------------------------------------<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			open = false; 
			continue;
		}*/
		if (sock->receive(data, MSG_LENGTH, received, addr.ip, addr.port) != sf::Socket::Done) {
			//error al recivir
			std::cout << "error al recivir" << std::endl;
		}
		else {
			//procesar el mensaje
			data[received] = '\0';
			mu.lock();
			std::cout << data;
			msgList->emplace(data);
			mu.unlock();
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
	
	bool open=true;
	std::queue<std::string> clientMessages;


	myThread = std::thread(&myReceiveFunction, &socket, &clients,&open); //abrimos el therad para el receive



	//aqui iría el bucle del juego
	while (open) {

	}

	

	
	std::cout << "menaje final del programa\n";
	myThread.join();
	
	system("pause");
}