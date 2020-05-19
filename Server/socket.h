#pragma once

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

// This class for server on UNIX after getting the socket to communicate
// with target client.

class Socket_POSIX_Server final
{
	int soc;
public:

	explicit Socket_POSIX_Server(int s);
	~Socket_POSIX_Server();
	int send_to_client(const void *buf, int len, int flags = 0);
	int recv_from_client(void *buf, int len, int flags = 0);
};