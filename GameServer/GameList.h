#pragma once
#include "Game.h"
#include <map>

class GameList {
private:
	std::map<int, Game*> gamelist;
	int lastGameID;

public:
	GameList();
	~GameList();

	bool AddGame(ClientProxy* creator,sf::UdpSocket* sock, int numberOfPlayers, float foodSpawn);
	void RemoveGame(int id);
	void checkEndedGames();
	void insertNewMsg(InMsg msg, int gameID);

	void update(sf::Time deltaTime);

	std::string getNames();

	void addPlayerToGame(int, ClientProxy*);
};