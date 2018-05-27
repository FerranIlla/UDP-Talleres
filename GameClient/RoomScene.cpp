#include "RoomScene.h"

RoomScene::RoomScene(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f) {
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

	btn_Leave = new Button(" Leave Room", "leave", sf::Vector2f(680, 550), sf::Vector2f(100, 30), font, 18);

	txt_waiting = sf::Text("Waiting for more players...", font, 20);
	txt_waiting.setPosition(300, 70);

	//chat
	message = ">";
	//std::vector<std::string> aMessages;
	txt_chat = sf::Text("", font, 14);
	txt_chat.setFillColor(sf::Color(0, 160, 0));
	txt_chat.setStyle(sf::Text::Bold);
	txt_chat.setPosition(0, 560);

	txt_message = sf::Text(message, font, 14);
	txt_message.setFillColor(sf::Color(0, 160, 0));
	txt_message.setStyle(sf::Text::Bold);
	txt_message.setPosition(0, 560);

	separator = sf::RectangleShape(sf::Vector2f(200, 5));
	separator.setFillColor(sf::Color(200, 200, 200, 255));
	separator.setPosition(0, 550);
}

void RoomScene::Update(sf::Time deltaTime) {
	//update button states
	btn_Leave->update(mousePos);



}
void  RoomScene::checkInput(sf::RenderWindow*window, sf::Time deltaTime) {
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
			if (btn_Leave->checkClick(mousePos)) {
				SceneManager::Instance().changeToMenu(socket, serverMessages, outMessages, myId, msgId, playerNick, font);
			}
			break;
		case sf::Event::KeyPressed:
			if (evento.key.code == sf::Keyboard::Return) {
				message[0] = ' ';
				std::string msj = "chat_" + message;
				//socket.send(msj.c_str(), msj.length());
				//sendNew...
				aMessages.push_back(playerNick+":"+message);
				if (aMessages.size() > 19)
					aMessages.erase(aMessages.begin(), aMessages.begin() + 1);
				
				message = ">";
			}
			break;
		case sf::Event::TextEntered:
			if (evento.text.unicode >= 32 && evento.text.unicode <= 126)
				message += (char)evento.text.unicode;
			else if (evento.text.unicode == 8 && message.getSize() > 0)
				message.erase(message.getSize() - 1, message.getSize());

			txt_message.setString(message);
			
			break;
		}
	}
}
void  RoomScene::checkReceivedMsg(sf::RenderWindow*window) {
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
		//if (type == TypeOfMessage::Start) //changeToGameScene
		//if (type == TypeOfMessage::Chat) //aMessages.pushBack(words[1]); check aMessage <= 19;

		serverMessages->pop();
	}
}

void  RoomScene::draw(sf::RenderWindow*window) {
	window->draw(txt_nick);
	window->draw(txt_waiting);
	btn_Leave->draw(window);

	//chat
	for (int i = 0; i < aMessages.size(); ++i) {
		std::string chatting = aMessages[i];
		txt_chat.setPosition(sf::Vector2f(0, (20 * i) + 120));
		txt_chat.setString(chatting);
		window->draw(txt_chat);

	}
	window->draw(txt_message);
	window->draw(separator);

}