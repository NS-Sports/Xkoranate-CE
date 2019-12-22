#ifndef SORTGOALSFOR_H
#define SORTGOALSFOR_H

#include "tablegenerator/tablerow.h"

class XkorSortGoalsForGr
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.goalsFor() > b.goalsFor());
		}
};

class XkorSortGoalsForEq
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.goalsFor() == b.goalsFor());
		}
};

#endif
