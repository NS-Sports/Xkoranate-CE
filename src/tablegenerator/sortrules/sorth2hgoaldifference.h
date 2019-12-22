#ifndef SORTH2HGOALDIFFERENCE_H
#define SORTH2HGOALDIFFERENCE_H

#include "tablegenerator/tablerow.h"

class XkorSortH2HGoalDiffGr
{
	public:
		QStringList l;
		XkorSortH2HGoalDiffGr(QStringList h2hTeams) : l(h2hTeams) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.h2hGoalDifference(l) > b.h2hGoalDifference(l));
		}
};

class XkorSortH2HGoalDiffEq
{
	public:
		QStringList l;
		XkorSortH2HGoalDiffEq(QStringList h2hTeams) : l(h2hTeams) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.h2hGoalDifference(l) == b.h2hGoalDifference(l));
		}
};

#endif
