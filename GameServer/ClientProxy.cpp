#include "ClientProxy.h"

ClientProxy::ClientProxy(ServerMap* map, Address ad, std::string name, int idPlayer) {
	pos.x = rand() % (map->getSize().x - 200) + 100;
	pos.y = rand() % (map->getSize().y - 200) + 100;
	address = ad;
	nickname = name;
	timeSincePing = sf::milliseconds(0);
	id = idPlayer;
}

void ClientProxy::resetPing() {
	timeSincePing = sf::milliseconds(0);
}


bool ClientProxy::isDisconected(sf::Time deltaTime) {
	timeSincePing += deltaTime;
	return timeSincePing > DisconnectTime;
}

void ClientProxy::addOutMsg(std::string s, int id) {
	outMessages.emplace(id, s);
}