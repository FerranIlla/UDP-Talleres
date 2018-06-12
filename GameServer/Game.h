#pragma once
#include "SFML\Graphics.hpp"
#include "NetworkFunctions.h"
#include "ClientProxy.h"

class Game {
private:
	std::string name;
	std::string creatorName;
	float foodSpawnRate;
	int idGame;

	sf::UdpSocket* socket;
	std::map<Address, ClientProxy*> clients;
	std::queue<InMsg> msgList;
	std::vector<InMsg> moveMsgList;
	int idOutMsg;

	sf::Time timeLastSendMoveMsg;
	sf::Time timeLastResend;
	sf::Time timeLastPing;

	bool gameStarted;
	bool gameEnded;
	int numberOfPlayers;

	ServerMap mapa;

public:
	Game(int,sf::UdpSocket*,ClientProxy*,std::string,int, float);
	~Game();
	void checkMsgList();
	void checkMoveSend(sf::Time);
	void checkResend(sf::Time);
	void update(sf::Time);
	void addMsg(InMsg);


	std::string getCreatorName();
	int getNumberOfPlayers();
	int getMaxNumberOfPlayers();

	void endGame();
	bool isEnded();
	

	void addPlayer(ClientProxy*);
	std::string getName();

};