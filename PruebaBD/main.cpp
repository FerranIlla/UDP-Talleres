#include "DBManager.h"
#include <iostream>

#define connexion "tcp://192.168.1.59:3306"
#define username "root"
#define pwd "linux123"

int main() {
	DBManager manager(connexion,username,pwd);

	manager.altaCuenta("David", "l");
	manager.altaCuenta("Juanito", "linux");
	

	manager.startSession(1);
	//if (manager.login("David", "l")) {
	//	std::cout << "estas dentro\n";
	//}
	//else {
	//	std::cout << "error\n";
	//}
	std::cout << "caca";
}