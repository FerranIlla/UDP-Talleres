#include "GameList.h"

GameList::GameList() {
	lastGameID = 0;
}

bool GameList::AddGame(ClientProxy* creator,sf::UdpSocket* sock,  int numberOfPlayers, float foodSpawnrate) {
	for (std::map<int, Game*>::iterator it = gamelist.begin(); it != gamelist.end(); ++it) {
		if (it->second->getCreatorName()==creator->nickname) {
			return false;
		}
	}
	std::string name = "Game " + std::to_string(gamelist.size()+1);
	Game* game=new Game(numberOfPlayers, sock, creator, name, lastGameID,foodSpawnrate);
	gamelist.emplace(lastGameID, game);
	lastGameID++;
	return true;
}

GameList::~GameList() {
	
}

void GameList::update(sf::Time deltaTime) {
	checkEndedGames();
	for (std::map<int, Game*>::iterator it = gamelist.begin(); it != gamelist.end(); ++it) {
		it->second->update(deltaTime);
	}
}

void GameList::RemoveGame(int id) {
	Game* game=gamelist.find(id)->second;
	game->endGame();
	delete game;
	gamelist.erase(id);
}
void GameList::insertNewMsg(InMsg msg, int gameID) {
	gamelist.find(gameID)->second->addMsg(msg);
}

void GameList::checkEndedGames() {
	std::list<int> gamesToErase;
	for (std::map<int, Game*>::iterator it = gamelist.begin(); it != gamelist.end(); it++) {
		if(it->second->isEnded())
			gamesToErase.push_back(it->first);
	}

	for (std::list<int>::iterator it = gamesToErase.begin(); it != gamesToErase.end(); ++it) {
		RemoveGame(*it);
	}
}

std::string GameList::getNames() {
	std::string ret = "_" + std::to_string(gamelist.size());

	for (std::map<int, Game*>::iterator it = gamelist.begin(); it != gamelist.end(); ++it) {
		ret += "_" + it->second->getName() + "\nby " + it->second->getCreatorName() + "\n" + std::to_string(it->second->getNumberOfPlayers()) + "/" + std::to_string(it->second->getMaxNumberOfPlayers())+"_"+ std::to_string(it->first);
	}
	return ret;
}

void GameList::addPlayerToGame(int gameId, ClientProxy* client) {
	gamelist.find(gameId)->second->addPlayer(client);
}