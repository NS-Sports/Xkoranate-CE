#ifndef SORTWINPERCENTNFL_H
#define SORTWINPERCENTNFL_H

#include "tablegenerator/tablerow.h"

class XkorSortWinPercentNFLGr
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			double wpA = (a.played() == 0 ? 0 : a.wins() / (a.wins() + a.losses()));
			double wpB = (b.played() == 0 ? 0 : b.wins() / (b.wins() + b.losses()));
			return (wpA > wpB);
		}
};

class XkorSortWinPercentNFLEq
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			double wpA = (a.played() == 0 ? 0 : a.wins() / (a.wins() + a.losses()));
			double wpB = (b.played() == 0 ? 0 : b.wins() / (b.wins() + b.losses()));
			return (wpA == wpB);
		}
};

#endif
