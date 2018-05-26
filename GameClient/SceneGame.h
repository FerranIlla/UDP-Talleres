#pragma once
#include "Scene.h"
#include "SFML\Graphics.hpp"
#include "SFML\Network.hpp"
#include <queue>
#include <map>
#include "NetworkFunctions.h"
#include "PlayerClient.h"

class SceneGame :public Scene {
private:
	sf::RenderWindow window;
	sf::UdpSocket* socket;
	std::queue<std::string>* serverMessages;
	std::map<int, outMsg>* outMessages;
	int* msgId;
	std::map <int, Player*>players;
	sf::Vector2f mousePos;
	int* myId;
	bool gameStarted;
	std::map<int, sf::CircleShape*> foods;

public:
	SceneGame(sf::RenderWindow* win,sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID ,int& msgid);

	void Update(float delta);
};