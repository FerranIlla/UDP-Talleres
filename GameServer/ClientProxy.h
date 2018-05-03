#pragma once
#include "timeDefines.h"
#include <string>
#include <map>
#include "ServerMap.h"
#include <utils.h>

class ClientProxy {
public:
	std::string nickname;
	sf::Vector2f pos;
	Address address; //suposo que no cal perque hi ha el map
	sf::Time timeSincePing;
	int id;
	std::map<int, std::string> outMessages;

	ClientProxy(ServerMap* map, Address ad, std::string name, int idPlayer);

	void resetPing();

	bool isDisconected(sf::Time deltaTime);

	void addOutMsg(std::string s, int id);

	void movePlayer(sf::Vector2f target, float delta);
};