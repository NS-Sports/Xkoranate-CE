#ifndef SORTWINS_H
#define SORTWINS_H

#include "tablegenerator/tablerow.h"

class XkorSortWinsGr
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.wins() > b.wins());
		}
};

class XkorSortWinsEq
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.wins() == b.wins());
		}
};

#endif
