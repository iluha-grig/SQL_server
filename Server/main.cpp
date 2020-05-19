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
#include "header_structures.h"
#include "socket.h"
#include "server.h"

int main_soc, main_semid, serv_pid;

void handler(int sig)
{
	if (sig == SIGINT)
	{
		if (getpid() == serv_pid)
		{
			// server main process
			std::cout << std::endl;
			std::cout << "Closing server." << std::endl;
			semctl(main_semid, 0, IPC_RMID, 0);
			shutdown(main_soc, 2);
			close(main_soc);
			// log
			exit(0);
		}
		else
		{
			//server worker
			// log
			exit(0);
		}
	}
}

/*
void handler_child(int sig)
{
	if (sig == SIGCHLD)
	{
		std::cout << "Finished work with client." << std::endl;
		// log
	}
}
*/

int main()
{
	std::cout << "Server socket listener process PID: " << getpid() << std::endl;
	serv_pid = getpid();
	if (signal(SIGINT, handler) == SIG_ERR)
	{
		perror("Error occurred in signal func");
		exit(10);
	}
	struct sigaction sig_act;
	sig_act.sa_handler = SIG_DFL;
	sig_act.sa_flags = SA_NOCLDWAIT;
	if (sigaction(SIGCHLD, &sig_act, nullptr) == -1)
	{
		perror("Error occurred in sigaction func");
		exit(11);
	}
	main_semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0777);
	if (main_semid == -1)
	{
		perror("Error occurred in semget func");
		exit(4);
	}
	if (semctl(main_semid, 0, SETVAL, 1) == -1)
	{
		perror("Error occurred in semctl func");
		exit(5);
	}
	int client_soc;
	socklen_t client_addr_len;
	main_soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if (main_soc == -1)
	{
		perror("Error occurred while creating socket");
		exit(1);
	}
	sockaddr_in addr_serv;
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_port = htons(8000);
	addr_serv.sin_addr.s_addr = inet_addr("192.168.0.109");
	if (bind(main_soc, (sockaddr *)&addr_serv, sizeof(addr_serv)) == -1)
	{
		perror("Error occurred while binding socket");
		exit(2);
	}
	if (listen(main_soc, 128) == -1)
	{
		perror("Error occurred in listen function");
		exit(3);
	}
	std::cout << "Start listenning socket..." << std::endl;
	while (true)
	{
		if ((client_soc = accept(main_soc, nullptr, &client_addr_len)) == -1)
		{
			perror("Error occurred in accept function");
			// log!!!
		}
		else
		{
			std::cout << "Accepted new client." << std::endl;
			// log
			if (fork() == 0)
			{
				Socket_POSIX_Server soc(client_soc);
				select_t sel;
				insert ins;
				update upd;
				del de;
				create cre;
				drop dro;
				where1 wh1;
				where2 wh2;
				where3 wh3;
				where4 wh4;
				answer ans;
				where_base *wh_ptr;
				int type, type_where;
				char buf[1000001];
				while (true)
				{
					try
					{
						soc.recv_from_client(&type, 4, MSG_WAITALL);
						soc.recv_from_client(&type_where, 4, MSG_WAITALL);
					}
					catch (const std::system_error& e)
					{
						// log
						exit(0);
					}
					try
					{
						if (type == 1)
						{
							sel.clear();
							memset(buf, 0, 1000001);
							int len;
							soc.recv_from_client(&len, 4, MSG_WAITALL);
							soc.recv_from_client(buf, len, MSG_WAITALL);
							sel.name.append(buf);
							memset(buf, 0, 1000001);
							int vec_len;
							soc.recv_from_client(&vec_len, 4, MSG_WAITALL);
							for (int i = 0; i < vec_len; ++i)
							{
								soc.recv_from_client(&len, 4, MSG_WAITALL);
								soc.recv_from_client(buf, len, MSG_WAITALL);
								sel.fields.push_back(std::string(buf));
								memset(buf, 0, 1000001);
							}
						}
						else if (type == 2)
						{
							ins.clear();
							memset(buf, 0, 1000001);
							int len, vec_len, int_tmp;
							long long tmp;
							soc.recv_from_client(&len, 4, MSG_WAITALL);
							soc.recv_from_client(buf, len, MSG_WAITALL);
							ins.name.append(buf);
							memset(buf, 0, 1000001);
							soc.recv_from_client(&vec_len, 4, MSG_WAITALL);
							for (int i = 0; i < vec_len; ++i)
							{
								soc.recv_from_client(&len, 4, MSG_WAITALL);
								soc.recv_from_client(buf, len, MSG_WAITALL);
								ins.fields_str.push_back(std::string(buf));
								memset(buf, 0, 1000001);
							}
							soc.recv_from_client(&vec_len, 4, MSG_WAITALL);
							for (int i = 0; i < vec_len; ++i)
							{
								soc.recv_from_client(&tmp, 8, MSG_WAITALL);
								ins.fields_num.push_back(tmp);
							}
							soc.recv_from_client(&vec_len, 4, MSG_WAITALL);
							for (int i = 0; i < vec_len; ++i)
							{
								soc.recv_from_client(&int_tmp, 4, MSG_WAITALL);
								ins.flags.push_back(int_tmp);
							}
						}
						else if (type == 3)
						{
							upd.clear();
							memset(buf, 0, 1000001);
							int len, vec_len;
							soc.recv_from_client(&len, 4, MSG_WAITALL);
							soc.recv_from_client(buf, len, MSG_WAITALL);
							upd.name.append(buf);
							memset(buf, 0, 1000001);
							soc.recv_from_client(&len, 4, MSG_WAITALL);
							soc.recv_from_client(buf, len, MSG_WAITALL);
							upd.field.append(buf);
							memset(buf, 0, 1000001);
							soc.recv_from_client(&vec_len, 4, MSG_WAITALL);
							for (int i = 0; i < vec_len; ++i)
							{
								soc.recv_from_client(&len, 4, MSG_WAITALL);
								soc.recv_from_client(buf, len, MSG_WAITALL);
								upd.expr.push_back(std::string(buf));
								memset(buf, 0, 1000001);
							}
						}
						else if (type == 4)
						{
							de.clear();
							memset(buf, 0, 1000001);
							int len;
							soc.recv_from_client(&len, 4, MSG_WAITALL);
							soc.recv_from_client(buf, len, MSG_WAITALL);
							de.name.append(buf);
							memset(buf, 0, 1000001);
						}
						else if (type == 5)
						{
							cre.clear();
							memset(buf, 0, 1000001);
							int len, vec_len;
							field_def tmp;
							soc.recv_from_client(&len, 4, MSG_WAITALL);
							soc.recv_from_client(buf, len, MSG_WAITALL);
							cre.name.append(buf);
							memset(buf, 0, 1000001);
							soc.recv_from_client(&vec_len, 4, MSG_WAITALL);
							for (int i = 0; i < vec_len; ++i)
							{
								soc.recv_from_client(&len, 4, MSG_WAITALL);
								soc.recv_from_client(buf, len, MSG_WAITALL);
								tmp.field.append(buf);
								memset(buf, 0, 1000001);
								soc.recv_from_client(&tmp.size, 8, MSG_WAITALL);
								cre.fields_def.push_back(std::move(tmp));
								tmp.field.clear();
							}
						}
						else
						{
							dro.clear();
							memset(buf, 0, 1000001);
							int len;
							soc.recv_from_client(&len, 4, MSG_WAITALL);
							soc.recv_from_client(buf, len, MSG_WAITALL);
							dro.name.append(buf);
							memset(buf, 0, 1000001);
						}
						if (type_where == 1)
						{
							wh1.clear();
							memset(buf, 0, 1000001);
							int len;
							soc.recv_from_client(&len, 4, MSG_WAITALL);
							soc.recv_from_client(buf, len, MSG_WAITALL);
							wh1.field.append(buf);
							memset(buf, 0, 1000001);
							soc.recv_from_client(&wh1.neg, 1, MSG_WAITALL);
							soc.recv_from_client(&len, 4, MSG_WAITALL);
							soc.recv_from_client(buf, len, MSG_WAITALL);
							wh1.str.append(buf);
							memset(buf, 0, 1000001);
							wh1.check_str();
							wh_ptr = &wh1;
						}
						else if (type_where == 2)
						{
							wh2.clear();
							memset(buf, 0, 1000001);
							int len, vec_len;
							long long tmp;
							soc.recv_from_client(&vec_len, 4, MSG_WAITALL);
							for (int i = 0; i < vec_len; ++i)
							{
								soc.recv_from_client(&len, 4, MSG_WAITALL);
								soc.recv_from_client(buf, len, MSG_WAITALL);
								wh2.expr.push_back(std::string(buf));
								memset(buf, 0, 1000001);
							}
							soc.recv_from_client(&wh2.neg, 1, MSG_WAITALL);
							soc.recv_from_client(&vec_len, 4, MSG_WAITALL);
							for (int i = 0; i < vec_len; ++i)
							{
								soc.recv_from_client(&len, 4, MSG_WAITALL);
								soc.recv_from_client(buf, len, MSG_WAITALL);
								wh2.list_str.push_back(std::string(buf));
								memset(buf, 0, 1000001);
							}
							soc.recv_from_client(&vec_len, 4, MSG_WAITALL);
							for (int i = 0; i < vec_len; ++i)
							{
								soc.recv_from_client(&tmp, 8, MSG_WAITALL);
								wh2.list_num.push_back(tmp);
							}
							wh_ptr = &wh2;
						}
						else if (type_where == 3)
						{
							wh3.clear();
							memset(buf, 0, 1000001);
							int len, vec_len;
							soc.recv_from_client(&vec_len, 4, MSG_WAITALL);
							for (int i = 0; i < vec_len; ++i)
							{
								soc.recv_from_client(&len, 4, MSG_WAITALL);
								soc.recv_from_client(buf, len, MSG_WAITALL);
								wh3.logic_expr.push_back(std::string(buf));
								memset(buf, 0, 1000001);
							}
							wh_ptr = &wh3;
						}
						else if (type_where == 4)
						{
							wh_ptr = &wh4;
						}
					}
					catch (const std::system_error& e)
					{
						// log
						exit(4);
					}
					catch (const std::logic_error& e)
					{
						int err = -1, len = strlen(e.what());
						soc.send_to_client(&err, 4);
						soc.send_to_client(&len, 4);
						soc.send_to_client(e.what(), len);
						continue;
					}
					try
					{
						if (type == 1)
						{
							std::vector<long long> sizes;
							key_t key = ftok(("OR/" + sel.name).data(), 'a');
							if (key == -1)
							{
								throw std::ios_base::failure("File not exists or another file error occurred");
							}
							int semid = semget(key, 0, 0);
							if (semid == -1)
							{
								throw std::ios_base::failure("File not exists or another file error occurred");
							}
							sembuf semop_down{0, -1, 0}, semop_up{0, 1, 0};
							if (semop(semid, &semop_down, 1) == -1)
							{
								throw std::ios_base::failure("File not exists or another file error occurred");
							}
							ans.clear();
							try
							{
								Select obj(&sel, wh_ptr);
								ans = obj.make_select();
								sizes = obj.get_head();
							}
							catch (const std::logic_error& e)
							{
								semop(semid, &semop_up, 1);
								throw;
							}
							catch (const std::ios_base::failure& e)
							{
								semop(semid, &semop_up, 1);
								throw;
							}
							semop(semid, &semop_up, 1);
							int err = 0, vec_len1 = ans.ans.size(), vec_len2, len, head_len = sizes.size();
							soc.send_to_client(&err, 4);
							soc.send_to_client(&head_len, 4);
							for (int i = 0; i < head_len; ++i)
							{
								soc.send_to_client(&sizes[i], 8);
							}
							soc.send_to_client(&vec_len1, 4);
							for (int i = 0; i < vec_len1; ++i)
							{
								vec_len2 = ans.ans[i].size();
								soc.send_to_client(&vec_len2, 4);
								for (int j = 0; j < vec_len2; ++j)
								{
									len = ans.ans[i][j].size();
									soc.send_to_client(&len, 4);
									soc.send_to_client(ans.ans[i][j].data(), len);
								}
							}
						}
						else if (type == 2)
						{
							std::vector<long long> sizes;
							key_t key = ftok(("OR/" + ins.name).data(), 'a');
							if (key == -1)
							{
								throw std::ios_base::failure("File not exists or another file error occurred");
							}
							int semid = semget(key, 0, 0);
							if (semid == -1)
							{
								throw std::ios_base::failure("File not exists or another file error occurred");
							}
							sembuf semop_down{0, -1, 0}, semop_up{0, 1, 0};
							if (semop(semid, &semop_down, 1) == -1)
							{
								throw std::ios_base::failure("File not exists or another file error occurred");
							}
							ans.clear();
							try
							{
								Insert obj(&ins);
								ans = obj.make_insert();
								sizes = obj.get_head();
							}
							catch (const std::logic_error& e)
							{
								semop(semid, &semop_up, 1);
								throw;
							}
							catch (const std::ios_base::failure& e)
							{
								semop(semid, &semop_up, 1);
								throw;
							}
							semop(semid, &semop_up, 1);
							int err = 0, vec_len1 = ans.ans.size(), vec_len2, len, head_len = sizes.size();
							soc.send_to_client(&err, 4);
							soc.send_to_client(&head_len, 4);
							for (int i = 0; i < head_len; ++i)
							{
								soc.send_to_client(&sizes[i], 8);
							}
							soc.send_to_client(&vec_len1, 4);
							for (int i = 0; i < vec_len1; ++i)
							{
								vec_len2 = ans.ans[i].size();
								soc.send_to_client(&vec_len2, 4);
								for (int j = 0; j < vec_len2; ++j)
								{
									len = ans.ans[i][j].size();
									soc.send_to_client(&len, 4);
									soc.send_to_client(ans.ans[i][j].data(), len);
								}
							}
						}
						else if (type == 3)
						{
							std::vector<long long> sizes;
							key_t key = ftok(("OR/" + upd.name).data(), 'a');
							if (key == -1)
							{
								throw std::ios_base::failure("File not exists or another file error occurred");
							}
							int semid = semget(key, 0, 0);
							if (semid == -1)
							{
								throw std::ios_base::failure("File not exists or another file error occurred");
							}
							sembuf semop_down{0, -1, 0}, semop_up{0, 1, 0};
							if (semop(semid, &semop_down, 1) == -1)
							{
								throw std::ios_base::failure("File not exists or another file error occurred");
							}
							ans.clear();
							try
							{
								Update obj(&upd, wh_ptr);
								ans = obj.make_update();
								sizes = obj.get_head();
							}
							catch (const std::logic_error& e)
							{
								semop(semid, &semop_up, 1);
								throw;
							}
							catch (const std::ios_base::failure& e)
							{
								semop(semid, &semop_up, 1);
								throw;
							}
							semop(semid, &semop_up, 1);
							int err = 0, vec_len1 = ans.ans.size(), vec_len2, len, head_len = sizes.size();
							soc.send_to_client(&err, 4);
							soc.send_to_client(&head_len, 4);
							for (int i = 0; i < head_len; ++i)
							{
								soc.send_to_client(&sizes[i], 8);
							}
							soc.send_to_client(&vec_len1, 4);
							for (int i = 0; i < vec_len1; ++i)
							{
								vec_len2 = ans.ans[i].size();
								soc.send_to_client(&vec_len2, 4);
								for (int j = 0; j < vec_len2; ++j)
								{
									len = ans.ans[i][j].size();
									soc.send_to_client(&len, 4);
									soc.send_to_client(ans.ans[i][j].data(), len);
								}
							}
						}
						else if (type == 4)
						{
							std::vector<long long> sizes;
							key_t key = ftok(("OR/" + de.name).data(), 'a');
							if (key == -1)
							{
								throw std::ios_base::failure("File not exists or another file error occurred");
							}
							int semid = semget(key, 0, 0);
							if (semid == -1)
							{
								throw std::ios_base::failure("File not exists or another file error occurred");
							}
							sembuf semop_down{0, -1, 0}, semop_up{0, 1, 0};
							if (semop(semid, &semop_down, 1) == -1)
							{
								throw std::ios_base::failure("File not exists or another file error occurred");
							}
							ans.clear();
							try
							{
								Delete obj(&de, wh_ptr);
								ans = obj.make_delete();
								sizes = obj.get_head();
							}
							catch (const std::logic_error& e)
							{
								semop(semid, &semop_up, 1);
								throw;
							}
							catch (const std::ios_base::failure& e)
							{
								semop(semid, &semop_up, 1);
								throw;
							}
							semop(semid, &semop_up, 1);
							int err = 0, vec_len1 = ans.ans.size(), vec_len2, len, head_len = sizes.size();
							soc.send_to_client(&err, 4);
							soc.send_to_client(&head_len, 4);
							for (int i = 0; i < head_len; ++i)
							{
								soc.send_to_client(&sizes[i], 8);
							}
							soc.send_to_client(&vec_len1, 4);
							for (int i = 0; i < vec_len1; ++i)
							{
								vec_len2 = ans.ans[i].size();
								soc.send_to_client(&vec_len2, 4);
								for (int j = 0; j < vec_len2; ++j)
								{
									len = ans.ans[i][j].size();
									soc.send_to_client(&len, 4);
									soc.send_to_client(ans.ans[i][j].data(), len);
								}
							}
						}
						else if (type == 5)
						{
							sembuf semop_down{0, -1, 0}, semop_up{0, 1, 0};
							if (semop(main_semid, &semop_down, 1) == -1)
							{
								throw std::ios_base::failure("Server was stopped");
							}
							try
							{
								Create::make_create(&cre);
							}
							catch (const std::logic_error& e)
							{
								semop(main_semid, &semop_up, 1);
								throw;
							}
							int fd = open(("OR/" + cre.name).data(), O_CREAT | O_EXCL, 0777);
							if (fd == -1)
							{
								// log
								semop(main_semid, &semop_up, 1);
								exit(1);
							}
							close(fd);
							key_t key = ftok(("OR/" + cre.name).data(), 'a');
							if (key == -1)
							{
								// log
								semop(main_semid, &semop_up, 1);
								exit(1);
							}
							int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0777);
							if (semid == -1)
							{
								// log
								semop(main_semid, &semop_up, 1);
								exit(1);
							}
							if (semctl(semid, 0, SETVAL, 1) == -1)
							{
								// log
								semop(main_semid, &semop_up, 1);
								exit(1);
							}
							semop(main_semid, &semop_up, 1);
							int err = 0;
							soc.send_to_client(&err, 4);
						}
						else
						{
							sembuf semop_down{0, -1, 0}, semop_up{0, 1, 0};
							if (semop(main_semid, &semop_down, 1) == -1)
							{
								throw std::ios_base::failure("Server was stopped");
							}
							key_t key = ftok(("OR/" + dro.name).data(), 'a');
							if (key == -1)
							{
								semop(main_semid, &semop_up, 1);
								throw std::ios_base::failure("File not exists or another file error occurred");
							}
							int semid = semget(key, 0, 0);
							if (semid == -1)
							{
								// log
								semop(main_semid, &semop_up, 1);
								exit(1);
							}
							if (semop(semid, &semop_down, 1) == -1)
							{
								// log
								semop(main_semid, &semop_up, 1);
								exit(1);
							}
							unlink(dro.name.data());
							if (semctl(semid, 0, IPC_RMID, 0) == -1)
							{
								// log
								semop(main_semid, &semop_up, 1);
								exit(1);
							}
							unlink(("OR/" + dro.name).data());
							semop(main_semid, &semop_up, 1);
							int err = 0;
							soc.send_to_client(&err, 4);
						}
					}
					catch (const std::logic_error& e)
					{
						int err = -1, len = strlen(e.what());
						soc.send_to_client(&err, 4);
						soc.send_to_client(&len, 4);
						soc.send_to_client(e.what(), len);
						continue;
					}
					catch (const std::ios_base::failure& e)
					{
						int err = -1, len = strlen(e.what());
						soc.send_to_client(&err, 4);
						soc.send_to_client(&len, 4);
						soc.send_to_client(e.what(), len);
						if (strcmp(e.what(), "Server was stopped") == 0)
						{
							// log
							exit(2);
						}
						continue;
					}
					catch (const std::system_error& e)
					{
						// log
						exit(3);
					}
				}
			}
		}
	}
}
