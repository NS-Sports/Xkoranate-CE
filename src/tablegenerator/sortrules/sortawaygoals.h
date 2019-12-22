#ifndef SORTAWAYGOALS_H
#define SORTAWAYGOALS_H

#include "tablegenerator/tablerow.h"

class XkorSortAwayGoalsGr
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.awayGoals() > b.awayGoals());
		}
};

class XkorSortAwayGoalsEq
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.awayGoals() == b.awayGoals());
		}
};

#endif
