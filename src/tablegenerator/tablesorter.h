#ifndef TABLESORTER_H
#define TABLESORTER_H

#include "tablegenerator/tablerow.h"

#include <QDebug>

class XkorTableSorter
{
	private:
		double pointsForWin, pointsForDraw, pointsForLoss;
		QStringList h2hTeams;
		template<typename Comparator1, typename Comparator2> std::vector<std::vector<XkorTableRow> > privateSort(std::vector<XkorTableRow> rows, Comparator1 gr, Comparator2 eq);
	public:
		void setH2HTeams(QStringList teams);
		void setPointsForWin(double pts);
		void setPointsForDraw(double pts);
		void setPointsForLoss(double pts);
		std::vector<std::vector<XkorTableRow> > sort(std::vector<XkorTableRow> rows, QString sortType);
};

#endif
