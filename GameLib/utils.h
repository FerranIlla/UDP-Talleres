#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include "SFML\Network.hpp"
#include "SFML\Graphics.hpp"	
#include <cmath>
#include <random>
#include <time.h>
#define RAD2DEG (180/M_PI)
#define DEG2RAD (M_PI/180)
#define PERCENT_PACKETLOST 0.0f

std::vector<std::string> commandToWords(std::string command);

sf::Vector2i charToDir(std::string str);

struct Address {
	sf::IpAddress ip;
	unsigned short port;

	friend bool operator< (const Address &lho, const Address &rho) {
		if (lho.ip < rho.ip) return true;
		else if (lho.ip == rho.ip) {
			if (lho.port < rho.port) return true;
		}
		return false;
	}
};

struct outMsgServer {
	std::string msg;
	Address to;
	float timeLastSend;

	outMsgServer(std::string txt) {
		timeLastSend = 0;
		msg = txt;
	}

	bool hasToResend(float delta, float waitTime) {
		waitTime += delta;
		if (timeLastSend >= waitTime) {
			timeLastSend -= waitTime;
			return true;
		}
		return false;
	}
};

struct outMsgClient {
	std::string msg;
	float timeLastSend;

	outMsgClient(std::string txt) {
		timeLastSend = 0;
		msg = txt;
	}

	bool hasToResend(float delta, float waitTime) {
		waitTime += delta;
		if (timeLastSend >= waitTime) {
			timeLastSend -= waitTime;
			return true;
		}
		return false;
	}
};

float length(sf::Vector2f vector);

float dotProduct(sf::Vector2f first, sf::Vector2f second);

sf::Vector2f crossProduct(sf::Vector2f first, sf::Vector2f second);

sf::Vector2f normalize(sf::Vector2f vec);

sf::Vector2f rotate(sf::Vector2f vec, float angle);

enum TypeOfMessage :int8_t {
	Hello, NewPlayer, Ack, Ping, Disconnect, Move
};


float getRandomFloat();

bool percentageGate(float percentageLost);