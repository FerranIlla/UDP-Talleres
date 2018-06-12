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
	int idPartida;
	std::map<int, std::string> outMessages;
	
	//movimiento
	std::list<sf::Vector2f*>body;
	sf::Vector2f* tail;
	bool isAlive;

	ClientProxy( Address ad, std::string name, int idPlayer);

	void resetPing();

	void SetPosition(sf::Vector2f);

	bool isDisconected(sf::Time deltaTime);

	void addOutMsg(std::string s, int id);

	void movePlayer(float delta);

	int checkFoodCollision(std::map<int, sf::Vector2f*>&foods);
	bool checkPlayersCollision(std::map<Address, ClientProxy*>&player);

	void grow();

	sf::Vector2f getHeadPos();
	int getRadius();
	void setTarget(sf::Vector2f t);
};