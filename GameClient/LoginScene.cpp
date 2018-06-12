#include "LoginScene.h"

LoginScene::LoginScene(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& msgid, sf::Font f) {
	socket = sock;
	serverMessages = sMsg;
	outMessages = outMsg;
	msgId = msgid;
	font = f;

	mousePos = sf::Vector2f(0, 0);
	registerScreen = false;

	btn_Login = new Button(" Login", "login", sf::Vector2f(200, 250), sf::Vector2f(100, 30), font, 20);
	btn_Register = new Button(" Register", "register", sf::Vector2f(200, 310), sf::Vector2f(100, 30), font, 20);


	txt_nickname = sf::Text("Username: " + nicknameInput, font, 20);
	txt_nickname.setPosition(400, 250);
	txt_password = sf::Text("Password: " + passwordInput, font, 20);
	txt_password.setPosition(400, 310);
	txt_serverMsg = sf::Text("Error:", font, 17);
	txt_serverMsg.setPosition(400, 500);

	editUsername.setPosition(txt_nickname.getPosition());
	editUsername.setSize(sf::Vector2f(300, 30));
	editUsername.setFillColor(sf::Color(50, 50, 50, 255));
	editPassword.setPosition(txt_password.getPosition());
	editPassword.setSize(sf::Vector2f(300, 30));
	editPassword.setFillColor(sf::Color(50, 50, 50, 255));
	editingUsername = editingPassword = false;

}

void LoginScene::Update(sf::Time deltaTime) {
	btn_Login->update(mousePos);
	btn_Register->update(mousePos);
}

void LoginScene::checkInput(sf::RenderWindow* window, sf::Time deltaTime) {
	sf::Event evento;
	while (window->pollEvent(evento)) {
		switch (evento.type) {
		case sf::Event::Closed:
			window->close();
			break;
		case sf::Event::KeyPressed:
			if (evento.key.code == sf::Keyboard::Return) {
				if (btn_Login->isSelected()) {
					if (nicknameInput.getSize() > 0 && passwordInput.getSize() > 0) {
						std::string msj = std::to_string(TypeOfMessage::Login) + "_" + std::to_string(msgId) + "_" + nicknameInput + "_" + passwordInput; 
						sendNew(msj, socket, msgId, outMessages);

					}
				}
				else if (btn_Register->isSelected()) {
					std::string msj = std::to_string(TypeOfMessage::Register) + "_" + std::to_string(msgId) + "_" + nicknameInput + "_" + passwordInput;
					sendNew(msj, socket, msgId, outMessages);
				}
				
			}
			break;
		case sf::Event::MouseMoved:
			mousePos = sf::Vector2f(evento.mouseMove.x, evento.mouseMove.y);
			break;
		case sf::Event::MouseButtonPressed:
			if (btn_Login->checkClick(mousePos)) {
				registerScreen = false;
				btn_Register->unselect();
			}
			if (btn_Register->checkClick(mousePos)) {
				registerScreen = true;
				btn_Login->unselect();
			}

			if (editUsername.getGlobalBounds().contains(mousePos)) {
				editingUsername = true;
				editingPassword = false;
			}
			else if (editPassword.getGlobalBounds().contains(mousePos)) {
				editingPassword = true;
				editingUsername = false;
			}
			else {
				editingPassword = editingUsername = false;
			}

			break;
		case sf::Event::TextEntered:
			if (editingUsername) {
				if (evento.text.unicode >= 32 && evento.text.unicode <= 126)
					nicknameInput += (char)evento.text.unicode;
				else if (evento.text.unicode == 8 && nicknameInput.getSize() > 0)
					nicknameInput.erase(nicknameInput.getSize() - 1, nicknameInput.getSize());

				txt_nickname.setString("Username: " + nicknameInput);
			}
			else if (editingPassword) {
				if (evento.text.unicode >= 32 && evento.text.unicode <= 126)
					passwordInput += (char)evento.text.unicode;
				else if (evento.text.unicode == 8 && passwordInput.getSize() > 0)
					passwordInput.erase(passwordInput.getSize() - 1, passwordInput.getSize());

				txt_password.setString("Password: " + passwordInput);
			}
			break;
		}
	}
}

void LoginScene::checkReceivedMsg(sf::RenderWindow* window) {
	while (!serverMessages->empty()) {
		mu.lock();
		std::vector<std::string> words = commandToWords(serverMessages->front());
		mu.unlock();
		TypeOfMessage type = (TypeOfMessage)std::stoi(words[0]);
		if (type == TypeOfMessage::Ack) {
			outMessages->erase(std::stoi(words[1]));
		}
		else if (type == TypeOfMessage::Ping) {
		//std::cout << "Ping Recivido";
		sendNormal(std::to_string(TypeOfMessage::Ping), socket);
		}
		else if (type==TypeOfMessage::Login) {
			myId = std::stoi(words[2]);
			playerNick = words[3];
			std::string s = std::to_string(TypeOfMessage::Ack) + "_" + words[1];
			sendNormal(s, socket);
			SceneManager::Instance().changeToMenu(socket, serverMessages, outMessages, myId, msgId, playerNick, font);
			std::cout << "Entras\n";

		}
		else if (type == TypeOfMessage::ErrorRegister) {
			txt_serverMsg.setString("Nombre de usuario ya existente");
		}
		else if (type == TypeOfMessage::ErrorLogin) {
			txt_serverMsg.setString("Convinacion usuario contraseña incorrecto");
		}
		else if (type == TypeOfMessage::Disconnect) {
			txt_serverMsg.setString("Disconnected from the server");
		}
		if (type == TypeOfMessage::Hello) {
			txt_serverMsg.setString("Connected to the server");
			outMessages->erase(0);
		}//recieve idPlayer, start MainMenuScene
		//if (type == TypeOfMessage::ErrorLogin) //nick or password incorrect

		serverMessages->pop();
	}
}

void LoginScene::draw(sf::RenderWindow* window) {
	btn_Login->draw(window);
	btn_Register->draw(window);

	if (editingUsername) window->draw(editUsername);
	else if (editingPassword) window->draw(editPassword);

	window->draw(txt_nickname);
	window->draw(txt_password);
	window->draw(txt_serverMsg);
}