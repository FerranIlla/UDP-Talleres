#include "utils.h"

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


float length(sf::Vector2f vector) {
return sqrt(vector.x*vector.x+vector.y*vector.y);
}  

float dotProduct(sf::Vector2f first, sf::Vector2f second) {
return first.x*second.x + first.y*second.y;
}
sf::Vector2f rotate(sf::Vector2f vec, float angle) {
	sf::Vector2f ret;
	float theCos = cos(angle);
	float theSin = sin(angle);
	ret.x = (theCos * vec.x) - (theSin * vec.y);
	ret.y = (theSin * vec.x) + (theCos * vec.y);
	return ret;
}



sf::Vector2f normalize(sf::Vector2f vec) {
return vec / length(vec);
}

