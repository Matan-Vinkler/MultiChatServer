#include "Socket.h"

bool init = false;
SOCKET s = INVALID_SOCKET;

void initialize_connections(std::function<void(std::string)> callback)
{
	int iResult;

	if (!init)
	{
		WSADATA wsa;
		iResult = WSAStartup(MAKEWORD(2, 2), &wsa);

		if (iResult != 0)
		{
			throw std::exception("WSAStartup failed");
		}

		init = true;
	}

	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, PORT_STR, &hints, &result);
	if (iResult != 0)
	{
		WSACleanup();
		throw std::exception("getaddrinfo failed");
	}

	s = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (s == INVALID_SOCKET)
	{
		WSACleanup();
		throw std::exception("socket failed");
	}

	iResult = bind(s, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		freeaddrinfo(result);
		WSACleanup();
		throw std::exception("bind failed");
	}
	freeaddrinfo(result);

	iResult = listen(s, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(s);
		WSACleanup();
		throw std::exception("listen failed");
	}

	callback("Initialized!");
}

Socket::Socket(std::string _ip)
{
	if (_ip == "")
	{
		SOCKADDR_IN addrinfo{ 0 };
		int sizeaddrinfo = sizeof(addrinfo);

		this->_socket = accept(s, (struct sockaddr*)&addrinfo, &sizeaddrinfo);
		if (this->_socket == INVALID_SOCKET)
		{
			closesocket(s);
			WSACleanup();
			throw std::exception("accept failed");
		}

		this->ip = std::string(inet_ntoa(addrinfo.sin_addr));
		this->port = (int)addrinfo.sin_port;
	}
	else
	{
		SOCKADDR_IN addrinfo{ 0 };
		int sizeaddrinfo = sizeof(addrinfo);
		addrinfo.sin_family = AF_INET;
		addrinfo.sin_addr.s_addr = inet_addr(_ip.c_str());
		addrinfo.sin_port = htons(BACK_PORT);

		this->_socket = socket(AF_INET, SOCK_STREAM, 0);
		int iResult = connect(this->_socket, (sockaddr*)&addrinfo, sizeaddrinfo);

		if (iResult != 0)
		{
			closesocket(this->_socket);
			WSACleanup();
			throw std::exception("connect failed");
		}

		this->ip = _ip;
		this->port = BACK_PORT;
	}
}

Socket::Socket(const Socket& other) : _socket(other._socket), ip(other.ip), port(other.port)
{
}

Socket::~Socket()
{
}

Socket& Socket::operator=(const Socket& other)
{
	this->_socket = other._socket;
	this->ip = other.ip;
	this->port = other.port;

	return *this;
}

std::string Socket::getIP() const
{
	return this->ip;
}

int Socket::getPort() const
{
	return this->port;
}

bool Socket::invalid() const
{
	return this->_socket == INVALID_SOCKET;
}

void Socket::Send(std::string msg)
{
	int iResult = send(this->_socket, msg.c_str(), msg.size(), 0);
	if (iResult == SOCKET_ERROR)
	{
		throw std::exception("send failed");
	}
}

std::string Socket::Recv(int size)
{
	char* buff = new char[size];
	int iResult = recv(this->_socket, buff, size, 0);

	if (iResult == SOCKET_ERROR || iResult == 0)
	{
		throw std::exception("recv failed");
	}

	return std::string(buff, iResult);
}

void Socket::Close()
{
	shutdown(this->_socket, SD_SEND);
	closesocket(this->_socket);

	this->_socket = INVALID_SOCKET;
	this->ip = nullptr;
	this->port = 0;
}
