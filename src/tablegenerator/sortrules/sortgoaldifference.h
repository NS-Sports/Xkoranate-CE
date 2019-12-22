#ifndef SORTGOALDIFFERENCE_H
#define SORTGOALDIFFERENCE_H

#include "tablegenerator/tablerow.h"

class XkorSortGoalDiffGr
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.goalDifference() > b.goalDifference());
		}
};

class XkorSortGoalDiffEq
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.goalDifference() == b.goalDifference());
		}
};

#endif
