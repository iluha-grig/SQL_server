#include <iostream>
#include <string>
#include <stdexcept>
#include <exception>
#include <vector>
#include <cctype>
#include <cerrno>
#include "Header_structures.h"

enum class lex_type_t {
	SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ID, FROM, COMMA, STAR, INTO, OPEN, CLOSE, LONG_NUM, STR,
	SET, EQ, TABLE, TEXT, LONG, WHERE, NOT, LIKE, IN, ALL, PLUS, MINUS, DEL, MOD, OR, AND, GR, LS, NOT_EQ, GR_EQ, LS_EQ, END
};

struct lexema
{
	lex_type_t type;
	std::string text;
};

namespace parser
{
	bool flag_vec;
	int counter;
	std::vector<lexema> vec_look3;
}

namespace lexer
{
	lex_type_t cur_lex_type;
	std::string cur_lex_text;
	int cur_lex_pos;
	int c;

	void init()
	{
		c = std::cin.get();
		while (c == ' ')
		{
			c = std::cin.get();
		}
		if (c == EOF || c == '\n')
		{
			throw std::logic_error("Empty request");
		}
		cur_lex_pos = 1;
	}

	void next()
	{
		if (parser::flag_vec)
		{
			if (static_cast<unsigned int>(parser::counter) < parser::vec_look3.size())
			{
				cur_lex_type = parser::vec_look3[parser::counter].type;
				cur_lex_text = parser::vec_look3[parser::counter].text;
				++parser::counter;
				return;
			}
			else
			{
				parser::flag_vec = false;
			}
		}

		cur_lex_text.clear();
		enum state_t {
			H, IS, II, IU, ID, IC, IF, IT, IL, IW, IN, IA, IO, IDENT, C, S, O, CL, LN, STR, E, P, M, D, MO, G_GE, L_LE, NE, OK,
			ISE, IIN, IUP, IDE, IDR, ICR, IFR, ITA, ITE, ILO, ILI, IWH, INO, IAL, IAN, IOR, STR_END, STR_MAY_END, GE, LE, NE_END,
			ISEL, ISET, IINS, IINT, IUPD, IDEL, IDRO, ICRE, IFRO, ITAB, ITEX, ILON, ILIK, IWHE, INOT, IALL, IAND,
			ISELE, IINSE, IINTO, IUPDA, IDELE, IDROP, ICREA, IFROM, ITABL, ITEXT, ILONG, ILIKE, IWHER,
			ISELEC, IINSER, IUPDAT, IDELET, ICREAT, ITABLE, IWHERE,
			ISELECT, IINSERT, IUPDATE, IDELETE, ICREATE
		};

		state_t state = H;

		while (state != OK)
		{
			switch (state)
			{
			case H:

				if (c == 'S')
				{
					state = IS;
				}
				else if (c == 'I')
				{
					state = II;
				}
				else if (c == 'U')
				{
					state = IU;
				}
				else if (c == 'D')
				{
					state = ID;
				}
				else if (c == 'C')
				{
					state = IC;
				}
				else if (c == 'F')
				{
					state = IF;
				}
				else if (c == 'T')
				{
					state = IT;
				}
				else if (c == 'L')
				{
					state = IL;
				}
				else if (c == 'W')
				{
					state = IW;
				}
				else if (c == 'N')
				{
					state = IN;
				}
				else if (c == 'A')
				{
					state = IA;
				}
				else if (c == 'O')
				{
					state = IO;
				}
				else if (isalpha(c) || c == '_')
				{
					state = IDENT;
				}
				else if (c == ',')
				{
					state = C;
				}
				else if (c == '*')
				{
					state = S;
				}
				else if (c == '(')
				{
					state = O;
				}
				else if (c == ')')
				{
					state = CL;
				}
				else if (c == '+')
				{
					state = P;
				}
				else if (c == '-')
				{
					state = M;
				}
				else if (c == '\'')
				{
					state = STR;
				}
				else if (c == '=')
				{
					state = E;
				}
				else if (isdigit(c))
				{
					state = LN;
				}
				else if (c == '/')
				{
					state = D;
				}
				else if (c == '%')
				{
					state = MO;
				}
				else if (c == '>')
				{
					state = G_GE;
				}
				else if (c == '<')
				{
					state = L_LE;
				}
				else if (c == '!')
				{
					state = NE;
				}
				else if (c == '\n')
				{
					cur_lex_type = lex_type_t::END;
					state = OK;
				}
				else if (isspace(c))
				{
					//stay in H
				}
				else
				{
					throw std::logic_error("Unexpected caracter with code " + std::to_string(c) + " in position " + std::to_string(cur_lex_pos));
				}
				break;

			case IS:

				if (c == 'E')
				{
					state = ISE;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case II:

				if (c == 'N')
				{
					state = IIN;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IU:

				if (c == 'P')
				{
					state = IUP;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ID:

				if (c == 'E')
				{
					state = IDE;
				}
				else if (c == 'R')
				{
					state = IDR;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IC:

				if (c == 'R')
				{
					state = ICR;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IF:

				if (c == 'R')
				{
					state = IFR;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IT:

				if (c == 'A')
				{
					state = ITA;
				}
				else if (c == 'E')
				{
					state = ITE;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IL:

				if (c == 'O')
				{
					state = ILO;
				}
				else if (c == 'I')
				{
					state = ILI;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IW:

				if (c == 'H')
				{
					state = IWH;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IN:

				if (c == 'O')
				{
					state = INO;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IA:

				if (c == 'L')
				{
					state = IAL;
				}
				else if (c == 'N')
				{
					state = IAN;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IO:

				if (c == 'R')
				{
					state = IOR;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IDENT:

				if (isalnum(c) || c == '_')
				{
					// stay in IDENT
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case C:

				cur_lex_type = lex_type_t::COMMA;
				state = OK;
				break;

			case S:

				cur_lex_type = lex_type_t::STAR;
				state = OK;
				break;

			case O:

				cur_lex_type = lex_type_t::OPEN;
				state = OK;
				break;

			case CL:

				cur_lex_type = lex_type_t::CLOSE;
				state = OK;
				break;

			case LN:

				if (isdigit(c))
				{
					//stay in LN
				}
				else
				{
					cur_lex_type = lex_type_t::LONG_NUM;
					state = OK;
				}
				break;

			case STR:

				if (c == '\'')
				{
					throw std::logic_error("String should have at least one character");
				}
				else if (c == '\n')
				{
					throw std::logic_error("Unexpected caracter with code " + std::to_string(c) + " in position " + std::to_string(cur_lex_pos));
				}
				else
				{
					state = STR_MAY_END;
				}
				break;

			case STR_MAY_END:

				if (c == '\'')
				{
					state = STR_END;
				}
				else if (c == '\n')
				{
					throw std::logic_error("Unexpected caracter with code " + std::to_string(c) + " in position " + std::to_string(cur_lex_pos));
				}
				break;

			case E:

				cur_lex_type = lex_type_t::EQ;
				state = OK;
				break;

			case P:

				cur_lex_type = lex_type_t::PLUS;
				state = OK;
				break;

			case M:

				cur_lex_type = lex_type_t::MINUS;
				state = OK;
				break;

			case D:

				cur_lex_type = lex_type_t::DEL;
				state = OK;
				break;

			case MO:

				cur_lex_type = lex_type_t::MOD;
				state = OK;
				break;

			case G_GE:

				if (c == '=')
				{
					state = GE;
				}
				else
				{
					cur_lex_type = lex_type_t::GR;
					state = OK;
				}
				break;

			case L_LE:

				if (c == '=')
				{
					state = LE;
				}
				else
				{
					cur_lex_type = lex_type_t::LS;
					state = OK;
				}
				break;

			case NE:

				if (c == '=')
				{
					state = NE_END;
				}
				else
				{
					throw std::logic_error("Unexpected caracter with code " + std::to_string(c) + " in position " + std::to_string(cur_lex_pos));
				}
				break;

			case ISE:

				if (c == 'L')
				{
					state = ISEL;
				}
				else if (c == 'T')
				{
					state = ISET;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IIN:

				if (c == 'S')
				{
					state = IINS;
				}
				else if (c == 'T')
				{
					state = IINT;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::IN;
					state = OK;
				}
				break;

			case IUP:

				if (c == 'D')
				{
					state = IUPD;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IDE:

				if (c == 'L')
				{
					state = IDEL;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IDR:

				if (c == 'O')
				{
					state = IDRO;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ICR:

				if (c == 'E')
				{
					state = ICRE;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IFR:

				if (c == 'O')
				{
					state = IFRO;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ITA:

				if (c == 'B')
				{
					state = ITAB;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ITE:

				if (c == 'X')
				{
					state = ITEX;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ILO:

				if (c == 'N')
				{
					state = ILON;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ILI:

				if (c == 'K')
				{
					state = ILIK;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IWH:

				if (c == 'E')
				{
					state = IWHE;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case INO:

				if (c == 'T')
				{
					state = INOT;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IAL:

				if (c == 'L')
				{
					state = IALL;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IAN:

				if (c == 'D')
				{
					state = IAND;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IOR:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::OR;
					state = OK;
				}
				break;

			case STR_END:

				cur_lex_type = lex_type_t::STR;
				state = OK;
				break;

			case GE:

				cur_lex_type = lex_type_t::GR_EQ;
				state = OK;
				break;

			case LE:

				cur_lex_type = lex_type_t::LS_EQ;
				state = OK;
				break;

			case NE_END:

				cur_lex_type = lex_type_t::NOT_EQ;
				state = OK;
				break;

			case ISEL:

				if (c == 'E')
				{
					state = ISELE;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ISET:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::SET;
					state = OK;
				}
				break;

			case IINS:

				if (c == 'E')
				{
					state = IINSE;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IINT:

				if (c == 'O')
				{
					state = IINTO;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IUPD:

				if (c == 'A')
				{
					state = IUPDA;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IDEL:

				if (c == 'E')
				{
					state = IDELE;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IDRO:

				if (c == 'P')
				{
					state = IDROP;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ICRE:

				if (c == 'A')
				{
					state = ICREA;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IFRO:

				if (c == 'M')
				{
					state = IFROM;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ITAB:

				if (c == 'L')
				{
					state = ITABL;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ITEX:

				if (c == 'T')
				{
					state = ITEXT;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ILON:

				if (c == 'G')
				{
					state = ILONG;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ILIK:

				if (c == 'E')
				{
					state = ILIKE;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IWHE:

				if (c == 'R')
				{
					state = IWHER;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case INOT:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::NOT;
					state = OK;
				}
				break;

			case IALL:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ALL;
					state = OK;
				}
				break;

			case IAND:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::AND;
					state = OK;
				}
				break;

			case ISELE:

				if (c == 'C')
				{
					state = ISELEC;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IINSE:

				if (c == 'R')
				{
					state = IINSER;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IINTO:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::INTO;
					state = OK;
				}
				break;

			case IUPDA:

				if (c == 'T')
				{
					state = IUPDAT;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IDELE:

				if (c == 'T')
				{
					state = IDELET;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IDROP:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::DROP;
					state = OK;
				}
				break;

			case ICREA:

				if (c == 'T')
				{
					state = ICREAT;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IFROM:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::FROM;
					state = OK;
				}
				break;

			case ITABL:

				if (c == 'E')
				{
					state = ITABLE;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ITEXT:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::TEXT;
					state = OK;
				}
				break;

			case ILONG:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::LONG;
					state = OK;
				}
				break;

			case ILIKE:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::LIKE;
					state = OK;
				}
				break;

			case IWHER:

				if (c == 'E')
				{
					state = IWHERE;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ISELEC:

				if (c == 'T')
				{
					state = ISELECT;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IINSER:

				if (c == 'T')
				{
					state = IINSERT;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IUPDAT:

				if (c == 'E')
				{
					state = IUPDATE;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case IDELET:

				if (c == 'E')
				{
					state = IDELETE;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ICREAT:

				if (c == 'E')
				{
					state = ICREATE;
				}
				else if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::ID;
					state = OK;
				}
				break;

			case ITABLE:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::TABLE;
					state = OK;
				}
				break;

			case IWHERE:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::WHERE;
					state = OK;
				}
				break;

			case ISELECT:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::SELECT;
					state = OK;
				}
				break;

			case IINSERT:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::INSERT;
					state = OK;
				}
				break;

			case IUPDATE:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::UPDATE;
					state = OK;
				}
				break;

			case IDELETE:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::DELETE;
					state = OK;
				}
				break;

			case ICREATE:

				if (isalnum(c) || c == '_')
				{
					state = IDENT;
				}
				else
				{
					cur_lex_type = lex_type_t::CREATE;
					state = OK;
				}
				break;

			case OK:

				break;
			}

			if (state != OK)
			{
				if (!isspace(c) || state == STR)
				{
					cur_lex_text.push_back(c);
				}
				c = std::cin.get();
				++cur_lex_pos;
			}
		}
	}
}

namespace parser
{
	select_t sel;
	insert ins;
	update upd;
	del de;
	create cre;
	drop dro;
	where1 wh1;
	where2 wh2;
	where3 wh3;
	field_def tmp;
	std::vector<std::string> logic_tmp;
	std::string str_tmp;
	bool deep_flag, flag_first, not_flag, deep_flag2, deep_flag3;
	int par_counter, w7_counter, w20_counter, w3_counter;
	int type, type_where;

	void init()
	{
		logic_tmp.clear();
		str_tmp.clear();
		sel.clear();
		ins.clear();
		upd.clear();
		de.clear();
		cre.clear();
		dro.clear();
		wh1.clear();
		wh2.clear();
		wh3.clear();
		vec_look3.clear();
		counter = 0;
		par_counter = 0;
		w7_counter = 0;
		flag_vec = false;
		type_where = 0;
		deep_flag = false;
		flag_first = true;
		not_flag = false;
		deep_flag2 = false;
		w20_counter = 0;
		w3_counter = 0;
		deep_flag3 = false;
		lexer::init();
		lexer::next();
	}

	void A();
	void B();
	void C();
	void D();
	void E();
	void F();
	void A1();
	void W();
	void B1();
	void E1();
	void E2();
	void E3();
	void WE1();
	void W11_B1();
	void W1_C();
	void W4_C();
	void W6_C();
	void W7_C();
	void W8_C();
	void W11_C();
	void WE23();
	void W1();
	void W2();
	void W3();
	void W4();
	void W5();
	void W6();
	void W7();
	void W8();
	void W9();
	void WE2();
	void WE21();
	void WE22();
	void WE24();
	void WE25();
	void W10();
	void W11();
	void W12();
	void W13();
	void W14();
	void W15();
	void W16();
	void W17();
	void W18();
	void W19();
	void W20();
	void W21();
	void W22();

	void S()
	{
		if (lexer::cur_lex_type == lex_type_t::SELECT)
		{
			type = 1;
			lexer::next();
			A();
		}
		else if (lexer::cur_lex_type == lex_type_t::INSERT)
		{
			type = 2;
			lexer::next();
			B();
		}
		else if (lexer::cur_lex_type == lex_type_t::UPDATE)
		{
			type = 3;
			lexer::next();
			C();
		}
		else if (lexer::cur_lex_type == lex_type_t::DELETE)
		{
			type = 4;
			lexer::next();
			D();
		}
		else if (lexer::cur_lex_type == lex_type_t::CREATE)
		{
			type = 5;
			lexer::next();
			E();
		}
		else if (lexer::cur_lex_type == lex_type_t::DROP)
		{
			type = 6;
			lexer::next();
			F();
		}
		else
		{
			throw std::logic_error("Request should start with SELECT/INSERT/UPDATE/DELETE/CREATE/DROP");
		}
	}

	void A()
	{
		A1();
		if (lexer::cur_lex_type != lex_type_t::FROM)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::ID)
		{
			throw std::logic_error("Incorrect syntax");
		}
		sel.name = lexer::cur_lex_text;
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::WHERE)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		W();
	}

	void A1()
	{
		if (lexer::cur_lex_type == lex_type_t::ID)
		{
			sel.fields.push_back(lexer::cur_lex_text);
			lexer::next();
			while (lexer::cur_lex_type == lex_type_t::COMMA)
			{
				lexer::next();
				if (lexer::cur_lex_type != lex_type_t::ID)
				{
					throw std::logic_error("Incorrect syntax");
				}
				sel.fields.push_back(lexer::cur_lex_text);
				lexer::next();
			}
		}
		else if (lexer::cur_lex_type == lex_type_t::STAR)
		{
			lexer::next();
		}
		else
		{
			throw std::logic_error("Incorrect syntax");
		}
	}

	void B()
	{
		if (lexer::cur_lex_type != lex_type_t::INTO)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::ID)
		{
			throw std::logic_error("Incorrect syntax");
		}
		ins.name = lexer::cur_lex_text;
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::OPEN)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		B1();
		while (lexer::cur_lex_type == lex_type_t::COMMA)
		{
			lexer::next();
			B1();
		}
		if (lexer::cur_lex_type != lex_type_t::CLOSE)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
	}

	void B1()
	{
		if (lexer::cur_lex_type == lex_type_t::STR)
		{
			ins.fields_str.push_back(lexer::cur_lex_text);
			ins.flags.push_back(0); // false means STR
			lexer::next();
		}
		else
		{
			W11_B1();
		}
	}

	void W11_B1()
	{
		if (lexer::cur_lex_type == lex_type_t::LONG_NUM)
		{
			ins.fields_num.push_back(std::strtoll(lexer::cur_lex_text.data(), nullptr, 10));
			if (errno == ERANGE)
			{
				errno = 0;
				throw std::logic_error("Too big number " + lexer::cur_lex_text);
			}
			ins.flags.push_back(1); // true means NUM
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::PLUS)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::LONG_NUM)
			{
				throw std::logic_error("Incorrect syntax");
			}
			ins.fields_num.push_back(std::strtoll(lexer::cur_lex_text.data(), nullptr, 10));
			if (errno == ERANGE)
			{
				errno = 0;
				throw std::logic_error("Too big number " + lexer::cur_lex_text);
			}
			ins.flags.push_back(1);
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::MINUS)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::LONG_NUM)
			{
				throw std::logic_error("Incorrect syntax");
			}
			if (lexer::cur_lex_text == "9223372036854775808")
			{
				ins.fields_num.push_back(LLONG_MIN);
			}
			else
			{
				ins.fields_num.push_back(std::strtoll(lexer::cur_lex_text.data(), nullptr, 10) * -1);
			}
			if (errno == ERANGE)
			{
				errno = 0;
				throw std::logic_error("Too big number -" + lexer::cur_lex_text);
			}
			ins.flags.push_back(1);
			lexer::next();
		}
		else
		{
			throw std::logic_error("Incorrect syntax");
		}
	}

	void C()
	{
		if (lexer::cur_lex_type != lex_type_t::ID)
		{
			throw std::logic_error("Incorrect syntax");
		}
		upd.name = lexer::cur_lex_text;
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::SET)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::ID)
		{
			throw std::logic_error("Incorrect syntax");
		}
		upd.field = lexer::cur_lex_text;
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::EQ)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		W1_C();
		if (lexer::cur_lex_type != lex_type_t::WHERE)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		W();
	}

	void W1_C()
	{
		if (lexer::cur_lex_type == lex_type_t::STR)
		{
			upd.expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else
		{
			W4_C();
		}
	}

	void W4_C()
	{
		W6_C();
		while (lexer::cur_lex_type == lex_type_t::PLUS || lexer::cur_lex_type == lex_type_t::MINUS)
		{
			int oper;
			if (lexer::cur_lex_type == lex_type_t::PLUS)
			{
				oper = 0;
			}
			else
			{
				oper = 1;
			}
			lexer::next();
			W6_C();
			if (oper == 0)
			{
				upd.expr.push_back(std::string("+"));
			}
			else
			{
				upd.expr.push_back(std::string("-"));
			}
		}
	}

	void W6_C()
	{
		W7_C();
		while (lexer::cur_lex_type == lex_type_t::STAR || lexer::cur_lex_type == lex_type_t::DEL || lexer::cur_lex_type == lex_type_t::MOD)
		{
			int oper;
			if (lexer::cur_lex_type == lex_type_t::STAR)
			{
				oper = 0;
			}
			else if (lexer::cur_lex_type == lex_type_t::DEL)
			{
				oper = 1;
			}
			else
			{
				oper = 2;
			}
			lexer::next();
			W7_C();
			if (oper == 0)
			{
				upd.expr.push_back(std::string("*"));
			}
			else if (oper == 1)
			{
				upd.expr.push_back(std::string("/"));
			}
			else
			{
				upd.expr.push_back(std::string("%"));
			}
		}
	}

	void W7_C()
	{
		if (lexer::cur_lex_type == lex_type_t::OPEN)
		{
			lexer::next();
			W4_C();
			if (lexer::cur_lex_type != lex_type_t::CLOSE)
			{
				throw std::logic_error("Incorrect syntax");
			}
			lexer::next();
		}
		else
		{
			W8_C();
		}
	}

	void W8_C()
	{
		if (lexer::cur_lex_type == lex_type_t::ID)
		{
			upd.expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else
		{
			W11_C();
		}
	}

	void W11_C()
	{
		if (lexer::cur_lex_type == lex_type_t::LONG_NUM)
		{
			upd.expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::PLUS)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::LONG_NUM)
			{
				throw std::logic_error("Incorrect syntax");
			}
			upd.expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::MINUS)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::LONG_NUM)
			{
				throw std::logic_error("Incorrect syntax");
			}
			upd.expr.push_back(std::string("-") + lexer::cur_lex_text);
			lexer::next();
		}
		else
		{
			throw std::logic_error("Incorrect syntax");
		}
	}

	void D()
	{
		if (lexer::cur_lex_type != lex_type_t::FROM)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::ID)
		{
			throw std::logic_error("Incorrect syntax");
		}
		de.name = lexer::cur_lex_text;
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::WHERE)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		W();
	}

	void E()
	{
		if (lexer::cur_lex_type != lex_type_t::TABLE)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::ID)
		{
			throw std::logic_error("Incorrect syntax");
		}
		cre.name = lexer::cur_lex_text;
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::OPEN)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		E1();
		if (lexer::cur_lex_type != lex_type_t::CLOSE)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
	}

	void E1()
	{
		E2();
		while (lexer::cur_lex_type == lex_type_t::COMMA)
		{
			lexer::next();
			E2();
		}
	}

	void E2()
	{
		if (lexer::cur_lex_type != lex_type_t::ID)
		{
			throw std::logic_error("Incorrect syntax");
		}
		tmp.field = lexer::cur_lex_text;
		lexer::next();
		E3();
	}

	void E3()
	{
		if (lexer::cur_lex_type == lex_type_t::TEXT)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::OPEN)
			{
				throw std::logic_error("Incorrect syntax");
			}
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::LONG_NUM)
			{
				throw std::logic_error("Incorrect syntax");
			}
			tmp.size = std::strtoll(lexer::cur_lex_text.data(), nullptr, 10);
			if (errno == ERANGE)
			{
				errno = 0;
				throw std::logic_error("Too big number " + lexer::cur_lex_text);
			}
			cre.fields_def.push_back(std::move(tmp));
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::CLOSE)
			{
				throw std::logic_error("Incorrect syntax");
			}
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::LONG)
		{
			tmp.size = -1;
			cre.fields_def.push_back(std::move(tmp));
			lexer::next();
		}
		else
		{
			throw std::logic_error("Incorrect syntax");
		}
	}

	void F()
	{
		if (lexer::cur_lex_type != lex_type_t::TABLE)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::ID)
		{
			throw std::logic_error("Incorrect syntax");
		}
		dro.name = lexer::cur_lex_text;
		lexer::next();
	}

	void W()
	{
		if (lexer::cur_lex_type == lex_type_t::ALL)
		{
			type_where = 4;
			lexer::next();
			// ALL
		}
		else
		{
			vec_look3.push_back(lexema{ lexer::cur_lex_type, lexer::cur_lex_text });
			lexer::next();
			vec_look3.push_back(lexema{ lexer::cur_lex_type, lexer::cur_lex_text });
			lexer::next();
			vec_look3.push_back(lexema{ lexer::cur_lex_type, lexer::cur_lex_text });
			flag_vec = true;
			lexer::next();
			if (vec_look3[1].type == lex_type_t::LIKE || vec_look3[2].type == lex_type_t::LIKE)
			{
				type_where = 1;
				WE1();
			}
			else
			{
				W1();
				if (par_counter != 0)
				{
					throw std::logic_error("Incorrect syntax");
				}
			}
		}
	}

	void WE23()
	{
		bool local_flag = false;
		bool local_flag2 = true;
		if (not_flag)
		{
			local_flag2 = false;
			not_flag = false;
			if (lexer::cur_lex_type != lex_type_t::OPEN)
			{
				local_flag = true;
			}
		}
		while (lexer::cur_lex_type == lex_type_t::OPEN)
		{
			++par_counter;
			lexer::next();
		}
		if (lexer::cur_lex_type == lex_type_t::NOT)
		{
			W3();
		}
		else
		{
			if (flag_first)
			{
				flag_first = false;
				W4();
				if (par_counter == 0 && type_where == 0)
				{
					type_where = 2;
					WE2();
				}
				else
				{
					type_where = 3;
					wh3.logic_expr = std::move(wh2.expr);
					W10();
					W11();
					wh3.logic_expr.push_back(str_tmp);
					if (!local_flag && local_flag2)
					{
						if (par_counter == 0 || lexer::cur_lex_type != lex_type_t::CLOSE)
						{
							throw std::logic_error("Incorrect syntax");
						}
						lexer::next();
						--par_counter;
					}
				}
			}
			else
			{
				W17();
				W10();
				W11();
				wh3.logic_expr.push_back(str_tmp);
				if (!local_flag && local_flag2)
				{
					if (par_counter == 0 || lexer::cur_lex_type != lex_type_t::CLOSE)
					{
						throw std::logic_error("Incorrect syntax");
					}
					lexer::next();
					--par_counter;
				}
			}
		}
	}

	void W17()
	{
		if (lexer::cur_lex_type == lex_type_t::STR)
		{
			wh3.logic_expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else
		{
			W18();
		}
	}

	void W18()
	{
	L2:
		W19();
		while (lexer::cur_lex_type == lex_type_t::PLUS || lexer::cur_lex_type == lex_type_t::MINUS)
		{
			int oper;
			if (lexer::cur_lex_type == lex_type_t::PLUS)
			{
				oper = 0;
			}
			else
			{
				oper = 1;
			}
			lexer::next();
			W19();
			if (oper == 0)
			{
				wh3.logic_expr.push_back(std::string("+"));
			}
			else
			{
				wh3.logic_expr.push_back(std::string("-"));
			}
		}
		if (w20_counter == 0 && lexer::cur_lex_type == lex_type_t::CLOSE)
		{
			if (par_counter == 0)
			{
				throw std::logic_error("Incorrect syntax");
			}
			lexer::next();
			--par_counter;
			deep_flag2 = true;
			goto L2;
		}
	}

	void W19()
	{
		W20();
		while (lexer::cur_lex_type == lex_type_t::STAR || lexer::cur_lex_type == lex_type_t::DEL || lexer::cur_lex_type == lex_type_t::MOD)
		{
			int oper;
			if (lexer::cur_lex_type == lex_type_t::STAR)
			{
				oper = 0;
			}
			else if (lexer::cur_lex_type == lex_type_t::DEL)
			{
				oper = 1;
			}
			else
			{
				oper = 2;
			}
			lexer::next();
			W20();
			if (oper == 0)
			{
				wh3.logic_expr.push_back(std::string("*"));
			}
			else if (oper == 1)
			{
				wh3.logic_expr.push_back(std::string("/"));
			}
			else
			{
				wh3.logic_expr.push_back(std::string("%"));
			}
		}
	}

	void W20()
	{
		if (deep_flag2)
		{
			deep_flag2 = false;
			return;
		}
		if (lexer::cur_lex_type == lex_type_t::OPEN)
		{
			++w20_counter;
			lexer::next();
			W18();
			if (lexer::cur_lex_type != lex_type_t::CLOSE)
			{
				throw std::logic_error("Incorrect syntax");
			}
			--w20_counter;
			lexer::next();
		}
		else
		{
			W21();
		}
	}

	void W21()
	{
		if (lexer::cur_lex_type == lex_type_t::ID)
		{
			wh3.logic_expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else
		{
			W22();
		}
	}

	void W22()
	{
		if (lexer::cur_lex_type == lex_type_t::LONG_NUM)
		{
			wh3.logic_expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::PLUS)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::LONG_NUM)
			{
				throw std::logic_error("Incorrect syntax");
			}
			wh3.logic_expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::MINUS)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::LONG_NUM)
			{
				throw std::logic_error("Incorrect syntax");
			}
			wh3.logic_expr.push_back(std::string("-") + lexer::cur_lex_text);
			lexer::next();
		}
		else
		{
			throw std::logic_error("Incorrect syntax");
		}
	}

	void W11()
	{
		if (lexer::cur_lex_type == lex_type_t::STR)
		{
			wh3.logic_expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else
		{
			W12();
		}
	}

	void W12()
	{
		W13();
		while (lexer::cur_lex_type == lex_type_t::PLUS || lexer::cur_lex_type == lex_type_t::MINUS)
		{
			int oper;
			if (lexer::cur_lex_type == lex_type_t::PLUS)
			{
				oper = 0;
			}
			else
			{
				oper = 1;
			}
			lexer::next();
			W13();
			if (oper == 0)
			{
				wh3.logic_expr.push_back(std::string("+"));
			}
			else
			{
				wh3.logic_expr.push_back(std::string("-"));
			}
		}
	}

	void W13()
	{
		W14();
		while (lexer::cur_lex_type == lex_type_t::STAR || lexer::cur_lex_type == lex_type_t::DEL || lexer::cur_lex_type == lex_type_t::MOD)
		{
			int oper;
			if (lexer::cur_lex_type == lex_type_t::STAR)
			{
				oper = 0;
			}
			else if (lexer::cur_lex_type == lex_type_t::DEL)
			{
				oper = 1;
			}
			else
			{
				oper = 2;
			}
			lexer::next();
			W14();
			if (oper == 0)
			{
				wh3.logic_expr.push_back(std::string("*"));
			}
			else if (oper == 1)
			{
				wh3.logic_expr.push_back(std::string("/"));
			}
			else
			{
				wh3.logic_expr.push_back(std::string("%"));
			}
		}
	}

	void W14()
	{
		if (lexer::cur_lex_type == lex_type_t::OPEN)
		{
			lexer::next();
			W12();
			if (lexer::cur_lex_type != lex_type_t::CLOSE)
			{
				throw std::logic_error("Incorrect syntax");
			}
			lexer::next();
		}
		else
		{
			W15();
		}
	}

	void W15()
	{
		if (lexer::cur_lex_type == lex_type_t::ID)
		{
			wh3.logic_expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else
		{
			W16();
		}
	}

	void W16()
	{
		if (lexer::cur_lex_type == lex_type_t::LONG_NUM)
		{
			wh3.logic_expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::PLUS)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::LONG_NUM)
			{
				throw std::logic_error("Incorrect syntax");
			}
			wh3.logic_expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::MINUS)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::LONG_NUM)
			{
				throw std::logic_error("Incorrect syntax");
			}
			wh3.logic_expr.push_back(std::string("-") + lexer::cur_lex_text);
			lexer::next();
		}
		else
		{
			throw std::logic_error("Incorrect syntax");
		}
	}

	void W10()
	{
		if (lexer::cur_lex_type == lex_type_t::EQ)
		{
			str_tmp = lexer::cur_lex_text;
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::GR)
		{
			str_tmp = lexer::cur_lex_text;
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::LS)
		{
			str_tmp = lexer::cur_lex_text;
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::GR_EQ)
		{
			str_tmp = lexer::cur_lex_text;
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::LS_EQ)
		{
			str_tmp = lexer::cur_lex_text;
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::NOT_EQ)
		{
			str_tmp = lexer::cur_lex_text;
			lexer::next();
		}
		else
		{
			throw std::logic_error("Incorrect syntax");
		}
	}

	void W1()
	{
	L3:
		W2();
		while (lexer::cur_lex_type == lex_type_t::OR)
		{
			lexer::next();
			W2();
			wh3.logic_expr.push_back(std::string("OR"));
		}
		if (w3_counter == 0 && lexer::cur_lex_type == lex_type_t::CLOSE)
		{
			if (par_counter == 0)
			{
				throw std::logic_error("Incorrect syntax");
			}
			lexer::next();
			--par_counter;
			deep_flag3 = true;
			goto L3;
		}
	}

	void W2()
	{
		W3();
		while (lexer::cur_lex_type == lex_type_t::AND)
		{
			lexer::next();
			W3();
			wh3.logic_expr.push_back(std::string("AND"));
		}
	}

	void W3()
	{
		if (deep_flag3)
		{
			deep_flag3 = false;
			return;
		}
		if (lexer::cur_lex_type == lex_type_t::NOT)
		{
			type_where = 3;
			lexer::next();
			bool not_counter = true;
			while (lexer::cur_lex_type == lex_type_t::NOT)
			{
				lexer::next();
				not_counter ^= true;
			}
			not_flag = true;
			++w3_counter;
			if (lexer::cur_lex_type != lex_type_t::OPEN)
			{
				throw std::logic_error("Incorrect syntax");
			}
			lexer::next();
			W1();
			if (lexer::cur_lex_type != lex_type_t::CLOSE)
			{
				throw std::logic_error("Incorrect syntax");
			}
			lexer::next();
			--w3_counter;
			if (not_counter)
			{
				wh3.logic_expr.push_back(std::string("NOT"));
			}
		}
		else
		{
			WE23();
		}
	}

	void W4()
	{
		if (lexer::cur_lex_type == lex_type_t::STR)
		{
			wh2.expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else
		{
			W5();
		}
	}

	void W5()
	{
	L1:
		W6();
		while (lexer::cur_lex_type == lex_type_t::PLUS || lexer::cur_lex_type == lex_type_t::MINUS)
		{
			int oper;
			if (lexer::cur_lex_type == lex_type_t::PLUS)
			{
				oper = 0;
			}
			else
			{
				oper = 1;
			}
			lexer::next();
			W6();
			if (oper == 0)
			{
				wh2.expr.push_back(std::string("+"));
			}
			else
			{
				wh2.expr.push_back(std::string("-"));
			}
		}
		if (w7_counter == 0 && lexer::cur_lex_type == lex_type_t::CLOSE)
		{
			if (par_counter == 0)
			{
				throw std::logic_error("Incorrect syntax");
			}
			lexer::next();
			--par_counter;
			deep_flag = true;
			goto L1;
		}
	}

	void W6()
	{
		W7();
		while (lexer::cur_lex_type == lex_type_t::STAR || lexer::cur_lex_type == lex_type_t::DEL || lexer::cur_lex_type == lex_type_t::MOD)
		{
			int oper;
			if (lexer::cur_lex_type == lex_type_t::STAR)
			{
				oper = 0;
			}
			else if (lexer::cur_lex_type == lex_type_t::DEL)
			{
				oper = 1;
			}
			else
			{
				oper = 2;
			}
			lexer::next();
			W7();
			if (oper == 0)
			{
				wh2.expr.push_back(std::string("*"));
			}
			else if (oper == 1)
			{
				wh2.expr.push_back(std::string("/"));
			}
			else
			{
				wh2.expr.push_back(std::string("%"));
			}
		}
	}

	void W7()
	{
		if (deep_flag)
		{
			deep_flag = false;
			return;
		}
		if (lexer::cur_lex_type == lex_type_t::OPEN)
		{
			++w7_counter;
			lexer::next();
			W5();
			if (lexer::cur_lex_type != lex_type_t::CLOSE)
			{
				throw std::logic_error("Incorrect syntax");
			}
			--w7_counter;
			lexer::next();
		}
		else
		{
			W8();
		}
	}

	void W8()
	{
		if (lexer::cur_lex_type == lex_type_t::ID)
		{
			wh2.expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else
		{
			W9();
		}
	}

	void W9()
	{
		if (lexer::cur_lex_type == lex_type_t::LONG_NUM)
		{
			wh2.expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::PLUS)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::LONG_NUM)
			{
				throw std::logic_error("Incorrect syntax");
			}
			wh2.expr.push_back(lexer::cur_lex_text);
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::MINUS)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::LONG_NUM)
			{
				throw std::logic_error("Incorrect syntax");
			}
			wh2.expr.push_back(std::string("-") + lexer::cur_lex_text);
			lexer::next();
		}
		else
		{
			throw std::logic_error("Incorrect syntax");
		}
	}

	void WE1()
	{
		if (lexer::cur_lex_type != lex_type_t::ID)
		{
			throw std::logic_error("Incorrect syntax");
		}
		wh1.field = lexer::cur_lex_text;
		lexer::next();
		if (lexer::cur_lex_type == lex_type_t::NOT)
		{
			wh1.neg = true;
			lexer::next();
		}
		if (lexer::cur_lex_type != lex_type_t::LIKE)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::STR)
		{
			throw std::logic_error("Incorrect syntax");
		}
		wh1.str = lexer::cur_lex_text;
		lexer::next();
	}

	void WE2()
	{
		if (lexer::cur_lex_type == lex_type_t::NOT)
		{
			wh2.neg = true;
			lexer::next();
		}
		if (lexer::cur_lex_type != lex_type_t::IN)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		if (lexer::cur_lex_type != lex_type_t::OPEN)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
		WE21();
		if (lexer::cur_lex_type != lex_type_t::CLOSE)
		{
			throw std::logic_error("Incorrect syntax");
		}
		lexer::next();
	}

	void WE21()
	{
		if (lexer::cur_lex_type == lex_type_t::STR)
		{
			WE22();
		}
		else
		{
			WE24();
		}
	}

	void WE22()
	{
		wh2.list_str.push_back(lexer::cur_lex_text);
		lexer::next();
		while (lexer::cur_lex_type == lex_type_t::COMMA)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::STR)
			{
				throw std::logic_error("Incorrect syntax");
			}
			wh2.list_str.push_back(lexer::cur_lex_text);
			lexer::next();
		}
	}

	void WE24()
	{
		WE25();
		while (lexer::cur_lex_type == lex_type_t::COMMA)
		{
			lexer::next();
			WE25();
		}
	}

	void WE25()
	{
		if (lexer::cur_lex_type == lex_type_t::LONG_NUM)
		{
			wh2.list_num.push_back(std::strtoll(lexer::cur_lex_text.data(), nullptr, 10));
			if (errno == ERANGE)
			{
				errno = 0;
				throw std::logic_error("Too big number " + lexer::cur_lex_text);
			}
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::PLUS)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::LONG_NUM)
			{
				throw std::logic_error("Incorrect syntax");
			}
			wh2.list_num.push_back(std::strtoll(lexer::cur_lex_text.data(), nullptr, 10));
			if (errno == ERANGE)
			{
				errno = 0;
				throw std::logic_error("Too big number " + lexer::cur_lex_text);
			}
			lexer::next();
		}
		else if (lexer::cur_lex_type == lex_type_t::MINUS)
		{
			lexer::next();
			if (lexer::cur_lex_type != lex_type_t::LONG_NUM)
			{
				throw std::logic_error("Incorrect syntax");
			}
			if (lexer::cur_lex_text == "9223372036854775808")
			{
				wh2.list_num.push_back(LLONG_MIN);
			}
			else
			{
				wh2.list_num.push_back(std::strtoll(lexer::cur_lex_text.data(), nullptr, 10) * -1);
			}
			if (errno == ERANGE)
			{
				errno = 0;
				throw std::logic_error("Too big number -" + lexer::cur_lex_text);
			}
			lexer::next();
		}
		else
		{
			throw std::logic_error("Incorrect syntax");
		}
	}

	void last_check()
	{
		if (parser::counter != parser::vec_look3.size())
		{
			throw std::logic_error("Incorrect syntax");
		}
		if (lexer::cur_lex_type != lex_type_t::END)
		{
			throw std::logic_error("Incorrect syntax");
		}
	}
}
