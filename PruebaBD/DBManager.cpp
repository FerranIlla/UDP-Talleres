#include "DBManager.h"

DBManager::DBManager(std::string conexion, std::string name, std::string pwd) {
	driver = get_driver_instance();
	connection = driver->connect(conexion.c_str(), name.c_str(), pwd.c_str());
	statement = connection->createStatement();
	statement->execute("USE gamedb");
}

bool DBManager::login(std::string name, std::string pwd) {
	std::string s = "select count(*) from UserAccounts where UserName='" + name + "' and Password='" + pwd + "';";
	resultSet = statement->executeQuery(s.c_str());
	if (resultSet->next()) {
		int num = resultSet->getInt(1);
		delete resultSet;
		if (num == 0) {
			return false;
		}
		return true;
	}
	return false;
}

bool DBManager::altaCuenta(std::string name, std::string pwd) {
	std::string s = "select count(*) from UserAccounts where UserName='" + name + "'";
	resultSet = statement->executeQuery(s.c_str());
	if (resultSet->next()) {
		int num = resultSet->getInt(1);
		delete resultSet;
		//si ya existe alguien con ese nombre
		if (num != 0) {
			std::cout << "Nombre ya existente\n";
			return false;
		}
		else {
			std::string insertStr = "INSERT INTO UserAccounts (UserName,Password) VALUES ('" + name + "','" + pwd + "');";
			statement->execute(insertStr.c_str());
		}

	}
	return false;
}

DBManager::~DBManager() {
	delete statement;
	delete connection;
}

void DBManager::startSession(int id) {
	std::string s = "INSERT INTO Sessions (UserID) VALUES (" + std::to_string(id) + ");";
	statement->execute(s.c_str());
}

void DBManager::endSession(int idSession) {
	std::string s = "UPDATE Sessions SET EndTime ";
}


