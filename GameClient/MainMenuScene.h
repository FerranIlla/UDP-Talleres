#pragma once
#include "Scene.h"
#include "Button.h"
#include <utils.h>
#include "NetworkFunctions.h"
#include <queue>
#include <map>
#include "SceneManager.h"

class SceneGame;

class MainMenuScene:public Scene{
private:
	sf::UdpSocket* socket;
	std::queue<std::string>* serverMessages;
	std::map<int, outMsg>* outMessages;
	int msgId;
	int myId;
	std::string playerNick;
	sf::Vector2f mousePos;
	sf::Font font;

	sf::Text txt_nick;

	Button* btn_CreateGame;
	Button* btn_JoinGame;
public:
	MainMenuScene(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f);

	void Update(sf::Time delta);
	void checkInput(sf::RenderWindow*, sf::Time deltaTime);
	void checkReceivedMsg(sf::RenderWindow*);
	void draw(sf::RenderWindow*);
};