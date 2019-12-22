#ifndef HIGHJUMPRESULTCOMPARATOR_H
#define HIGHJUMPRESULTCOMPARATOR_H

#include "paradigms/comparators/basicresultcomparator.h"

class XkorHighJumpResultComparator : public XkorBasicResultComparator
{
	public:
		XkorHighJumpResultComparator(QString type, QHash<QString, QVariant> opt) : XkorBasicResultComparator(type, opt) {;;;}

		virtual bool operator() (XkorResult a, XkorResult b)
		{
			if(isAscending)
				return sortAscending(a, b);
			else
				return sortAscending(b, a);
		}

		virtual bool sortAscending(XkorResult a, XkorResult b)
		{
			if(a.score() < b.score())
				return true;
			else if(a.score() == b.score() && a.value("lastHeightFailures").toInt() > b.value("lastHeightFailures").toInt())
				return true;
			else if(a.score() == b.score() && a.value("lastHeightFailures") == b.value("lastHeightFailures") && a.value("failures").toInt() > b.value("failures").toInt())
				return true;
			else
				return false;
		}

		virtual void sort(QVector<XkorResult> & res)
		{
			std::sort(res.begin(), res.end(), *this);
		}

	protected:
		bool isRankingRound;
};

#endif // HIGHJUMPRESULTCOMPARATOR_H
