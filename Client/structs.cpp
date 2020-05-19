#include "Header_structures.h"

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

void where1::clear()
{
	field.clear();
	neg = false;
	str.clear();
}

void where2::clear()
{
	expr.clear();
	neg = false;
	list_str.clear();
	list_num.clear();
}

void where3::clear()
{
	logic_expr.clear();
}