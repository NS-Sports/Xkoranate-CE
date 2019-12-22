#ifndef SORTGOALAVERAGE_H
#define SORTGOALAVERAGE_H

#include <limits>

#include "tablegenerator/tablerow.h"

class XkorSortGoalAverageGr
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			double gaA = (a.goalsAgainst() == 0 ? std::numeric_limits<double>::max() : a.goalsFor() / a.goalsAgainst());
			double gaB = (b.goalsAgainst() == 0 ? std::numeric_limits<double>::max() : b.goalsFor() / b.goalsAgainst());
			return (gaA > gaB);
		}
};

class XkorSortGoalAverageEq
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			double gaA = (a.goalsAgainst() == 0 ? std::numeric_limits<double>::max() : a.goalsFor() / a.goalsAgainst());
			double gaB = (b.goalsAgainst() == 0 ? std::numeric_limits<double>::max() : b.goalsFor() / b.goalsAgainst());
			return (gaA == gaB);
		}
};

#endif
