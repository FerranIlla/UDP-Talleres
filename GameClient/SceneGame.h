#pragma once
#include "Scene.h"
#include "SFML\Graphics.hpp"
#include "SFML\Network.hpp"
#include <queue>
#include <map>
#include "NetworkFunctions.h"
#include "PlayerClient.h"
#include "ClientMap.h"
#include "SceneManager.h"
#include "MainMenuScene.h"

class SceneGame :public Scene {
private:
	sf::UdpSocket* socket;
	std::queue<std::string>* serverMessages;
	std::map<int, outMsg>* outMessages;
	int msgId;

	std::string playerNick;
	std::map <int, Player*>players;
	sf::Vector2f mousePos;
	int myId;

	bool gameStarted;

	std::map<int, sf::CircleShape*> foods;
	Walls walls;
	sf::Time timeLastMoveSend;
public:
	SceneGame(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID ,int& msgid, std::string nick);

	void Update(sf::Time delta );
	void checkInput(sf::RenderWindow*, sf::Time deltaTime);
	void checkReceivedMsg(sf::RenderWindow*);
	void draw(sf::RenderWindow*);

};