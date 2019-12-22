#include "rplist.h"

XkorRPList::XkorRPList()
{
	rpCalcType = "olympic";
	teams = true;
	min = 0;
	max = 1;
	eff = 15;
}

double XkorRPList::bonus(const QString & code)
{
	return (bon[code]["bonus"] - min) / (max - min);
}
