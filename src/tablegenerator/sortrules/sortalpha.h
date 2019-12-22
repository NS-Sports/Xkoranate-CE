#ifndef SORTALPHA_H
#define SORTALPHA_H

#include "tablegenerator/tablerow.h"

class XkorSortAlphaGr
{
	public:
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (QString::localeAwareCompare(a.name(), b.name()) < 0);
		}
};

class XkorSortAlphaEq
{
	public:
		bool operator () (const XkorTableRow &, const XkorTableRow &)
		{
			return true; // if we’re sorting on alphabetical order, the teams are tied!
		}
};

#endif
