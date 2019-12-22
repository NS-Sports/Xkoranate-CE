#ifndef SORTH2HPOINTS_H
#define SORTH2HPOINTS_H

#include "tablegenerator/tablerow.h"

class XkorSortH2HPointsGr
{
	public:
		QStringList l;
		XkorSortH2HPointsGr(QStringList h2hTeams) : l(h2hTeams) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.h2hPoints(l) > b.h2hPoints(l));
		}
};

class XkorSortH2HPointsEq
{
	public:
		QStringList l;
		XkorSortH2HPointsEq(QStringList h2hTeams) : l(h2hTeams) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.h2hPoints(l) == b.h2hPoints(l));
		}
};

#endif
