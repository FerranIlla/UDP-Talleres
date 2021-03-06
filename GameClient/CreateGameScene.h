#pragma once
#include "Scene.h"
#include "Button.h"
#include <utils.h>
#include "NetworkFunctions.h"
#include <queue>
#include <map>
#include "SceneManager.h"

class SceneGame;

class CreateGameScene :public Scene {
private:
	sf::UdpSocket* socket;
	std::queue<std::string>* serverMessages;
	std::map<int, outMsg>* outMessages;
	int msgId;
	int myId;
	std::string playerNick;
	sf::Vector2f mousePos;
	sf::Font font;


	Button* btn_Back;
	sf::Text txt_nick;

	sf::Text txt_players, txt_food;
	int numberOfPlayers;
	float foodRate;
	Button* btn_Create;
	Button* btn_playersMore, *btn_playersLess, *btn_foodMore, *btn_foodLess;

public:
	CreateGameScene(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f);

	void Update(sf::Time delta);
	void checkInput(sf::RenderWindow*, sf::Time deltaTime);
	void checkReceivedMsg(sf::RenderWindow*);
	void draw(sf::RenderWindow*);
};