#pragma once
#include <SFML\Network.hpp>
#include <mutex>
#include <map>
#include <queue>
#include "utils.h"
#include <iostream>

typedef outMsgClient outMsg;

#define MaxDataRecived 100
#define ServerAdress "127.0.0.1"
#define ResendTime sf::milliseconds(250)
#define sendMovementTime sf::milliseconds(150)
#define MaxIdMsg 255


extern std::mutex mu;

void sendNew(std::string s, sf::UdpSocket* socket, int &id, std::map<int, outMsg>* outMessages);

void sendAck(int id, sf::UdpSocket* socket);

void sendNormal(std::string msg, sf::UdpSocket* socket);

void receive(sf::UdpSocket* socket, std::queue<std::string>* msgList, sf::RenderWindow* window);