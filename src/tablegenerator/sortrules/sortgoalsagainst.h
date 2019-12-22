#ifndef SORTGOALSAGAINST_H
#define SORTGOALSAGAINST_H

#include "tablegenerator/tablerow.h"

class XkorSortGoalsAgainstGr
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.goalsAgainst() < b.goalsAgainst());
		}
};

class XkorSortGoalsAgainstEq
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.goalsAgainst() == b.goalsAgainst());
		}
};

#endif
