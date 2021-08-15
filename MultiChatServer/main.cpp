#include <iostream>
#include <atomic>
#include <functional>

#include "User.h"

std::vector<User> all_users;
std::queue<std::string> pendingMsg;

std::mutex mtx_users, mtx_msg;

std::atomic<int> msgCount, usersCount;

void sendPendingMessages()
{
	while (true)
	{
		while (!pendingMsg.empty())
		{
			mtx_msg.lock();
			std::string msg = pendingMsg.front();
			pendingMsg.pop();
			mtx_msg.unlock();

			mtx_users.lock();

			std::vector<User>::iterator it = all_users.begin();
			while (it != all_users.end())
			{
				try
				{
					int roomID = atoi(msg.substr(5, 4).c_str());
					if ((*it).getRoomID() == roomID)
					{
						(*it).getSender().Send(msg);
					}

					++it;
				}
				catch (const std::exception e)
				{
					it = all_users.erase(it);
					usersCount.fetch_sub(1);
				}
			}

			msgCount.fetch_sub(1);

			mtx_users.unlock();
		}
	}
}

void handleUser(User user)
{
	mtx_users.lock();
	all_users.push_back(user);
	mtx_users.unlock();

	usersCount.fetch_add(1);

	while (true)
	{
		try
		{
			std::string _msg = user.getSocket().Recv(1024);

			int msgSize = atoi(_msg.substr(1, 2).c_str());
			std::string msg_ = _msg.substr(3, msgSize);

			std::string msg = user.generateMsg(msg_);
			user.sendMessagetoQueue(msg);

			msgCount.fetch_add(1);
		}
		catch (std::exception e)
		{
			break;
		}
	}
}

void listenConnections()
{
	msgCount.store(0);
	usersCount.store(0);

	while (true)
	{
		try
		{
			Socket recvSocket;

			std::string conString = recvSocket.Recv(1024);

			int usernameSize = atoi(conString.substr(1, 2).c_str());
			std::string username = conString.substr(3, usernameSize);
			int roomID = atoi(conString.substr(usernameSize + 3, 4).c_str());

			Socket sendSocket(recvSocket.getIP());

			User user(username, roomID, recvSocket, sendSocket);

			std::thread new_thread(&handleUser, user);
			new_thread.detach();
		}
		catch(std::exception e)
		{
			break;
		}
	}
}

int main()
{
	initialize_connections([](std::string msg) {std::cout << msg << std::endl; });

	std::thread t1(listenConnections);
	std::thread t2(sendPendingMessages);

	t2.detach();
	t1.join();

	return 0;
}