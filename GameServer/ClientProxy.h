#pragma once
#define _USE_MATH_DEFINES
#include "timeDefines.h"
#include <string>
#include <map>
#include "ServerMap.h"
#include <utils.h>
#include <PlayerBase.h>

class ClientProxy : public PlayerBase {
public:
	std::string nickname;
	sf::Vector2f pos;
	Address address; //suposo que no cal perque hi ha el map
	sf::Time timeSincePing;
	int id;
	std::map<int, std::string> outMessages;
	
	//movimiento
	std::list<sf::Vector2f*>body;
	sf::Vector2f* tail;


	ClientProxy(ServerMap* map, Address ad, std::string name, int idPlayer);

	void resetPing();

	bool isDisconected(sf::Time deltaTime);

	void addOutMsg(std::string s, int id);

	void movePlayer(float delta);
};