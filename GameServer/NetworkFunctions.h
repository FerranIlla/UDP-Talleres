#pragma once
#include <string>
#include "utils.h"
#include <mutex>
#include <queue>
#include <iostream>

#define MSG_LENGTH 512

#define MaxIdMsg 255;

extern std::mutex mu;

typedef outMsgServer outMsg;

struct InMsg {
	std::string msg;
	Address addr;

	InMsg(std::string s, Address ad) {
		msg = s;
		addr = ad;
	}
};

void myReceiveFunction(sf::UdpSocket *sock, std::queue<InMsg>* msgList, bool* open);

void sendNew(std::string s, sf::UdpSocket* socket, int &id, Address addr, std::map<int, std::string>* outMessages);

void sendNormal(std::string msg, sf::UdpSocket* socket, Address addr);
