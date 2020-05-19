#pragma once

#include "Header_structures.h"

enum class lex_type_t 
{
	SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ID, FROM, COMMA, STAR, INTO, OPEN, CLOSE, LONG_NUM, STR,
	SET, EQ, TABLE, TEXT, LONG, WHERE, NOT, LIKE, IN, ALL, PLUS, MINUS, DEL, MOD, OR, AND, GR, LS, NOT_EQ, GR_EQ, LS_EQ, END
};

namespace lexer
{
	extern lex_type_t cur_lex_type;
	extern int c;
}

namespace parser
{
	extern int type, type_where;
	extern select_t sel;
	extern insert ins;
	extern update upd;
	extern del de;
	extern create cre;
	extern drop dro;
	extern where1 wh1;
	extern where2 wh2;
	extern where3 wh3;
	void init();
	void S();
	void last_check();
}