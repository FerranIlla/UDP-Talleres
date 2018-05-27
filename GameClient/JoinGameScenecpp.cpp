#include "JoinGameScene.h"

JoinGameScene::JoinGameScene(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f) {
	socket = sock;
	serverMessages = sMsg;
	outMessages = outMsg;
	msgId = msgid;
	myId = pID;
	playerNick = nick;
	font = f;

	mousePos = sf::Vector2f(0, 0);

	txt_nick = sf::Text(playerNick, font, 18);
	txt_nick.setPosition(10, 10);

	btn_Back = new Button(" Back", "back", sf::Vector2f(20, 550), sf::Vector2f(80, 30), font, 18);

	timeSinceLastUpdateList = sf::milliseconds(0);
	maxTimeUpdateList = sf::milliseconds(10000); //cada 10 segundos
	//sendNew(list...) //ask for list


	//temporal for test
	listOfGames.push_back("hola\n3/4\nadeu");
	listOfGames.push_back("hola2\n0/2\nadeu");
	listOfGames.push_back("hola3\n2/2\nadeu");

	for (int i = 0; i < listOfGames.size(); i++) {
		Button* b = new Button(listOfGames[i], "game", sf::Vector2f(300, 100+90*i), sf::Vector2f(200, 70), font, 18);
		games.push_back(b);
	}
}

void JoinGameScene::Update(sf::Time deltaTime) {
	//update button states
	btn_Back->update(mousePos);

	for (int i = 0; i < games.size(); ++i) {
		games[i]->update(mousePos);
	}

	timeSinceLastUpdateList += deltaTime;
	if (timeSinceLastUpdateList > maxTimeUpdateList) {
		timeSinceLastUpdateList = sf::milliseconds(0);
		//sendNew(list...) //ask for list
	}
}
void  JoinGameScene::checkInput(sf::RenderWindow*window, sf::Time deltaTime) {
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
			for (int i = 0; i < games.size(); ++i) {
				if (games[i]->checkClick(mousePos)) {
					//send start game
					//change scene to gameScene or roomScene
					//SceneManager::Instance().changeToRoom(socket, serverMessages, outMessages, myId, msgId, playerNick, font);
				}
			}

			if (btn_Back->checkClick(mousePos)) {
				SceneManager::Instance().changeToMenu(socket, serverMessages, outMessages, myId, msgId, playerNick, font);
			}

			break;
		}
	}
}
void  JoinGameScene::checkReceivedMsg(sf::RenderWindow*window) {
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
		//if (type == TypeOfMessage::Join) //recieve game info and changeToRoomScene
		//if (type == TypeOfMessage::Error) //cant create or cant join

		serverMessages->pop();
	}
}

void  JoinGameScene::draw(sf::RenderWindow*window) {
	window->draw(txt_nick);
	btn_Back->draw(window);

	for (int i = 0; i < games.size(); ++i) {
		games[i]->draw(window);
	}
}