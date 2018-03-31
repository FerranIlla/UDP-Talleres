#pragma once
#include <vector>
#include "SFML\Graphics.hpp"	

std::vector<std::string> commandToWords(std::string command) {
	std::vector<std::string> subMsj(1, "");
	int wordCount = 0;
	//convertir el mensaje completo a palabras
	for (int i = 0; i < command.length(); ++i) {
		if (command[i] != '_') {
			subMsj[wordCount] += command[i];
		}
		else {
			subMsj.push_back("");
			wordCount++;
		}
	}
	return subMsj;
}

sf::Vector2i charToDir(std::string str) {
	sf::Vector2i direction;
	if (str == "r") {
		direction.x = 1;
		direction.y = 0;
	}
	else if (str == "l") {
		direction.x = -1;
		direction.y = 0;
	}
	else if (str == "u") {
		direction.x = 0;
		direction.y = -1;
	}
	else if (str == "d") {
		direction.x = 0;
		direction.y = 1;
	}

	return direction;
}