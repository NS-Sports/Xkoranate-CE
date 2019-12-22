#ifndef SORTH2HGOALSFOR_H
#define SORTH2HGOALSFOR_H

#include "tablegenerator/tablerow.h"

class XkorSortH2HGoalsForGr
{
	public:
		QStringList l;
		XkorSortH2HGoalsForGr(QStringList h2hTeams) : l(h2hTeams) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.h2hGoalsFor(l) > b.h2hGoalsFor(l));
		}
};

class XkorSortH2HGoalsForEq
{
	public:
		QStringList l;
		XkorSortH2HGoalsForEq(QStringList h2hTeams) : l(h2hTeams) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.h2hGoalsFor(l) == b.h2hGoalsFor(l));
		}
};

#endif
