#ifndef SOCKET_H
#define SOCKET_H

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <functional>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 8888
#define PORT_STR "8888"

#define BACK_PORT 8887

void initialize_connections(std::function<void(std::string)> callback);

class Socket
{
private:
	SOCKET _socket;
	std::string ip;
	int port;

public:
	Socket(std::string _ip = "");
	Socket(const Socket& other);
	~Socket();
	Socket& operator=(const Socket& other);

	std::string getIP() const;
	int getPort() const;
	bool invalid() const;

	void Send(std::string msg);
	std::string Recv(int size);
	void Close();
};

#endif // !SOCKET_H
