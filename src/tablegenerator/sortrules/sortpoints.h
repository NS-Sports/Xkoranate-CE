#ifndef SORTPOINTS_H
#define SORTPOINTS_H

#include "tablegenerator/tablerow.h"

class XkorSortPointsGr
{
	public:
		double pointsForWin, pointsForDraw, pointsForLoss;
		XkorSortPointsGr(double w, double d, double l) : pointsForWin(w), pointsForDraw(d), pointsForLoss(l) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.wins() * pointsForWin + a.draws() * pointsForDraw + a.losses() * pointsForLoss > b.wins() * pointsForWin + b.draws() * pointsForDraw + b.losses() * pointsForLoss);
		}
};

class XkorSortPointsEq
{
	public:
		double pointsForWin, pointsForDraw, pointsForLoss;
		XkorSortPointsEq(double w, double d, double l) : pointsForWin(w), pointsForDraw(d), pointsForLoss(l) {;;;}
		bool operator () (const XkorTableRow & a, const XkorTableRow & b)
		{
			return (a.wins() * pointsForWin + a.draws() * pointsForDraw + a.losses() * pointsForLoss == b.wins() * pointsForWin + b.draws() * pointsForDraw + b.losses() * pointsForLoss);
		}
};

#endif
