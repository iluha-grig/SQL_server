#include <iostream>
#include <memory>
#include <exception>
#include <stdexcept>
#include <string.h>
#include <cctype>
#include <string>
#include <limits>
#include "Header_analysis.h"
#include "Header_structures.h"
#include "Header_socket.h"

#if defined(max)
#undef max
#endif

void help() // manual
{
	std::cout << "You can use SELECT, INSERT, UPDATE, DELETE, CREATE or DROP sentences." << std::endl;
	std::cout << "SELECT sentence syntax: SELECT <list of fields> FROM <table name> <WHERE-clause>" << std::endl;
	std::cout << "INSERT sentence syntax: INSERT INTO <table name> (<field value> {, <field value> })" << std::endl;
	std::cout << "UPDATE sentence syntax: UPDATE <table name> SET <field name> = <expression> <WHERE-clause>" << std::endl;
	std::cout << "DELETE sentence syntax: DELETE FROM <table name> <WHERE-clause>" << std::endl;
	std::cout << "CREATE sentence syntax: CREATE TABLE <table name> ( <list of fields definitions> )" << std::endl;
	std::cout << "List of fields definitions syntax: <field definition> { , <field definition> }" << std::endl;
	std::cout << "Field definition syntax: <field name> <field type>" << std::endl;
	std::cout << "Field type syntax: TEXT ( <unsigned long long> ) | LONG" << std::endl;
	std::cout << "DROP sentence syntax: DROP TABLE <table name>" << std::endl;
	std::cout << std::endl;
	std::cout << "WHERE-clause syntax: WHERE <field name of TEXT type> [ NOT ] LIKE <string prototype> |" << std::endl;
	std::cout << "WHERE <expression> [ NOT ] IN ( <list of constants> ) |" << std::endl;
	std::cout << "WHERE <logic expression> |" << std::endl;
	std::cout << "WHERE ALL" << std::endl;
	std::cout << std::endl;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	std::unique_ptr<Socket_Win_Client> soc;
reset:
	try
	{
		soc = std::make_unique<Socket_Win_Client>();
	}
	catch (const std::system_error & e)
	{
		std::cerr << "Error occurred while connecting to network. Error message:" << std::endl;
		std::cerr << e.what() << '.' << std::endl;
		std::cerr << "Try to connect after program restart." << std::endl;
		exit(0);
	}
	soc->set_server_addr(AF_INET, 8000, "192.168.0.109");
	try
	{
		soc->connect_to_server();
	}
	catch (const std::system_error & e)
	{
		std::cerr << "Error occurred while connecting to server. Error message:" << std::endl;
		std::cerr << e.what() << '.' << std::endl;
		std::cerr << "Make sure that SQL server is available now and try to reconnect after program restart." << std::endl;
		exit(0);
	}

	std::cout << "You are using SQL client program." << std::endl;
	std::cout << "Here you can make requests to SQL server and work with databases." << std::endl;
	std::cout << "Print \"exit\" to quit the program. Print \"help\" to see manual." << std::endl;
	std::string req;
	while (true)
	{
		if (std::cin.eof() || !std::cin.good())
		{
			exit(0);
		}
		std::getline(std::cin, req);
		if (req == "exit")
		{
			exit(0);
		}
		if (req == "help")
		{
			help();
			req.clear();
			continue;
		}
		for (unsigned int i = 0; i < req.size() + 1; ++i)
		{
			std::cin.unget();
		}
		req.clear();
		try
		{
			parser::init();
			parser::S();
			parser::last_check();
		}
		catch (const std::logic_error & e)
		{
			std::cerr << "Error occurred during analysing your request. Error message:" << std::endl;
			std::cerr << e.what() << '.' << std::endl;
			std::cerr << "Try again, please. You also can read manual using \"help\" command." << std::endl;
			if (lexer::cur_lex_type != lex_type_t::END && lexer::c != '\n')
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
			continue;
		}
		if (parser::type == 2 || parser::type == 5 || parser::type == 6)
		{
			parser::type_where = 0;
		}
		try
		{
			soc->send_to_server(&parser::type, sizeof(int));
			soc->send_to_server(&parser::type_where, sizeof(int));
		}
		catch (const std::system_error & e)
		{
			std::cerr << "Error occurred while sending data to server. Error message:" << std::endl;
			std::cerr << e.what() << '.' << std::endl;
			std::cerr << "Try again after automatic restart." << std::endl;
			soc.reset();
			goto reset;
		}
		try
		{
			if (parser::type == 1)
			{
				int len = parser::sel.name.size();
				if (len > 1000000)
				{
					throw std::logic_error("String length cannot be greater than one million characters");
				}
				soc->send_to_server(&len, sizeof(int));
				soc->send_to_server(parser::sel.name.data(), len);
				int vec_len = parser::sel.fields.size();
				soc->send_to_server(&vec_len, sizeof(int));
				for (int i = 0; i < vec_len; ++i)
				{
					len = parser::sel.fields[i].size();
					if (len > 1000000)
					{
						throw std::logic_error("String length cannot be greater than one million characters");
					}
					soc->send_to_server(&len, sizeof(int));
					soc->send_to_server(parser::sel.fields[i].data(), len);
				}
			}
			else if (parser::type == 2)
			{
				int len = parser::ins.name.size();
				if (len > 1000000)
				{
					throw std::logic_error("String length cannot be greater than one million characters");
				}
				soc->send_to_server(&len, sizeof(int));
				soc->send_to_server(parser::ins.name.data(), len);
				int vec_len = parser::ins.fields_str.size();
				soc->send_to_server(&vec_len, sizeof(int));
				for (int i = 0; i < vec_len; ++i)
				{
					len = parser::ins.fields_str[i].size();
					if (len > 1000000)
					{
						throw std::logic_error("String length cannot be greater than one million characters");
					}
					soc->send_to_server(&len, sizeof(int));
					soc->send_to_server(parser::ins.fields_str[i].data(), len);
				}
				vec_len = parser::ins.fields_num.size();
				soc->send_to_server(&vec_len, sizeof(int));
				for (int i = 0; i < vec_len; ++i)
				{
					soc->send_to_server(&parser::ins.fields_num[i], sizeof(long long));
				}
				vec_len = parser::ins.flags.size();
				soc->send_to_server(&vec_len, sizeof(int));
				for (int i = 0; i < vec_len; ++i)
				{
					soc->send_to_server(&parser::ins.flags[i], sizeof(int));
				}
			}
			else if (parser::type == 3)
			{
				int len = parser::upd.name.size();
				if (len > 1000000)
				{
					throw std::logic_error("String length cannot be greater than one million characters");
				}
				soc->send_to_server(&len, sizeof(int));
				soc->send_to_server(parser::upd.name.data(), len);
				len = parser::upd.field.size();
				if (len > 1000000)
				{
					throw std::logic_error("String length cannot be greater than one million characters");
				}
				soc->send_to_server(&len, sizeof(int));
				soc->send_to_server(parser::upd.field.data(), len);
				int vec_len = parser::upd.expr.size();
				soc->send_to_server(&vec_len, sizeof(int));
				for (int i = 0; i < vec_len; ++i)
				{
					len = parser::upd.expr[i].size();
					if (len > 1000000)
					{
						throw std::logic_error("String length cannot be greater than one million characters");
					}
					soc->send_to_server(&len, sizeof(int));
					soc->send_to_server(parser::upd.expr[i].data(), len);
				}
			}
			else if (parser::type == 4)
			{
				int len = parser::de.name.size();
				if (len > 1000000)
				{
					throw std::logic_error("String length cannot be greater than one million characters");
				}
				soc->send_to_server(&len, sizeof(int));
				soc->send_to_server(parser::de.name.data(), len);
			}
			else if (parser::type == 5)
			{
				int len = parser::cre.name.size();
				if (len > 1000000)
				{
					throw std::logic_error("String length cannot be greater than one million characters");
				}
				soc->send_to_server(&len, sizeof(int));
				soc->send_to_server(parser::cre.name.data(), len);
				int vec_len = parser::cre.fields_def.size();
				soc->send_to_server(&vec_len, sizeof(int));
				for (int i = 0; i < vec_len; ++i)
				{
					len = parser::cre.fields_def[i].field.size();
					if (len > 1000000)
					{
						throw std::logic_error("String length cannot be greater than one million characters");
					}
					soc->send_to_server(&len, sizeof(int));
					soc->send_to_server(parser::cre.fields_def[i].field.data(), len);
					soc->send_to_server(&parser::cre.fields_def[i].size, sizeof(long long));
				}
			}
			else
			{
				int len = parser::dro.name.size();
				if (len > 1000000)
				{
					throw std::logic_error("String length cannot be greater than one million characters");
				}
				soc->send_to_server(&len, sizeof(int));
				soc->send_to_server(parser::dro.name.data(), len);
			}
			if (parser::type_where == 1)
			{
				int len = parser::wh1.field.size();
				if (len > 1000000)
				{
					throw std::logic_error("String length cannot be greater than one million characters");
				}
				soc->send_to_server(&len, sizeof(int));
				soc->send_to_server(parser::wh1.field.data(), len);
				soc->send_to_server(&parser::wh1.neg, sizeof(bool));
				len = parser::wh1.str.size();
				if (len > 1000000)
				{
					throw std::logic_error("String length cannot be greater than one million characters");
				}
				soc->send_to_server(&len, sizeof(int));
				soc->send_to_server(parser::wh1.str.data(), len);
			}
			else if (parser::type_where == 2)
			{
				int vec_len = parser::wh2.expr.size();
				soc->send_to_server(&vec_len, sizeof(int));
				int len;
				for (int i = 0; i < vec_len; ++i)
				{
					len = parser::wh2.expr[i].size();
					if (len > 1000000)
					{
						throw std::logic_error("String length cannot be greater than one million characters");
					}
					soc->send_to_server(&len, sizeof(int));
					soc->send_to_server(parser::wh2.expr[i].data(), len);
				}
				soc->send_to_server(&parser::wh2.neg, sizeof(bool));
				vec_len = parser::wh2.list_str.size();
				soc->send_to_server(&vec_len, sizeof(int));
				for (int i = 0; i < vec_len; ++i)
				{
					len = parser::wh2.list_str[i].size();
					if (len > 1000000)
					{
						throw std::logic_error("String length cannot be greater than one million characters");
					}
					soc->send_to_server(&len, sizeof(int));
					soc->send_to_server(parser::wh2.list_str[i].data(), len);
				}
				vec_len = parser::wh2.list_num.size();
				soc->send_to_server(&vec_len, sizeof(int));
				for (int i = 0; i < vec_len; ++i)
				{
					soc->send_to_server(&parser::wh2.list_num[i], sizeof(long long));
				}
			}
			else if (parser::type_where == 3)
			{
				int vec_len = parser::wh3.logic_expr.size();
				soc->send_to_server(&vec_len, sizeof(int));
				int len;
				for (int i = 0; i < vec_len; ++i)
				{
					len = parser::wh3.logic_expr[i].size();
					if (len > 1000000)
					{
						throw std::logic_error("String length cannot be greater than one million characters");
					}
					soc->send_to_server(&len, sizeof(int));
					soc->send_to_server(parser::wh3.logic_expr[i].data(), len);
				}
			}
			else if (parser::type_where == 4)
			{
				// parser::type_where == 4
				// pass
			}
		}
		catch (const std::system_error & e)
		{
			std::cerr << "Error occurred while sending data to server. Error message:" << std::endl;
			std::cerr << e.what() << '.' << std::endl;
			std::cerr << "Try again after automatic restart." << std::endl;
			soc.reset();
			goto reset;
		}
		catch (const std::logic_error & e)
		{
			std::cerr << "Error occurred while sending data to server. Error message:" << std::endl;
			std::cerr << e.what() << '.' << std::endl;
			std::cerr << "Try again after automatic restart." << std::endl;
			soc.reset();
			goto reset;
		}
		try
		{
			int err;
			soc->recv_from_server(&err, sizeof(int), MSG_WAITALL);
			if (err == -1)
			{
				int len;
				char buf[1000];
				memset(buf, 0, 1000);
				soc->recv_from_server(&len, sizeof(int), MSG_WAITALL);
				soc->recv_from_server(buf, len, MSG_WAITALL);
				std::cerr << "Server send error message:" << std::endl;
				std::cerr << buf << '.' << std::endl;
			}
			else
			{
				if (parser::type == 5)
				{
					std::cout << "Table was created successfully." << std::endl;
				}
				else if (parser::type == 6)
				{
					std::cout << "Table was deleted successfully." << std::endl;
				}
				else
				{
					std::vector<long long> sizes;
					std::string str_tmp;
					int len, vec_len1, vec_len2, vec_len3;
					long long tmp_long;
					char buf[1000001];
					memset(buf, 0, 1000001);
					soc->recv_from_server(&vec_len3, sizeof(int), MSG_WAITALL);
					for (int i = 0; i < vec_len3; ++i)
					{
						soc->recv_from_server(&tmp_long, sizeof(long long), MSG_WAITALL);
						sizes.push_back(tmp_long);
					}
					soc->recv_from_server(&vec_len1, sizeof(int), MSG_WAITALL);
					for (int i = 0; i < vec_len1; ++i)
					{
						soc->recv_from_server(&vec_len2, sizeof(int), MSG_WAITALL);
						for (int j = 0; j < vec_len2; ++j)
						{
							if (i == 0)
							{
								soc->recv_from_server(&len, sizeof(int), MSG_WAITALL);
								soc->recv_from_server(buf, len, MSG_WAITALL);
								std::cout << buf << "      ";
							}
							else
							{
								soc->recv_from_server(&len, sizeof(int), MSG_WAITALL);
								soc->recv_from_server(buf, len, MSG_WAITALL);
								str_tmp.clear();
								str_tmp.append(buf);
								if (sizes[j] == -1)
								{
									str_tmp.append(20 - str_tmp.size(), ' ');
								}
								else
								{
									str_tmp.append(static_cast<unsigned long long>(sizes[j]) - str_tmp.size(), ' ');
								}
								std::cout << str_tmp << ' ';
							}
							memset(buf, 0, 1000001);
						}
						std::cout << std::endl;
					}
				}
			}
		}
		catch (const std::system_error & e)
		{
			std::cerr << "Error occurred while receiving data from server. Error message:" << std::endl;
			std::cerr << e.what() << '.' << std::endl;
			if (strcmp(e.what(), "Error on server occurred. Try to repeat your request later") == 0)
			{
				exit(0);
			}
			std::cerr << "Try again after automatic restart." << std::endl;
			soc.reset();
			goto reset;
		}
	}
	return 0;
}