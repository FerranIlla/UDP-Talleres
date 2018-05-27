#pragma once
#include "Scene.h"
#include "SceneGame.h"
#include "MainMenuScene.h"
#include "JoinGameScene.h"
#include "CreateGameScene.h"

class SceneManager {
public:
	Scene* scene;
	static SceneManager& Instance() {
		static SceneManager s;
		return s;
	}
	//void Update(float delta);
public:
	SceneManager() {}
	SceneManager(SceneManager const&) = delete;
	void operator=(SceneManager const&) = delete;

	void changeToSceneGame(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f);
	void changeToMenu(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f);
	void changeToJoin(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f);
	void changeToCreate(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f);

};