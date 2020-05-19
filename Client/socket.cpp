#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment (lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <string>
#include "Header_socket.h"

Socket_Win::Socket_Win(int af, int type, int protocol)
{
	int error_code;
	if ((error_code = WSAStartup(MAKEWORD(2, 2), &w_data)) != 0)
	{
		throw std::system_error(std::error_code(), "WSAStartup finished with error code " + std::to_string(error_code));
	}
	soc = socket(af, type, protocol);
	if (soc == INVALID_SOCKET)
	{
		throw std::system_error(std::error_code(), "Socket function finished with error code " + std::to_string(WSAGetLastError()));
	}
}

Socket_Win::Socket_Win(SOCKET s)
{
	int error_code;
	if ((error_code = WSAStartup(MAKEWORD(2, 2), &w_data)) != 0)
	{
		throw std::system_error(std::error_code(), "WSAStartup finished with error code " + std::to_string(error_code));
	}
	if (s == INVALID_SOCKET)
	{
		throw std::system_error(std::error_code(), "Invalid socket descriptor");
	}
	soc = s;
}

Socket_Win::Socket_Win(SOCKET s, const sockaddr_in& a)
{
	int error_code;
	if ((error_code = WSAStartup(MAKEWORD(2, 2), &w_data)) != 0)
	{
		throw std::system_error(std::error_code(), "WSAStartup finished with error code " + std::to_string(error_code));
	}
	if (s == INVALID_SOCKET)
	{
		throw std::system_error(std::error_code(), "Invalid socket descriptor");
	}
	soc = s;
	addr = a;
}

Socket_Win::Socket_Win(SOCKET s, sockaddr_in&& a)
{
	int error_code;
	if ((error_code = WSAStartup(MAKEWORD(2, 2), &w_data)) != 0)
	{
		throw std::system_error(std::error_code(), "WSAStartup finished with error code " + std::to_string(error_code));
	}
	if (s == INVALID_SOCKET)
	{
		throw std::system_error(std::error_code(), "Invalid socket descriptor");
	}
	soc = s;
	addr = std::move(a);
}

Socket_Win::Socket_Win(const sockaddr_in& a, int af, int type, int protocol) : Socket_Win(af, type, protocol)
{
	addr = a;
}

Socket_Win::Socket_Win(sockaddr_in&& a, int af, int type, int protocol) : Socket_Win(af, type, protocol)
{
	addr = std::move(a);
}

Socket_Win::~Socket_Win()
{
	WSACleanup();
}

void Socket_Win::make_socket(int af, int type, int protocol)
{
	soc = socket(af, type, protocol);
	if (soc == INVALID_SOCKET)
	{
		throw std::system_error(std::error_code(), "Socket function finished with error code " + std::to_string(WSAGetLastError()));
	}
}

void Socket_Win::set_myaddr(const sockaddr_in& a)
{
	addr = a;
}

void Socket_Win::set_myaddr(sockaddr_in&& a)
{
	addr = std::move(a);
}

void Socket_Win::set_myaddr(int family, unsigned short port, const char* ip)
{
	addr.sin_family = family;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
}

Socket_Win_Client::Socket_Win_Client(int af, int type, int protocol) : Socket_Win(af, type, protocol) {}

Socket_Win_Client::Socket_Win_Client(SOCKET s) : Socket_Win(s) {}

Socket_Win_Client::Socket_Win_Client(SOCKET s, const sockaddr_in& a) : Socket_Win(s, a) {}

Socket_Win_Client::Socket_Win_Client(SOCKET s, sockaddr_in&& a) : Socket_Win(s, std::move(a)) {}

Socket_Win_Client::Socket_Win_Client(SOCKET s, const sockaddr_in& a1, const sockaddr_in& a2) : Socket_Win(s, a1), addr_serv(a2) {}

Socket_Win_Client::Socket_Win_Client(SOCKET s, sockaddr_in&& a1, sockaddr_in&& a2) : Socket_Win(s, std::move(a1)), addr_serv(std::move(a2)) {}

Socket_Win_Client::Socket_Win_Client(const sockaddr_in& a, int af, int type, int protocol) : Socket_Win(a, af, type, protocol) {}

Socket_Win_Client::Socket_Win_Client(sockaddr_in&& a, int af, int type, int protocol) : Socket_Win(std::move(a), af, type, protocol) {}

Socket_Win_Client::Socket_Win_Client(const sockaddr_in& a1, const sockaddr_in& a2, int af, int type, int protocol) : Socket_Win(a1, af, type, protocol), addr_serv(a2) {}

Socket_Win_Client::Socket_Win_Client(sockaddr_in&& a1, sockaddr_in&& a2, int af, int type, int protocol) : Socket_Win(std::move(a1), af, type, protocol), addr_serv(std::move(a2)) {}

Socket_Win_Client::~Socket_Win_Client()
{
	shutdown(soc, SD_BOTH);
	closesocket(soc);
}

void Socket_Win_Client::set_server_addr(const sockaddr_in& a)
{
	addr_serv = a;
}

void Socket_Win_Client::set_server_addr(sockaddr_in&& a)
{
	addr_serv = std::move(a);
}

void Socket_Win_Client::set_server_addr(int family, unsigned short port, const char* ip)
{
	addr_serv.sin_family = family;
	addr_serv.sin_port = htons(port);
	addr_serv.sin_addr.S_un.S_addr = inet_addr(ip);
}

void Socket_Win_Client::connect_to_server()
{
	if (connect(soc, (const sockaddr*)&addr_serv, sizeof(addr_serv)) == SOCKET_ERROR)
	{
		throw std::system_error(std::error_code(), "Connect function finished with error code " + std::to_string(WSAGetLastError()));
	}
}

int Socket_Win_Client::send_to_server(void* buf, int len, int flags)
{
	int res = send(soc, (const char*)buf, len, flags);
	if (res == SOCKET_ERROR)
	{
		throw std::system_error(std::error_code(), "Send function finished with error code " + std::to_string(WSAGetLastError()));
	}
	return res;
}

int Socket_Win_Client::send_to_server(const char* buf, int len, int flags)
{
	int res = send(soc, buf, len, flags);
	if (res == SOCKET_ERROR)
	{
		throw std::system_error(std::error_code(), "Send function finished with error code " + std::to_string(WSAGetLastError()));
	}
	return res;
}

int Socket_Win_Client::recv_from_server(void* buf, int len, int flags)
{
	int res = recv(soc, (char*)buf, len, flags);
	if (res == SOCKET_ERROR)
	{
		throw std::system_error(std::error_code(), "Recv function finished with error code " + std::to_string(WSAGetLastError()));
	}
	if (res < len)
	{
		throw std::system_error(std::error_code(), "Error on server occurred. Try to repeat your request later");
	}
	return res;
}