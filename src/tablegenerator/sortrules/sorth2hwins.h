#ifndef SORTH2HWINS_H
#define SORTH2HWINS_H

#include "tablegenerator/tablerow.h"

class XkorSortH2HWinsGr
{
	public:
		QStringList l;
		XkorSortH2HWinsGr(QStringList h2hTeams) : l(h2hTeams) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.h2hWins(l) > b.h2hWins(l));
		}
};

class XkorSortH2HWinsEq
{
	public:
		QStringList l;
		XkorSortH2HWinsEq(QStringList h2hTeams) : l(h2hTeams) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.h2hWins(l) == b.h2hWins(l));
		}
};

#endif
