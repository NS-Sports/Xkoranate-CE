#ifndef SORTWINPERCENT_H
#define SORTWINPERCENT_H

#include "tablegenerator/tablerow.h"

class XkorSortWinPercentGr
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			double wpA = (a.played() == 0 ? 0 : (a.wins() + a.draws() / 2) / a.played());
			double wpB = (b.played() == 0 ? 0 : (b.wins() + b.draws() / 2) / b.played());
			return (wpA > wpB);
		}
};

class XkorSortWinPercentEq
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			double wpA = (a.played() == 0 ? 0 : (a.wins() + a.draws() / 2) / a.played());
			double wpB = (b.played() == 0 ? 0 : (b.wins() + b.draws() / 2) / b.played());
			return (wpA == wpB);
		}
};

#endif
