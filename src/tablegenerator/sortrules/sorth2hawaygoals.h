#ifndef SORTH2HAWAYGOALS_H
#define SORTH2HAWAYGOALS_H

#include "tablegenerator/tablerow.h"

class XkorSortH2HAwayGoalsGr
{
	public:
		QStringList l;
		XkorSortH2HAwayGoalsGr(QStringList h2hTeams) : l(h2hTeams) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.h2hAwayGoals(l) > b.h2hAwayGoals(l));
		}
};

class XkorSortH2HAwayGoalsEq
{
	public:
		QStringList l;
		XkorSortH2HAwayGoalsEq(QStringList h2hTeams) : l(h2hTeams) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.h2hAwayGoals(l) == b.h2hAwayGoals(l));
		}
};

#endif
