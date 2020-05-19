#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>
#include <cerrno>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <algorithm>
#include <cstdio>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include "socket.h"

Socket_POSIX_Server::Socket_POSIX_Server(int s) : soc(s) {}

Socket_POSIX_Server::~Socket_POSIX_Server()
{
	shutdown(soc, 2);
	close(soc);
}

int Socket_POSIX_Server::send_to_client(const void *buf, int len, int flags)
{
	int res = send(soc, buf, len, flags);
	if (res == -1)
	{
		throw std::system_error(std::error_code(), "Send function finished with error code " + std::to_string(errno));
	}
	return res;
}

int Socket_POSIX_Server::recv_from_client(void *buf, int len, int flags)
{
	int res = recv(soc, buf, len, flags);
	if (res == -1)
	{
		throw std::system_error(std::error_code(), "Recv function finished with error code " + std::to_string(errno));
	}
	if (res < len)
	{
		throw std::system_error(std::error_code(), "Connection closed by client or error occurred in client program");
	}
	return res;
}