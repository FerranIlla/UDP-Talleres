#include "MainMenuScene.h"

MainMenuScene::MainMenuScene() {

}

void MainMenuScene::Update(sf::Time deltaTime) {

}
void  MainMenuScene::checkInput(sf::RenderWindow*window, sf::Time deltaTime) {
	sf::Event evento;
	while (window->pollEvent(evento)) {
		switch (evento.type) {
		case sf::Event::Closed:
			window->close();
			break;
		}
	}
}
void  MainMenuScene::checkReceivedMsg(sf::RenderWindow*window) {
	
}

void  MainMenuScene::draw(sf::RenderWindow*window) {

}