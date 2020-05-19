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
#include "server.h"
#include "header_structures.h"

Table::Table(const std::string& str) : filename(str)
{
	fin.open(filename, std::ios_base::in);
	if (!fin.is_open())
	{
		throw std::ios_base::failure("File not exists or another file error occurred");
	}
	fout.open(filename, std::ios_base::in | std::ios_base::ate);
	if (!fout.is_open())
	{
		throw std::ios_base::failure("File not exists or another file error occurred");
	}
	int len;
	std::string str_tmp;
	std::stringstream ss;
	fin >> len;
	fin.get();
	for (int i = 0; i < len; ++i)
	{
		std::getline(fin, str_tmp);
		ss << str_tmp;
		ss >> str_tmp;
		head.push_back(std::move(str_tmp));
		ss >> str_tmp;
		head_map[head.back()] = std::strtoll(str_tmp.data(), nullptr, 10);
		ss = std::stringstream();
	}
	start = fin.tellg();
}

Table::~Table()
{
	fout.flush();
	fin.close();
	fout.close();
}

bool Table::eof_in() noexcept
{
	fin.get();
	bool res;
	if (fin.eof())
	{
		res = true;
		fin.clear();
	}
	else
	{
		res = false;
		fin.unget();
	}
	return res;
}

/*
may be unsafe

bool Table::eof_out() const noexcept
{
	return fout.eof();
}
*/

void Table::start_pos_in()
{
	fin.seekg(start);
}

void Table::end_pos_out()
{
	fout.seekp(0, std::ios_base::end);
}

std::unordered_map<std::string, std::string> Table::read_first_map()
{
	std::string str;
	std::unordered_map<std::string, std::string> map;
	fin.seekg(start);
	std::getline(fin, str);
	std::stringstream ss(str);
	for (auto iter = head.begin(); iter != head.end(); ++iter)
	{
		ss >> str;
		map[*iter] = str;
	}
	return std::move(map);
}

std::vector<std::string> Table::read_first_vector()
{
	std::string str;
	std::vector<std::string> vec;
	fin.seekg(start);
	std::getline(fin, str);
	std::stringstream ss(str);
	for (unsigned long long i = 0; i < head.size(); ++i)
	{
		ss >> str;
		vec.push_back(str);
	}
	return std::move(vec);
}

std::unordered_map<std::string, std::string> Table::read_next_map()
{
	std::string str;
	std::unordered_map<std::string, std::string> map;
	std::getline(fin, str);
	std::stringstream ss(str);
	for (auto iter = head.begin(); iter != head.end(); ++iter)
	{
		ss >> str;
		map[*iter] = str;
	}
	return std::move(map);
}

std::vector<std::string> Table::read_next_vector()
{
	std::string str;
	std::vector<std::string> vec;
	std::getline(fin, str);
	std::stringstream ss(str);
	for (unsigned long long i = 0; i < head.size(); ++i)
	{
		ss >> str;
		vec.push_back(str);
	}
	return std::move(vec);
}

std::ostream& Table::put_num(long long num)
{
	std::string str(std::to_string(num));
	str.append(20 - str.size(), ' ');
	return fout << str;
}

std::ostream& Table::put_str(std::string str, long long len)
{
	str.append(len - str.size(), ' ');
	return fout << str;
}

void Table::seek_back_in()
{
	for (auto iter = head.rbegin(); iter != head.rend(); ++iter)
	{
		if (head_map[*iter] < 0)
		{
			fin.seekg(-21, std::ios_base::cur);
		}
		else
		{
			fin.seekg(head_map[*iter] * -1 - 1, std::ios_base::cur);
		}
	}
}

void Table::set_on_field_out(const std::string& field)
{
	auto iter = head.begin();
	while (*iter != field)
	{
		if (head_map[*iter] < 0)
		{
			fout.seekp(21, std::ios_base::cur);
		}
		else
		{
			fout.seekp(head_map[*iter] + 1, std::ios_base::cur);
		}
		++iter;
	}
}

void Table::sync_in_out()
{
	fout.seekp(fin.tellg());
}

void Table::read_discard_line_in()
{
	std::string tmp;
	std::getline(fin, tmp);
}

Select::Select(select_t* sel, where_base* ptr) : Table(sel->name), sel_ptr(sel), wh_ptr(ptr) {}

answer Select::make_select()
{
	for (const std::string& item : sel_ptr->fields)
	{
		if (std::find(head.begin(), head.end(), item) == head.end())
		{
			throw std::logic_error("No such field");
		}
	}
	answer ans;
	std::vector<std::string> vec_tmp;
	std::unordered_map<std::string, std::string> map_tmp;
	if (sel_ptr->fields.size() == 0)
	{
		ans.ans.push_back(head);
	}
	else
	{
		ans.ans.push_back(sel_ptr->fields);
	}
	start_pos_in();
	while (!eof_in())
	{
		map_tmp = read_next_map();
		if (wh_ptr->choose(head_map, map_tmp))
		{
			if (sel_ptr->fields.size() == 0)
			{
				for (const std::string& item : head)
				{
					vec_tmp.push_back(std::move(map_tmp[item]));
				}
			}
			else
			{
				for (const std::string& item : sel_ptr->fields)
				{
					vec_tmp.push_back(std::move(map_tmp[item]));
				}
			}
			ans.ans.push_back(std::move(vec_tmp));
			vec_tmp.clear();
		}
	}
	return std::move(ans);
}

std::vector<long long> Select::get_head()
{
	std::vector<long long> tmp;
	if (sel_ptr->fields.size() == 0)
	{
		for (const std::string& item : head)
		{
			tmp.push_back(head_map[item]);
		}
	}
	else
	{
		for (const std::string& item : sel_ptr->fields)
		{
			tmp.push_back(head_map[item]);
		}
	}
	return std::move(tmp);
}

void Drop::make_drop(const std::string& filename)
{
	std::ifstream fin(filename);
	if (!fin.is_open())
	{
		throw std::logic_error("File not exists or had been removed already");
	}
	fin.close();
	unlink(filename.data());
}

void Create::make_create(create* ptr)
{
	std::ifstream fin(ptr->name);
	if (fin.is_open())
	{
		fin.close();
		throw std::logic_error("File already exists");
	}
	std::unordered_set<std::string> set;
	for (const auto& item : ptr->fields_def)
	{
		if (item.size == 0)
		{
			throw std::logic_error("Field size cannot be zero");
		}
		set.insert(item.field);
	}
	if (set.size() != ptr->fields_def.size())
	{
		throw std::logic_error("Table should have unique fields");
	}
	std::ofstream fout(ptr->name);
	fout << ptr->fields_def.size() << '\n';
	for (const auto& item : ptr->fields_def)
	{
		fout << item.field << ' ' << item.size << '\n';
	}
	fout.close();
}

Insert::Insert(insert* ins) : Table(ins->name), ins_ptr(ins) {}

answer Insert::make_insert()
{
	if (head.size() != ins_ptr->flags.size())
	{
		throw std::logic_error("Wrong number of fields");
	}
	int counter = 0;
	std::vector<std::string>::iterator iter_str_ptr = ins_ptr->fields_str.begin();
	for (const std::string& item : head)
	{
		if ((head_map[item] < 0 && !ins_ptr->flags[counter]) || (head_map[item] > 0 && ins_ptr->flags[counter]))
		{
			throw std::logic_error("Wrong argument. Type mismatch");
		}
		if (!ins_ptr->flags[counter])
		{
			if (iter_str_ptr->size() - 2 > static_cast<unsigned long long>(head_map[item]))
			{
				throw std::logic_error("String \"" + *iter_str_ptr + "\" is longer then text field \"" + item + "\" can hold");
			}
			++iter_str_ptr;
		}
		++counter;
	}
	end_pos_out();
	std::vector<std::string>::iterator iter_str = ins_ptr->fields_str.begin();
	std::vector<long long>::iterator iter_num = ins_ptr->fields_num.begin();
	std::vector<std::string>::iterator iter_head = head.begin();
	for (bool item : ins_ptr->flags)
	{
		if (item)
		{
			put_num(*iter_num) << ' ';
			++iter_num;
		}
		else
		{
			put_str(iter_str->substr(1, iter_str->size() - 2), head_map[*iter_head]) << ' ';
			++iter_str;
		}
		++iter_head;
	}
	fout.seekp(-1, std::ios_base::cur);
	fout.put('\n');
	fout.flush();
	fout.close();
	answer ans;
	ans.ans.push_back(head);
	start_pos_in();
	while (!eof_in())
	{
		ans.ans.push_back(read_next_vector());
	}
	return std::move(ans);
}

std::vector<long long> Insert::get_head()
{
	std::vector<long long> tmp;
	for (const std::string& item : head)
	{
		tmp.push_back(head_map[item]);
	}
	return std::move(tmp);
}

Update::Update(update* upd, where_base* wh) : Table(upd->name), upd_ptr(upd), wh_ptr(wh) {}

answer Update::make_update()
{
	if (std::find(head.begin(), head.end(), upd_ptr->field) == head.end())
	{
		throw std::logic_error("No such field");
	}
	bool type = true;
	if (head_map[upd_ptr->field] > 0)
	{
		if (upd_ptr->expr.size() != 1)
		{
			throw std::logic_error("Wrong expression argument");
		}
		if (isalpha(upd_ptr->expr[0][0]) || upd_ptr->expr[0][0] == '_')
		{
			if (std::find(head.begin(), head.end(), upd_ptr->expr[0]) == head.end())
			{
				throw std::logic_error("No such TEXT field");
			}
			if (head_map[upd_ptr->expr[0]] < 0)
			{
				throw std::logic_error("Type mismatch. Cannot write LONG_NUM field in TEXT field");
			}
			if (head_map[upd_ptr->field] < head_map[upd_ptr->expr[0]])
			{
				throw std::logic_error("Cannot write TEXT field with bigger size into TEXT field with smaller size");
			}
		}
		else if (upd_ptr->expr[0][0] == '\'')
		{
			if (upd_ptr->expr[0].size() - 2 > static_cast<unsigned long long>(head_map[upd_ptr->field]))
			{
				throw std::logic_error("String size is bigger than this TEXT field can hold");
			}
		}
		else
		{
			throw std::logic_error("Wrong argument \"" + upd_ptr->expr[0] + "\"");
		}
		type = false;
	}
	else
	{
		for (const std::string& item : upd_ptr->expr)
		{
			if (isalpha(item[0]) || item[0] == '_')
			{
				if (std::find(head.begin(), head.end(), item) == head.end())
				{
					throw std::logic_error("No such field");
				}
				if (head_map[item] > 0)
				{
					throw std::logic_error("Type mismatch. Cannot use TEXT field \"" + item + "\" in expression which should be written into LONG_NUM field");
				}
			}
			else if (item[0] == '\'')
			{
				throw std::logic_error("Wrong argument. Cannot use string in expression which should be written into LONG_NUM field");
			}
		}
	}
	std::unordered_map<std::string, std::string> tmp_map;
	start_pos_in();
	if (type)
	{
		std::stack<long long> stack_num;
		while (!eof_in())
		{
			tmp_map = read_next_map();
			if (wh_ptr->choose(head_map, tmp_map))
			{
				for (const std::string& item : upd_ptr->expr)
				{
					if (isalpha(item[0]) || item[0] == '_')
					{
						stack_num.push(strtoll(tmp_map[item].data(), nullptr, 10));
					}
					else if (isdigit(item[0]) || (item[0] == '+' && item.size() > 1) || (item[0] == '-' && item.size() > 1))
					{
						stack_num.push(strtoll(item.data(), nullptr, 10));
						if (errno == ERANGE)
						{
							errno = 0;
							throw std::logic_error("Too big number " + item);
						}
					}
					else if (item == "+")
					{
						long long op2 = stack_num.top();
						stack_num.pop();
						long long op1 = stack_num.top();
						stack_num.pop();
						stack_num.push(op1 + op2);
					}
					else if (item == "-")
					{
						long long op2 = stack_num.top();
						stack_num.pop();
						long long op1 = stack_num.top();
						stack_num.pop();
						stack_num.push(op1 - op2);
					}
					else if (item == "*")
					{
						long long op2 = stack_num.top();
						stack_num.pop();
						long long op1 = stack_num.top();
						stack_num.pop();
						stack_num.push(op1 * op2);
					}
					else if (item == "/")
					{
						long long op2 = stack_num.top();
						stack_num.pop();
						long long op1 = stack_num.top();
						stack_num.pop();
						stack_num.push(op1 / op2);
					}
					else
					{
						long long op2 = stack_num.top();
						stack_num.pop();
						long long op1 = stack_num.top();
						stack_num.pop();
						stack_num.push(op1 % op2);
					}
				}
				seek_back_in();
				sync_in_out();
				set_on_field_out(upd_ptr->field);
				put_num(stack_num.top());
				stack_num.pop();
				read_discard_line_in();
			}
		}
	}
	else
	{
		if (upd_ptr->expr[0][0] == '\'')
		{
			std::string expr(upd_ptr->expr[0].substr(1, upd_ptr->expr[0].size() - 2));
			while (!eof_in())
			{
				tmp_map = read_next_map();
				if (wh_ptr->choose(head_map, tmp_map))
				{
					seek_back_in();
					sync_in_out();
					set_on_field_out(upd_ptr->field);
					put_str(expr, head_map[upd_ptr->field]);
					read_discard_line_in();
				}
			}
		}
		else
		{
			while (!eof_in())
			{
				tmp_map = read_next_map();
				if (wh_ptr->choose(head_map, tmp_map))
				{
					seek_back_in();
					sync_in_out();
					set_on_field_out(upd_ptr->field);
					put_str(tmp_map[upd_ptr->expr[0]], head_map[upd_ptr->field]);
					read_discard_line_in();
				}
			}
		}
	}
	fout.flush();
	fout.close();
	answer ans;
	ans.ans.push_back(head);
	start_pos_in();
	while (!eof_in())
	{
		ans.ans.push_back(read_next_vector());
	}
	return std::move(ans);
}

std::vector<long long> Update::get_head()
{
	std::vector<long long> tmp;
	for (const std::string& item : head)
	{
		tmp.push_back(head_map[item]);
	}
	return std::move(tmp);
}

Delete::Delete(del* del, where_base* wh) : Table(del->name), wh_ptr(wh)
{
	tmp_file = std::tmpfile();
	fin.seekg(0, std::ios_base::beg);
	while (fin.tellg() != start)
	{
		std::fputc(fin.get(), tmp_file);
	}
}

Delete::~Delete()
{
	std::fclose(tmp_file);
}

answer Delete::make_delete()
{
	std::unordered_map<std::string, std::string> tmp_map;
	while (!eof_in())
	{
		tmp_map = read_next_map();
		if (wh_ptr->choose(head_map, tmp_map))
		{
			// pass
		}
		else
		{
			seek_back_in();
			int c;
			while ((c = fin.get()) != '\n')
			{
				std::fputc(c, tmp_file);
			}
			std::fputc('\n', tmp_file);
		}
	}
	fin.close();
	fout.close();
	fout.open(filename, std::ios_base::out);
	std::fseek(tmp_file, 0, SEEK_SET);
	int c;
	while ((c = fgetc(tmp_file)) != EOF)
	{
		fout.put(c);
	}
	fout.flush();
	fout.close();
	fin.open(filename, std::ios_base::in);
	start_pos_in();
	answer ans;
	ans.ans.push_back(head);
	while (!eof_in())
	{
		ans.ans.push_back(read_next_vector());
	}
	return std::move(ans);
}

std::vector<long long> Delete::get_head()
{
	std::vector<long long> tmp;
	for (const std::string& item : head)
	{
		tmp.push_back(head_map[item]);
	}
	return std::move(tmp);
}
