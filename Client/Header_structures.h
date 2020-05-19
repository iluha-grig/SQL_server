#pragma once

#include <string>
#include <vector>

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

struct where1
{
	std::string field;
	bool neg;
	std::string str;
	void clear();
};

struct where2
{
	std::vector<std::string> expr;
	bool neg;
	std::vector<std::string> list_str;
	std::vector<long long> list_num;
	void clear();
};

struct where3
{
	std::vector<std::string> logic_expr;
	void clear();
};