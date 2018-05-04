#include "Mapa.h"

Map::Map(sf::Vector2i size):size(size){
	
}

bool Map::isPlayerOutside(sf::Vector2f pos, float radius){
	if (pos.y - radius < 0) return true;
	else if (pos.y + radius > size.y) return true;
	else if (pos.x - radius < 0) return true;
	else if (pos.x + radius > size.x) return true;
	
	return false;
}

sf::Vector2i Map::getSize() {
	return size;
}