#pragma once
#pragma once
#include "Scene.h"
#include "Button.h"
#include <utils.h>
#include "NetworkFunctions.h"
#include <queue>
#include <map>
#include "SceneManager.h"

class SceneGame;

class LoginScene :public Scene {
private:
	sf::UdpSocket* socket;
	std::queue<std::string>* serverMessages;
	std::map<int, outMsg>* outMessages;
	int msgId;
	int myId;
	std::string playerNick;
	sf::Vector2f mousePos;
	sf::Font font;

	bool registerScreen;
	
	sf::String nicknameInput, passwordInput;
	sf::Text txt_nickname;
	sf::Text txt_password;
	sf::Text txt_serverMsg;


	sf::RectangleShape editUsername, editPassword;
	bool editingUsername, editingPassword;

	Button* btn_Login;
	Button* btn_Register;
	
public:
	LoginScene(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& msgid, sf::Font f);

	void Update(sf::Time delta);
	void checkInput(sf::RenderWindow*, sf::Time deltaTime);
	void checkReceivedMsg(sf::RenderWindow*);
	void draw(sf::RenderWindow*);
};