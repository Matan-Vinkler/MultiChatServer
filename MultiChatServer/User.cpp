#include "User.h"

std::vector<int> usedIDs;
std::mutex mtx_id;

extern std::queue<std::string> pendingMsg;
extern std::mutex mtx_msg;

std::string intTo4BytesStr(int n)
{
	std::string retn;

	if (n < 10)
	{
		retn = "000" + std::to_string(n);
	}
	else if (n >= 10 && n <= 99)
	{
		retn = "00" + std::to_string(n);
	}
	else if (n >= 100 && n <= 999)
	{
		retn = "0" + std::to_string(n);
	}
	else
	{
		retn = std::to_string(n);
	}

	return retn;
}

std::string intTo2BytesStr(int n)
{
	std::string retn;

	if (n < 10)
	{
		retn = "0" + std::to_string(n);
	}
	else
	{
		retn = std::to_string(n);
	}

	return retn;
}

User::User(std::string username, int roomID, Socket& conn, Socket& sender) : username(username), roomID(roomID), conn(conn), sender(sender)
{
	int id;
	bool flag = true;

	while (true)
	{
		id = std::rand() % 1000;

		mtx_id.lock();
		for (std::vector<int>::iterator it = usedIDs.begin(); it != usedIDs.end(); it++)
		{
			if (id == *it)
			{
				flag = false;
			}
		}
		mtx_id.unlock();

		if (flag) break;
	}

	this->userID = id;
	
	std::lock_guard<std::mutex> lg(mtx_id);
	usedIDs.push_back(id);
}

User::User(const User& other) : username(other.username), userID(other.userID), roomID(other.roomID), conn(other.conn), sender(other.sender)
{

}

User::~User()
{

}

User& User::operator=(const User& other)
{
	this->username = other.username;
	this->userID = other.userID;
	this->roomID = other.roomID;
	this->conn = other.conn;
	this->sender = other.sender;

	return *this;
}

std::string User::getUsername() const
{
	return this->username;
}

int User::getUserID() const
{
	return this->userID;
}

int User::getRoomID() const
{
	return this->roomID;
}

Socket User::getSocket() const
{
	return this->conn;
}

Socket User::getSender() const
{
	return this->sender;
}

std::string User::generateMsg(std::string msg) const
{
	char buff[1024];
	int n = sprintf(buff, "2%s%s%s%s%s%s", intTo4BytesStr(this->userID).c_str(), intTo4BytesStr(this->roomID).c_str(), intTo2BytesStr(this->username.size()).c_str(), this->username.c_str(), intTo2BytesStr(msg.size()).c_str(), msg.c_str());

	std::string retn(buff, n);
	return retn;
}

void User::sendMessagetoQueue(std::string msg) const
{
	std::lock_guard<std::mutex> lg1(mtx_msg);
	pendingMsg.push(msg);
}
