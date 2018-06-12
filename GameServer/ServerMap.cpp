#include "ServerMap.h"

ServerMap::ServerMap(sf::Vector2i size, float foods):Map(size) {
	foodId = -1;
	foodCreationInterval = sf::seconds(foods);
	spawnFood();
}

void ServerMap::spawnFood() {
	foodId = (foodId + 1) % 255;
	sf::Vector2f* newFood=new sf::Vector2f(rand() % (size.x - 100) + 50, rand() % (size.y - 100) + 50);
	food.emplace(foodId, newFood);
	lastFoodCreation = sf::seconds(0);
}


bool ServerMap::update(sf::Time delta) {
	lastFoodCreation += delta;
	if (lastFoodCreation > foodCreationInterval) {
		spawnFood();
		return true;
	}
	return false;
}