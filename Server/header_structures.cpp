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

void select_t::clear()
{
	name.clear();
	fields.clear();
}

void insert::clear()
{
	name.clear();
	fields_str.clear();
	fields_num.clear();
	flags.clear();
}

void update::clear()
{
	name.clear();
	field.clear();
	expr.clear();
}

void del::clear()
{
	name.clear();
}

void create::clear()
{
	name.clear();
	fields_def.clear();
}

void drop::clear()
{
	name.clear();
}

bool where4::choose(std::unordered_map<std::string, long long>&, std::unordered_map<std::string, std::string>&) const
{
	return true;
}

void where1::clear()
{
	field.clear();
	neg = false;
	str.clear();
}

bool where1::choose(std::unordered_map<std::string, long long>& head_map, std::unordered_map<std::string, std::string>& map) const
{
	if (map.find(field) == map.end())
	{
		throw std::logic_error("No such field");
	}
	if (head_map[field] < 0)
	{
		throw std::logic_error("Type mismatch. \"" + field + "\" should be TEXT field");
	}
	if (neg)
	{
		return !like(map[field].cbegin(), str.cbegin() + 1, map[field].cend(), str.cend() - 1);
	}
	else
	{
		return like(map[field].cbegin(), str.cbegin() + 1, map[field].cend(), str.cend() - 1);
	}
}

void where1::check_str()
{
	init();
	S();
	if (cur_lex_type != lex_type_t::END)
	{
		throw std::logic_error("Incorrect syntax of string in LIKE clause");
	}
}

void where1::lexer_init()
{
	ss << str;
	ss.get();
	c = ss.get();
}

void where1::next()
{
	enum state_t { H, CHAR, PER, UNDER, OPEN, CLOSE, XOR, DEF, OK };
	state_t state = H;
	while (state != OK)
	{
		switch (state)
		{
		case H:

			if (c == '%')
			{
				state = PER;
			}
			else if (c == '_')
			{
				state = UNDER;
			}
			else if (c == '[')
			{
				state = OPEN;
			}
			else if (c == ']')
			{
				state = CLOSE;
			}
			else if (c == '^')
			{
				state = XOR;
			}
			else if (c == '-')
			{
				state = DEF;
			}
			else if (c == '\'')
			{
				cur_lex_type = lex_type_t::END;
				state = OK;
			}
			else
			{
				state = CHAR;
			}
			break;

		case CHAR:

			cur_lex_type = lex_type_t::CHAR;
			state = OK;
			break;

		case PER:

			cur_lex_type = lex_type_t::PER;
			state = OK;
			break;

		case UNDER:

			cur_lex_type = lex_type_t::UNDER;
			state = OK;
			break;

		case OPEN:

			cur_lex_type = lex_type_t::OPEN;
			state = OK;
			break;

		case CLOSE:

			cur_lex_type = lex_type_t::CLOSE;
			state = OK;
			break;

		case XOR:

			cur_lex_type = lex_type_t::XOR;
			state = OK;
			break;

		case DEF:

			cur_lex_type = lex_type_t::DEF;
			state = OK;
			break;

		case OK:

			break;
		}
		
		if (state != OK)
		{
			cur_lex_text = c;
			c = ss.get();
		}
	}
}

void where1::init()
{
	lexer_init();
	next();
}

void where1::S()
{
	if (cur_lex_type == lex_type_t::CHAR)
	{
		next();
		S1();
	}
	else if (cur_lex_type == lex_type_t::PER)
	{
		next();
		S1();
	}
	else if (cur_lex_type == lex_type_t::UNDER)
	{
		next();
		S1();
	}
	else if (cur_lex_type == lex_type_t::OPEN)
	{
		next();
		A();
		if (cur_lex_type != lex_type_t::CLOSE)
		{
			throw std::logic_error("Incorrect syntax of string in LIKE clause");
		}
		next();
		S1();
	}
	else if (cur_lex_type == lex_type_t::XOR)
	{
		next();
		S1();
	}
	else if (cur_lex_type == lex_type_t::DEF)
	{
		next();
		S1();
	}
	else if (cur_lex_type == lex_type_t::CLOSE)
	{
		next();
		S1();
	}
	else
	{
		throw std::logic_error("Incorrect syntax of string in LIKE clause");
	}
}

void where1::S1()
{
	if (cur_lex_type == lex_type_t::CHAR)
	{
		next();
		S1();
	}
	else if (cur_lex_type == lex_type_t::PER)
	{
		next();
		S1();
	}
	else if (cur_lex_type == lex_type_t::UNDER)
	{
		next();
		S1();
	}
	else if (cur_lex_type == lex_type_t::OPEN)
	{
		next();
		A();
		if (cur_lex_type != lex_type_t::CLOSE)
		{
			throw std::logic_error("Incorrect syntax of string in LIKE clause");
		}
		next();
		S1();
	}
	else if (cur_lex_type == lex_type_t::XOR)
	{
		next();
		S1();
	}
	else if (cur_lex_type == lex_type_t::DEF)
	{
		next();
		S1();
	}
	else if (cur_lex_type == lex_type_t::CLOSE)
	{
		next();
		S1();
	}
	else if (cur_lex_type == lex_type_t::END)
	{
		// pass
	}
	else
	{
		throw std::logic_error("Incorrect syntax of string in LIKE clause");
	}
}

void where1::A()
{
	if (cur_lex_type == lex_type_t::XOR)
	{
		next();
		B1();
		C();
	}
	else
	{
		B1();
		C();
	}
}

void where1::B1()
{
	if (cur_lex_type == lex_type_t::CHAR)
	{
		c1 = cur_lex_text;
		next();
	}
	else if (cur_lex_type == lex_type_t::PER)
	{
		c1 = cur_lex_text;
		next();
	}
	else if (cur_lex_type == lex_type_t::UNDER)
	{
		c1 = cur_lex_text;
		next();
	}
	else if (cur_lex_type == lex_type_t::OPEN)
	{
		c1 = cur_lex_text;
		next();
	}
	else
	{
		throw std::logic_error("Incorrect syntax of string in LIKE clause");
	}
}

void where1::C()
{
	if (cur_lex_type == lex_type_t::DEF)
	{
		next();
		B2();
		if (c1 > c2)
		{
			throw std::logic_error("Incorrect syntax of string in LIKE clause.\nFirst character in range should be less or equal than second one");
		}
	}
	else
	{
		D();
	}
}

void where1::B2()
{
	if (cur_lex_type == lex_type_t::CHAR)
	{
		c2 = cur_lex_text;
		next();
	}
	else if (cur_lex_type == lex_type_t::PER)
	{
		c2 = cur_lex_text;
		next();
	}
	else if (cur_lex_type == lex_type_t::UNDER)
	{
		c2 = cur_lex_text;
		next();
	}
	else if (cur_lex_type == lex_type_t::OPEN)
	{
		c2 = cur_lex_text;
		next();
	}
	else
	{
		throw std::logic_error("Incorrect syntax of string in LIKE clause");
	}
}

void where1::D()
{
	if (cur_lex_type == lex_type_t::CLOSE)
	{
		// pass
	}
	else
	{
		B();
		D();
	}
}

void where1::B()
{
	if (cur_lex_type == lex_type_t::CHAR)
	{
		next();
	}
	else if (cur_lex_type == lex_type_t::PER)
	{
		next();
	}
	else if (cur_lex_type == lex_type_t::UNDER)
	{
		next();
	}
	else if (cur_lex_type == lex_type_t::OPEN)
	{
		next();
	}
	else
	{
		throw std::logic_error("Incorrect syntax of string in LIKE clause");
	}
}

bool where1::like(std::string::const_iterator iter1, std::string::const_iterator iter2, std::string::const_iterator iter1_end, std::string::const_iterator iter2_end) const
{
	while (iter2 != iter2_end && iter1 != iter1_end)
	{
		if (*iter2 == '_')
		{
			++iter1;
			++iter2;
		}
		else if (*iter2 == '[')
		{
			++iter2;
			if (*iter2 == '^')
			{
				++iter2;
				if (*(iter2 + 1) == '-')
				{
					char c1 = *iter2;
					iter2 += 2;
					if (*iter1 >= c1 && *iter1 <= *iter2)
					{
						return false;
					}
					++iter1;
					iter2 += 2;
				}
				else
				{
					while (*iter2 != ']')
					{
						if (*iter1 == *iter2)
						{
							return false;
						}
						++iter2;
					}
					++iter1;
					++iter2;
				}
			}
			else
			{
				if (*(iter2 + 1) == '-')
				{
					char c1 = *iter2;
					iter2 += 2;
					if (*iter1 < c1 || *iter1 > *iter2)
					{
						return false;
					}
					++iter1;
					iter2 += 2;
				}
				else
				{
					bool flag = false;
					while (*iter2 != ']')
					{
						if (*iter1 == *iter2)
						{
							flag = true;
						}
						++iter2;
					}
					if (!flag)
					{
						return false;
					}
					++iter1;
					++iter2;
				}
			}
		}
		else if (*iter2 == '%')
		{
			++iter2;
			if (iter2 == iter2_end)
			{
				return true;
			}
			while (iter1 != iter1_end)
			{
				if (like(iter1, iter2, iter1_end, iter2_end))
				{
					return true;
				}
				++iter1;
			}
			return false;
		}
		else
		{
			if (*iter1 != *iter2)
			{
				return false;
			}
			++iter1;
			++iter2;
		}
	}
	if (iter1 == iter1_end && iter2 == iter2_end)
	{
		return true;
	}
	else if (iter2 != iter2_end)
	{
		if (*iter2 == '%')
		{
			return like(iter1, ++iter2, iter1_end, iter2_end);
		}
		else
		{
			return false;
		}
	}
	return false;
}

void where2::clear()
{
	expr.clear();
	neg = false;
	list_str.clear();
	list_num.clear();
}

bool where2::choose(std::unordered_map<std::string, long long>& head_map, std::unordered_map<std::string, std::string>& map) const
{
	if (list_str.size() > 0)
	{
		if (expr.size() > 1)
		{
			throw std::logic_error("Wrong argument");
		}
		if (expr.back().front() == '\'')
		{
			bool flag = (std::find(list_str.begin(), list_str.end(), expr.back()) == list_str.end());
			if (neg)
			{
				return flag;
			}
			else
			{
				return !flag;
			}
		}
		else if (isalpha(expr[0][0]) || expr[0][0] == '_')
		{
			if (map.find(expr.back()) == map.end())
			{
				throw std::logic_error("No such field");
			}
			if (head_map[expr.back()] < 0)
			{
				throw std::logic_error("Wrong argument");
			}
			std::string str("'" + map[expr.back()] + "'");
			bool flag = (std::find(list_str.begin(), list_str.end(), str) == list_str.end());
			if (neg)
			{
				return flag;
			}
			else
			{
				return !flag;
			}
		}
		else
		{
			throw std::logic_error("Wrong argument");
		}
	}
	else
	{
		if (expr.size() == 1 && expr.front().front() == '\'')
		{
			throw std::logic_error("Wrong argument");
		}
		std::stack<long long> stack;
		for (const std::string& item : expr)
		{
			if (isdigit(item[0]))
			{
				stack.push(std::strtoll(item.data(), nullptr, 10));
				if (errno == ERANGE)
				{
					errno = 0;
					throw std::logic_error("Too big number " + item);
				}
			}
			else if ((item[0] == '-' && item.size() > 1) || (item[0] == '+' && item.size() > 1))
			{
				stack.push(std::strtoll(item.data(), nullptr, 10));
				if (errno == ERANGE)
				{
					errno = 0;
					throw std::logic_error("Too big number " + item);
				}
			}
			else if (isalpha(item[0]) || item[0] == '_')
			{
				if (map.find(item) == map.end())
				{
					throw std::logic_error("No such field");
				}
				if (head_map[item] > 0)
				{
					throw std::logic_error("Wrong argument");
				}
				stack.push(std::strtoll(map[item].data(), nullptr, 10));
			}
			else if (item == "+")
			{
				long long op2 = stack.top();
				stack.pop();
				long long op1 = stack.top();
				stack.pop();
				stack.push(op1 + op2);
			}
			else if (item == "-")
			{
				long long op2 = stack.top();
				stack.pop();
				long long op1 = stack.top();
				stack.pop();
				stack.push(op1 - op2);
			}
			else if (item == "*")
			{
				long long op2 = stack.top();
				stack.pop();
				long long op1 = stack.top();
				stack.pop();
				stack.push(op1 * op2);
			}
			else if (item == "/")
			{
				long long op2 = stack.top();
				stack.pop();
				long long op1 = stack.top();
				stack.pop();
				stack.push(op1 / op2);
			}
			else
			{
				long long op2 = stack.top();
				stack.pop();
				long long op1 = stack.top();
				stack.pop();
				stack.push(op1 % op2);
			}
		}
		bool flag = (std::find(list_num.begin(), list_num.end(), stack.top()) == list_num.end());
		if (neg)
		{
			return flag;
		}
		else
		{
			return !flag;
		}
	}
}

void where3::clear()
{
	logic_expr.clear();
}

bool where3::choose(std::unordered_map<std::string, long long>& head_map, std::unordered_map<std::string, std::string>& map) const
{
	std::stack<long long> stack_num;
	std::stack<std::string> stack_str;
	bool flag = true, type;
	for (const std::string& item : logic_expr)
	{
		if (flag && (item != "AND") && (item != "OR") && (item != "NOT"))
		{
			if (isalpha(item[0]) || item[0] == '_')
			{
				if (map.find(item) == map.end())
				{
					throw std::logic_error("No such field");
				}
				if (head_map[item] < 0)
				{
					type = true; //true means num
					stack_num.push(std::strtoll(map[item].data(), nullptr, 10));
				}
				else
				{
					type = false; //false means str
					stack_str.push("'" + map[item] + "'");
				}
			}
			else
			{
				if (isdigit(item[0]) || (item[0] == '-' && item.size() > 1) || (item[0] == '+' && item.size() > 1))
				{
					type = true;
					stack_num.push(std::strtoll(item.data(), nullptr, 10));
					if (errno == ERANGE)
					{
						errno = 0;
						throw std::logic_error("Too big number " + item);
					}
				}
				else
				{
					type = false;
					stack_str.push(item);
				}
			}
			flag = false;
		}
		else
		{
			if (stack_str.size() > 2)
			{
				throw std::logic_error("Wrong argument");
			}
			if (((isalpha(item[0])) && (item != "AND") && (item != "OR") && (item != "NOT")) || (item[0] == '_'))
			{
				if (map.find(item) == map.end())
				{
					throw std::logic_error("No such field");
				}
				if (type)
				{
					if (head_map[item] > 0)
					{
						throw std::logic_error("Wrong argument");
					}
					stack_num.push(std::strtoll(map[item].data(), nullptr, 10));
				}
				else
				{
					if (head_map[item] < 0)
					{
						throw std::logic_error("Wrong argument");
					}
					stack_str.push("'" + map[item] + "'");
				}
			}
			else if (isdigit(item[0]) || (item[0] == '-' && item.size() > 1) || (item[0] == '+' && item.size() > 1) || item[0] == '\'')
			{
				if (type)
				{
					if (item.front() == '\'')
					{
						throw std::logic_error("Wrong argument");
					}
					stack_num.push(std::strtoll(item.data(), nullptr, 10));
					if (errno == ERANGE)
					{
						errno = 0;
						throw std::logic_error("Too big number " + item);
					}
				}
				else
				{
					if (item.front() != '\'')
					{
						throw std::logic_error("Wrong argument");
					}
					stack_str.push(item);
				}
			}
			else if (item == "+")
			{
				if (stack_str.size() != 0)
				{
					throw std::logic_error("Wrong argument");
				}
				long long op2 = stack_num.top();
				stack_num.pop();
				long long op1 = stack_num.top();
				stack_num.pop();
				stack_num.push(op1 + op2);
			}
			else if (item == "-")
			{
				if (stack_str.size() != 0)
				{
					throw std::logic_error("Wrong argument");
				}
				long long op2 = stack_num.top();
				stack_num.pop();
				long long op1 = stack_num.top();
				stack_num.pop();
				stack_num.push(op1 - op2);
			}
			else if (item == "*")
			{
				if (stack_str.size() != 0)
				{
					throw std::logic_error("Wrong argument");
				}
				long long op2 = stack_num.top();
				stack_num.pop();
				long long op1 = stack_num.top();
				stack_num.pop();
				stack_num.push(op1 * op2);
			}
			else if (item == "/")
			{
				if (stack_str.size() != 0)
				{
					throw std::logic_error("Wrong argument");
				}
				long long op2 = stack_num.top();
				stack_num.pop();
				long long op1 = stack_num.top();
				stack_num.pop();
				stack_num.push(op1 / op2);
			}
			else if (item == "%")
			{
				if (stack_str.size() != 0)
				{
					throw std::logic_error("Wrong argument");
				}
				long long op2 = stack_num.top();
				stack_num.pop();
				long long op1 = stack_num.top();
				stack_num.pop();
				stack_num.push(op1 % op2);
			}
			else if (item == "AND")
			{
				if (stack_str.size() != 0)
				{
					throw std::logic_error("Wrong argument");
				}
				long long op2 = stack_num.top();
				stack_num.pop();
				long long op1 = stack_num.top();
				stack_num.pop();
				stack_num.push(op1 && op2);
			}
			else if (item == "OR")
			{
				if (stack_str.size() != 0)
				{
					throw std::logic_error("Wrong argument");
				}
				long long op2 = stack_num.top();
				stack_num.pop();
				long long op1 = stack_num.top();
				stack_num.pop();
				stack_num.push(op1 || op2);
			}
			else if (item == "NOT")
			{
				if (stack_str.size() != 0)
				{
					throw std::logic_error("Wrong argument");
				}
				long long op = stack_num.top();
				stack_num.pop();
				stack_num.push(!op);
			}
			else if (item == "=")
			{
				if (stack_str.size() == 2)
				{
					std::string str2 = stack_str.top();
					stack_str.pop();
					std::string str1 = stack_str.top();
					stack_str.pop();
					stack_num.push(str1 == str2);
				}
				else
				{
					if (stack_str.size() != 0)
					{
						throw std::logic_error("Wrong argument");
					}
					long long op2 = stack_num.top();
					stack_num.pop();
					long long op1 = stack_num.top();
					stack_num.pop();
					stack_num.push(op1 == op2);
				}
				flag = true;
			}
			else if (item == ">")
			{
				if (stack_str.size() == 2)
				{
					std::string str2 = stack_str.top();
					stack_str.pop();
					std::string str1 = stack_str.top();
					stack_str.pop();
					stack_num.push(str1 > str2);
				}
				else
				{
					if (stack_str.size() != 0)
					{
						throw std::logic_error("Wrong argument");
					}
					long long op2 = stack_num.top();
					stack_num.pop();
					long long op1 = stack_num.top();
					stack_num.pop();
					stack_num.push(op1 > op2);
				}
				flag = true;
			}
			else if (item == "<")
			{
				if (stack_str.size() == 2)
				{
					std::string str2 = stack_str.top();
					stack_str.pop();
					std::string str1 = stack_str.top();
					stack_str.pop();
					stack_num.push(str1 < str2);
				}
				else
				{
					if (stack_str.size() != 0)
					{
						throw std::logic_error("Wrong argument");
					}
					long long op2 = stack_num.top();
					stack_num.pop();
					long long op1 = stack_num.top();
					stack_num.pop();
					stack_num.push(op1 < op2);
				}
				flag = true;
			}
			else if (item == ">=")
			{
				if (stack_str.size() == 2)
				{
					std::string str2 = stack_str.top();
					stack_str.pop();
					std::string str1 = stack_str.top();
					stack_str.pop();
					stack_num.push(str1 >= str2);
				}
				else
				{
					if (stack_str.size() != 0)
					{
						throw std::logic_error("Wrong argument");
					}
					long long op2 = stack_num.top();
					stack_num.pop();
					long long op1 = stack_num.top();
					stack_num.pop();
					stack_num.push(op1 >= op2);
				}
				flag = true;
			}
			else if (item == "<=")
			{
				if (stack_str.size() == 2)
				{
					std::string str2 = stack_str.top();
					stack_str.pop();
					std::string str1 = stack_str.top();
					stack_str.pop();
					stack_num.push(str1 <= str2);
				}
				else
				{
					if (stack_str.size() != 0)
					{
						throw std::logic_error("Wrong argument");
					}
					long long op2 = stack_num.top();
					stack_num.pop();
					long long op1 = stack_num.top();
					stack_num.pop();
					stack_num.push(op1 <= op2);
				}
				flag = true;
			}
			else
			{
				if (stack_str.size() == 2)
				{
					std::string str2 = stack_str.top();
					stack_str.pop();
					std::string str1 = stack_str.top();
					stack_str.pop();
					stack_num.push(str1 != str2);
				}
				else
				{
					if (stack_str.size() != 0)
					{
						throw std::logic_error("Wrong argument");
					}
					long long op2 = stack_num.top();
					stack_num.pop();
					long long op1 = stack_num.top();
					stack_num.pop();
					stack_num.push(op1 != op2);
				}
				flag = true;
			}
		}
	}
	if (stack_num.size() != 1 || stack_str.size() != 0)
	{
		throw std::logic_error("Error in logic expression of WHERE claus occured");
	}
	return stack_num.top();
}

void answer::clear()
{
	ans.clear();
}
