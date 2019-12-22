#include "tablegenerator/tablesorter.h"

#include "tablegenerator/sortrules/sortalpha.h"
#include "tablegenerator/sortrules/sortawaygoals.h"
#include "tablegenerator/sortrules/sortgoalaverage.h"
#include "tablegenerator/sortrules/sortgoaldifference.h"
#include "tablegenerator/sortrules/sortgoalsagainst.h"
#include "tablegenerator/sortrules/sortgoalsfor.h"
#include "tablegenerator/sortrules/sorth2hawaygoals.h"
#include "tablegenerator/sortrules/sorth2hgoaldifference.h"
#include "tablegenerator/sortrules/sorth2hgoalsagainst.h"
#include "tablegenerator/sortrules/sorth2hgoalsfor.h"
#include "tablegenerator/sortrules/sorth2hpoints.h"
#include "tablegenerator/sortrules/sorth2hwins.h"
#include "tablegenerator/sortrules/sortpoints.h"
#include "tablegenerator/sortrules/sortwinpercent.h"
#include "tablegenerator/sortrules/sortwinpercentnfl.h"
#include "tablegenerator/sortrules/sortwinpercentpure.h"
#include "tablegenerator/sortrules/sortwins.h"

#include <algorithm>

void XkorTableSorter::setH2HTeams(QStringList teams)
{
	h2hTeams = teams;
}

void XkorTableSorter::setPointsForWin(double pts)
{
	pointsForWin = pts;
}

void XkorTableSorter::setPointsForDraw(double pts)
{
	pointsForDraw = pts;
}

void XkorTableSorter::setPointsForLoss(double pts)
{
	pointsForLoss = pts;
}

std::vector<std::vector<XkorTableRow> > XkorTableSorter::sort(std::vector<XkorTableRow> rows, QString sortType)
{
	// h2h team list
	QStringList l;
	if(sortType.left(3) == "h2h")
	{
		for(std::vector<XkorTableRow>::iterator i = rows.begin(); i != rows.end(); ++i)
			l << i->name();
	}
	
	std::vector<std::vector<XkorTableRow> > rval;
	if(sortType == "alphabetical")
		rval = privateSort(rows, XkorSortAlphaGr(), XkorSortAlphaEq());
	else if(sortType == "awayGoals")
		rval = privateSort(rows, XkorSortAwayGoalsGr(), XkorSortAwayGoalsEq());
	else if(sortType == "goalAverage")
		rval = privateSort(rows, XkorSortGoalAverageGr(), XkorSortGoalAverageEq());
	else if(sortType == "goalDifference")
		rval = privateSort(rows, XkorSortGoalDiffGr(), XkorSortGoalDiffEq());
	else if(sortType == "goalsAgainst")
		rval = privateSort(rows, XkorSortGoalsAgainstGr(), XkorSortGoalsAgainstEq());
	else if(sortType == "goalsFor")
		rval = privateSort(rows, XkorSortGoalsForGr(), XkorSortGoalsForEq());
	else if(sortType == "h2hAwayGoals")
	{
		XkorSortH2HAwayGoalsGr gr(l);
		XkorSortH2HAwayGoalsEq eq(l);
		rval = privateSort(rows, gr, eq);
	}
	else if(sortType == "h2hGoalDifference")
	{
		XkorSortH2HGoalDiffGr gr(l);
		XkorSortH2HGoalDiffEq eq(l);
		rval = privateSort(rows, gr, eq);
	}
	else if(sortType == "h2hGoalsAgainst")
	{
		XkorSortH2HGoalsAgainstGr gr(l);
		XkorSortH2HGoalsAgainstEq eq(l);
		rval = privateSort(rows, gr, eq);
	}
	else if(sortType == "h2hGoalsFor")
	{
		XkorSortH2HGoalsForGr gr(l);
		XkorSortH2HGoalsForEq eq(l);
		rval = privateSort(rows, gr, eq);
	}
	else if(sortType == "h2hPoints")
	{
		XkorSortH2HPointsGr gr(l);
		XkorSortH2HPointsEq eq(l);
		rval = privateSort(rows, gr, eq);
	}
	else if(sortType == "h2hWins")
	{
		XkorSortH2HWinsGr gr(l);
		XkorSortH2HWinsEq eq(l);
		rval = privateSort(rows, gr, eq);
	}
	else if(sortType == "points")
	{
		XkorSortPointsGr gr(pointsForWin, pointsForDraw, pointsForLoss);
		XkorSortPointsEq eq(pointsForWin, pointsForDraw, pointsForLoss);
		rval = privateSort(rows, gr, eq);
	}
	else if(sortType == "winPercent")
		rval = privateSort(rows, XkorSortWinPercentGr(), XkorSortWinPercentEq());
	else if(sortType == "winPercentNFL")
		rval = privateSort(rows, XkorSortWinPercentNFLGr(), XkorSortWinPercentNFLEq());
	else if(sortType == "winPercentPure")
		rval = privateSort(rows, XkorSortWinPercentPureGr(), XkorSortWinPercentPureEq());
	else if(sortType == "wins")
		rval = privateSort(rows, XkorSortWinsGr(), XkorSortWinsEq());
	else
		rval.push_back(rows);
	return rval;
}

template <typename Comparator1, typename Comparator2>
std::vector<std::vector<XkorTableRow> > XkorTableSorter::privateSort(std::vector<XkorTableRow> rows, Comparator1 gr, Comparator2 eq)
{
	std::vector<std::vector<XkorTableRow> > rval;
	if(rows.size() > 1)
	{
		// sort and place into bins
		std::sort(rows.begin(), rows.end(), gr);
		rval.push_back(std::vector<XkorTableRow>(1, rows[0]));
		for(size_t i = 1; i < rows.size(); ++i)
		{
			if(eq(rows[i], rows[i - 1]))
				(--rval.end())->push_back(rows[i]);
			else
				rval.push_back(std::vector<XkorTableRow>(1, rows[i]));
		}
	}
	else if(rows.size() == 1)
		rval.push_back(std::vector<XkorTableRow>(1, rows[0]));
	return rval;
}
