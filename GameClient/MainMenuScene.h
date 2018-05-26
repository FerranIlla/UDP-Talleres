#pragma once
#include "Scene.h"

class SceneGame;

class MainMenuScene:public Scene{
public:
	MainMenuScene();

	void Update(sf::Time delta);
	void checkInput(sf::RenderWindow*, sf::Time deltaTime);
	void checkReceivedMsg(sf::RenderWindow*);
	void draw(sf::RenderWindow*);
};