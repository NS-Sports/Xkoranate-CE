#ifndef SORTH2HGOALSAGAINST_H
#define SORTH2HGOALSAGAINST_H

#include "tablegenerator/tablerow.h"

class XkorSortH2HGoalsAgainstGr
{
	public:
		QStringList l;
		XkorSortH2HGoalsAgainstGr(QStringList h2hTeams) : l(h2hTeams) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.h2hGoalsAgainst(l) < b.h2hGoalsAgainst(l));
		}
};

class XkorSortH2HGoalsAgainstEq
{
	public:
		QStringList l;
		XkorSortH2HGoalsAgainstEq(QStringList h2hTeams) : l(h2hTeams) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.h2hGoalsAgainst(l) == b.h2hGoalsAgainst(l));
		}
};

#endif // SORTH2HGOALSAGAINST_H
