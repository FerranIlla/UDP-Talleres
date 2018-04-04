#include "Mapa.h"

Map::Map( sf::Vector2i s) {
	size = s;


}

sf::Vector2i Map::getSize() {
	return size;
}
