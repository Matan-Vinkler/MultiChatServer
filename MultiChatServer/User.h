#ifndef USER_H
#define USER_H

#define _CRT_SECURE_NO_WARNINGS

#include "Socket.h"
#include <string>
#include <queue>
#include <vector>
#include <cstdlib>
#include <format>
#include <mutex>

class User
{
private:
	std::string username;
	int userID;
	int roomID;
	Socket conn;
	Socket sender;

public:
	User(std::string username, int roomID, Socket& conn, Socket& sender);
	User(const User& other);
	~User();
	User& operator=(const User& other);

	std::string getUsername() const;
	int getUserID() const;
	int getRoomID() const;
	Socket getSocket() const;
	Socket getSender() const;

	std::string generateMsg(std::string msg) const;
	void sendMessagetoQueue(std::string msg) const;
};

#endif // !USER_H