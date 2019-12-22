#ifndef ARCHERYRESULTCOMPARATOR_H
#define ARCHERYRESULTCOMPARATOR_H

#include "paradigms/comparators/basicresultcomparator.h"

class XkorArcheryResultComparator : public XkorBasicResultComparator
{
	public:
		XkorArcheryResultComparator(QString type, QHash<QString, QVariant> opt) : XkorBasicResultComparator(type, opt)
		{
			if(type == "rankingRound")
				isRankingRound = true;
			else
				isRankingRound = false;
		}

		virtual bool operator() (XkorResult a, XkorResult b)
		{
			if(isAscending)
				return sortAscending(a, b);
			else
				return sortAscending(b, a);
		}

		virtual void sort(QVector<XkorResult> & res)
		{
			std::sort(res.begin(), res.end(), *this);
		}

		virtual bool sortAscending(XkorResult a, XkorResult b)
		{
			if(a.score() < b.score())
				return true;
			else if(isRankingRound && a.score() == b.score() && a.value("tens").toDouble() < b.value("tens").toDouble())
				return true;
			else if(isRankingRound && a.score() == b.score() && a.value("tens") == b.value("tens") && a.value("Xs").toDouble() < b.value("Xs").toDouble())
				return true;
			else if(a.score() == b.score() && (!isRankingRound || (a.value("tens") == b.value("tens") && a.value("Xs") == b.value("Xs"))) && a.value("tbScore").toDouble() < b.value("tbScore").toDouble())
				return true;
			else if(a.score() == b.score() && (!isRankingRound || (a.value("tens") == b.value("tens") && a.value("Xs") == b.value("Xs"))) && a.value("tbScore") == b.value("tbScore") && a.value("lots").toDouble() < b.value("lots").toDouble())
				return true;
			else
				return false;
		}

	protected:
		bool isRankingRound;
};

#endif // ARCHERYRESULTCOMPARATOR_H
