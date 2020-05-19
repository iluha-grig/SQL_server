#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

class Socket_Win
{
protected:

	WSADATA w_data;
	SOCKET soc;
	sockaddr_in addr;
	explicit Socket_Win(int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);
	Socket_Win(SOCKET s);
	Socket_Win(SOCKET s, const sockaddr_in& a);
	Socket_Win(SOCKET s, sockaddr_in&& a);
	Socket_Win(const sockaddr_in& a, int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);
	Socket_Win(sockaddr_in&& a, int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);
	Socket_Win(const Socket_Win&) = delete;
	Socket_Win(Socket_Win&&) = delete;
	~Socket_Win();
public:

	void make_socket(int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);
	void set_myaddr(const sockaddr_in& a);
	void set_myaddr(sockaddr_in&& a);
	void set_myaddr(int family, unsigned short port, const char* ip);
};

class Socket_Win_Client final : public Socket_Win
{
	sockaddr_in addr_serv;
public:

	explicit Socket_Win_Client(int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);
	Socket_Win_Client(SOCKET s);
	Socket_Win_Client(SOCKET s, const sockaddr_in& a);
	Socket_Win_Client(SOCKET s, sockaddr_in&& a);
	Socket_Win_Client(SOCKET s, const sockaddr_in& a1, const sockaddr_in& a2);
	Socket_Win_Client(SOCKET s, sockaddr_in&& a1, sockaddr_in&& a2);
	Socket_Win_Client(const sockaddr_in& a, int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);
	Socket_Win_Client(sockaddr_in&& a, int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);
	Socket_Win_Client(const sockaddr_in& a1, const sockaddr_in& a2, int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);
	Socket_Win_Client(sockaddr_in&& a1, sockaddr_in&& a2, int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);
	~Socket_Win_Client();
	void set_server_addr(const sockaddr_in& a);
	void set_server_addr(sockaddr_in&& a);
	void set_server_addr(int family, unsigned short port, const char* ip);
	void connect_to_server();
	int send_to_server(void* buf, int len, int flags = 0);
	int send_to_server(const char* buf, int len, int flags = 0);
	int recv_from_server(void* buf, int len, int flags = 0);
};