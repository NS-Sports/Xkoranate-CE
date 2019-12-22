#ifndef SORTWINPERCENTPURE_H
#define SORTWINPERCENTPURE_H

#include "tablegenerator/tablerow.h"

class XkorSortWinPercentPureGr
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			double wpA = (a.played() == 0 ? 0 : a.wins() / a.played());
			double wpB = (b.played() == 0 ? 0 : b.wins() / b.played());
			return (wpA > wpB);
		}
};

class XkorSortWinPercentPureEq
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			double wpA = (a.played() == 0 ? 0 : a.wins() / a.played());
			double wpB = (b.played() == 0 ? 0 : b.wins() / b.played());
			return (wpA == wpB);
		}
};

#endif
