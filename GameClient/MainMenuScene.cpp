#include "MainMenuScene.h"

MainMenuScene::MainMenuScene(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f) {
	socket = sock;
	serverMessages = sMsg;
	outMessages = outMsg;
	msgId = msgid;
	myId = pID;
	playerNick = nick;
	font = f;

	mousePos = sf::Vector2f(0, 0);
	seeListOfGames = true;

	btn_Start = new Button("Start Game", "temp", sf::Vector2f(355, 52), sf::Vector2f(70, 48), font, 18);
}

void MainMenuScene::Update(sf::Time deltaTime) {
	//update button states
	btn_Start->update(mousePos);
}
void  MainMenuScene::checkInput(sf::RenderWindow*window, sf::Time deltaTime) {
	sf::Event evento;
	while (window->pollEvent(evento)) {
		switch (evento.type) {
		case sf::Event::Closed:
			window->close();
			break;
		case sf::Event::MouseMoved:
			mousePos = sf::Vector2f(evento.mouseMove.x, evento.mouseMove.y);
			break;
		case sf::Event::MouseButtonPressed:
			if (btn_Start->checkClick(mousePos)) {
				SceneManager::Instance().changeToSceneGame(socket, serverMessages, outMessages, myId, msgId, playerNick, font);
			}

			break;
		}
	}
}
void  MainMenuScene::checkReceivedMsg(sf::RenderWindow*window) {
	while (!serverMessages->empty()) {
		mu.lock();
		std::vector<std::string> words = commandToWords(serverMessages->front());
		mu.unlock();
		TypeOfMessage type = (TypeOfMessage)std::stoi(words[0]);
		if (type == TypeOfMessage::Ack) {
			outMessages->erase(std::stoi(words[1]));
		}
		/*else if (type == TypeOfMessage::Ping) {
			//std::cout << "Ping Recivido";
			sendNormal(std::to_string(TypeOfMessage::Ping), socket);
		}*/
		//if (type == TypeOfMessage::List)
		//if (type == TypeOfMessage::Join) //recieve game info
		//if (type == TypeOfMessage::Error) //cant create or cant join
		//if (type == TypeOfMessage::Chat)

		serverMessages->pop();
	}
}

void  MainMenuScene::draw(sf::RenderWindow*window) {
	btn_Start->draw(window);

}