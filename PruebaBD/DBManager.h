#pragma once
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn\resultset.h>
#include <cppconn\statement.h>
#include <cppconn\exception.h>
#include <string>

class DBManager {
	sql::Driver* driver;
	sql::Connection* connection;
	sql::Statement* statement;
	sql::ResultSet* resultSet;

public:
	DBManager(std::string conexion, std::string name, std::string pwd);
	~DBManager();
	bool altaCuenta(std::string name, std::string pwd);
	bool login(std::string name, std::string pwd);
	void startSession(int id);
	void increaseNumberOfGames(int playerID);
	void endSession(int playerID);
};