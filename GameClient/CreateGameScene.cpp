#include "CreateGameScene.h"

CreateGameScene::CreateGameScene(sf::UdpSocket* sock, std::queue<std::string>* sMsg, std::map<int, outMsg>* outMsg, int& pID, int& msgid, std::string nick, sf::Font f) {
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

	numberOfPlayers = 4;
	foodRate = 2.0;

	txt_players = sf::Text("Players: "+ std::to_string(numberOfPlayers), font, 18);
	txt_players.setPosition(300, 200);
	btn_playersMore = new Button(" +", "add", sf::Vector2f(460, 200), sf::Vector2f(30, 30), font, 25);
	btn_playersLess = new Button(" -", "substract", sf::Vector2f(500, 200), sf::Vector2f(30, 30), font, 25);

	txt_food = sf::Text("Food rate: "+ std::to_string(foodRate), font, 18);
	txt_food.setPosition(300, 280);
	btn_foodMore = new Button(" +", "add", sf::Vector2f(460, 280), sf::Vector2f(30, 30), font, 25);
	btn_foodLess = new Button(" -", "substract", sf::Vector2f(500, 280), sf::Vector2f(30, 30), font, 25);

	btn_Create = new Button("   Create", "create", sf::Vector2f(300, 450), sf::Vector2f(100, 30), font, 18);
	
}

void CreateGameScene::Update(sf::Time deltaTime) {
	//update button states
	btn_Back->update(mousePos);
	btn_Create->update(mousePos);
	btn_playersMore->update(mousePos);
	btn_playersLess->update(mousePos);
	btn_foodMore->update(mousePos);
	btn_foodLess->update(mousePos);

	

}
void  CreateGameScene::checkInput(sf::RenderWindow*window, sf::Time deltaTime) {
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
			if (btn_playersMore->checkClick(mousePos)) {
				btn_playersMore->unselect();
				if(numberOfPlayers < 6) numberOfPlayers++; //MAXPLAYERS

				txt_players.setString("Players: " + std::to_string(numberOfPlayers));
			}
			else if (btn_playersLess->checkClick(mousePos) && !btn_playersLess->isBlocked()) {
				btn_playersLess->unselect();
				if (numberOfPlayers > 2) --numberOfPlayers;

				txt_players.setString("Players: " + std::to_string(numberOfPlayers));
			}
			else if (btn_foodMore->checkClick(mousePos) && !btn_foodMore->isBlocked()) {
				btn_foodMore->unselect();
				if (foodRate < 3.f) foodRate += 0.5f;

				txt_food.setString("Food rate: " + std::to_string(foodRate));
			}
			else if (btn_foodLess->checkClick(mousePos) && !btn_foodLess->isBlocked()) {
				btn_foodLess->unselect();
				if (foodRate > 0.5f) foodRate -= 0.5f;

				txt_food.setString("Food rate: " + std::to_string(foodRate));
			}

			if (btn_Back->checkClick(mousePos)) {
				SceneManager::Instance().changeToMenu(socket, serverMessages, outMessages, myId, msgId, playerNick, font);
			}

			if (btn_Create->checkClick(mousePos)) {
				
				std::string s = std::to_string(TypeOfMessage::CreateGame) + "_" + std::to_string(msgId) + "_" + std::to_string(numberOfPlayers) + "_" + std::to_string(foodRate);
				sendNew(s, socket, msgId, outMessages);
				SceneManager::Instance().changeToRoom(socket, serverMessages, outMessages, myId, msgId, playerNick, font);

				//sendNew(create+"_"+numberOfPlayers+"_"+foodRate...)
			}

			break;
		}
	}
}
void  CreateGameScene::checkReceivedMsg(sf::RenderWindow*window) {
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
		//if (type == TypeOfMessage::Join) //recieve game info && changeToGameScene
		//if (type == TypeOfMessage::Error) //cant create

		serverMessages->pop();
	}
}

void  CreateGameScene::draw(sf::RenderWindow*window) {
	window->draw(txt_nick);
	window->draw(txt_players);
	window->draw(txt_food);
	btn_Back->draw(window); 
	btn_Create->draw(window);
	btn_playersMore->draw(window);
	btn_playersLess->draw(window);
	btn_foodMore->draw(window);
	btn_foodLess->draw(window);


}