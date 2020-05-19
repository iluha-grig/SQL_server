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
#include "header_structures.h"

class Table
{
protected:
	std::string filename;
	std::ifstream fin;
	std::ofstream fout;
	std::vector<std::string> head;
	std::unordered_map<std::string, long long> head_map;
	std::ios::pos_type start;

	Table(const std::string& str);
	~Table();
	bool eof_in() noexcept;
	//bool eof_out() const noexcept; may be unsafe
	void start_pos_in();
	void end_pos_out();
	std::unordered_map<std::string, std::string> read_first_map();
	std::vector<std::string> read_first_vector();
	std::unordered_map<std::string, std::string> read_next_map();
	std::vector<std::string> read_next_vector();
	std::ostream& put_num(long long num);
	std::ostream& put_str(std::string str, long long len);
	void seek_back_in();
	
	// it is guaranteed that field exists in table
	// fout should stay on the first pos in current line
	void set_on_field_out(const std::string& field);
	void sync_in_out();
	void read_discard_line_in();
};

class Select : protected Table
{
	select_t* sel_ptr;
	where_base* wh_ptr;
public:
	Select(select_t* sel, where_base* ptr);
	answer make_select();
	std::vector<long long> get_head();
};

class Drop
{
public:
	static void make_drop(const std::string& filename);
};

class Create
{
public:
	static void make_create(create* ptr);
};

class Insert : protected Table
{
	insert* ins_ptr;
public:
	Insert(insert* ins);
	answer make_insert();
	std::vector<long long> get_head();
};

class Update : protected Table
{
	update* upd_ptr;
	where_base* wh_ptr;
public:
	Update(update* upd, where_base* wh);
	answer make_update();
	std::vector<long long> get_head();
};

class Delete : protected Table
{
	where_base* wh_ptr;
	FILE* tmp_file;
public:
	Delete(del* del, where_base* wh);
	~Delete();
	answer make_delete();
	std::vector<long long> get_head();
};
