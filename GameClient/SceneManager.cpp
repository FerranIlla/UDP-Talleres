#include "SceneManager.h"

void SceneManager::changeToSceneGame(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick) {
	scene = new SceneGame(sock, sMsg, outMsg, pID, msgid, nick);
}

void SceneManager::changeToMenu() {
	scene = new MainMenuScene;

}
