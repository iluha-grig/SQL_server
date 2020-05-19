# SQL_server
There are two parts: Client part and Server part. Client part works on Windows OS and was compiled with Microsoft Visual C++, so there is no Makefile for client part. Server part works on Linux OS and was compiled with g++ 7.5.0 version,
there is Makefile for server part.
The variant of task is 4.2.
Server works with different client using multiprocessing. All works are protected with semaphore sync and signals for each file separately.
Supports all checks except analysing OF and CF flags. User should remember that all computations proceed in 64-bit numbers (long on Linux and long long on Windows).
Program supports a lot of features. For example, server can be shutdowned in three ways:
1) Ctrl+C --- All works stop immediately.
2) SIGINT to main process --- New clients can't connect, working clients can only read and modify tables. If client try to create or drop table his program will be shutdowned. Reconnect is impossible.
3) SIGKILL to main process --- New clients can't connect, working clients can work while they want with no limits. Reconnect is impossible. !!!IN THIS MODE SERVER MACHINE SHOULD BE RESTARTED!!!

In working server directory should be only two files: ALL and OR. ALL is an executable file and OR must be an empty catalog. It is system catalog to provide semaphore works. Nothing else is needed to do, server program manages all system
files, shared resources, processes and zombies automatically. No TIME_WAIT problems.

Before compiling client part and server part you need to change server local ip address in two code lines to final server ip address which will be used.
1) In file Server/main.cpp in line 110 write your server ip address.
2) In file Client/main.cpp in line 53 write the same ip address (ip address of server).

All done using recursive descent with actions and one constant (on 3 positions) look ahead.

!!!After server machine reboot semaphores for existing files in OR/ directory should be created manually before starting server program. There should be one array of semaphores for each file in OR/ directory.
Each array consists one semaphore initialized with 1 value.!!!
