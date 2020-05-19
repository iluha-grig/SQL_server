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

struct select_t
{
	std::string name;
	std::vector<std::string> fields;
	void clear();
};

struct insert
{
	std::string name;
	std::vector<std::string> fields_str;
	std::vector<long long> fields_num;
	std::vector<int> flags;
	void clear();
};

struct update
{
	std::string name;
	std::string field;
	std::vector<std::string> expr;
	void clear();
};

struct del
{
	std::string name;
	void clear();
};

struct field_def
{
	std::string field;
	long long size;
};

struct create
{
	std::string name;
	std::vector<field_def> fields_def;
	void clear();
};

struct drop
{
	std::string name;
	void clear();
};

struct where_base
{
	virtual bool choose(std::unordered_map<std::string, long long>&, std::unordered_map<std::string, std::string>&) const = 0;
	virtual ~where_base() = default;
};

struct where4 : where_base
{
	bool choose(std::unordered_map<std::string, long long>&, std::unordered_map<std::string, std::string>&) const override;
};

struct where1 : where_base
{
	std::string field;
	bool neg;
	std::string str;
	void clear();
	bool choose(std::unordered_map<std::string, long long>& head_map, std::unordered_map<std::string, std::string>& map) const override;
	void check_str();
private:

	enum class lex_type_t { CHAR, PER, UNDER, OPEN, CLOSE, XOR, DEF, END };
	std::stringstream ss;

	lex_type_t cur_lex_type;
	int cur_lex_text;
	int c;
	void lexer_init();
	void next();

	int c1, c2;
	void init();
	void S();
	void S1();
	void A();
	void B();
	void B1();
	void B2();
	void C();
	void D();

	bool like(std::string::const_iterator iter1, std::string::const_iterator iter2, std::string::const_iterator iter1_end, std::string::const_iterator iter2_end) const;
};

struct where2 : where_base
{
	std::vector<std::string> expr;
	bool neg;
	std::vector<std::string> list_str;
	std::vector<long long> list_num;
	void clear();
	bool choose(std::unordered_map<std::string, long long>& head_map, std::unordered_map<std::string, std::string>& map) const override;
};

struct where3 : where_base
{
	std::vector<std::string> logic_expr;
	void clear();
	bool choose(std::unordered_map<std::string, long long>& head_map, std::unordered_map<std::string, std::string>& map) const override;
};

struct answer
{
	std::vector<std::vector<std::string>> ans;
	void clear();
};
