#include "SceneManager.h"


void SceneManager::changeToSceneGame(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f,std::string players) {
	scene = new SceneGame(sock, sMsg, outMsg, pID, msgid, nick, f,players);

}

void SceneManager::changeToMenu(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f) {
	scene = new MainMenuScene(sock, sMsg,  outMsg, pID,  msgid, nick, f);

}

void SceneManager::changeToJoin(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f) {
	scene = new JoinGameScene(sock, sMsg, outMsg, pID, msgid, nick, f);

}

void SceneManager::changeToCreate(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f) {
	scene = new CreateGameScene(sock, sMsg, outMsg, pID, msgid, nick, f);

}

void SceneManager::changeToRoom(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f) {
	scene = new RoomScene(sock, sMsg, outMsg, pID, msgid, nick, f);
}
