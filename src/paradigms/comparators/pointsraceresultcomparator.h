#ifndef POINTSRACERESULTCOMPARATOR_H
#define POINTSRACERESULTCOMPARATOR_H

#include "paradigms/comparators/basicresultcomparator.h"

class XkorPointsRaceResultComparator : public XkorBasicResultComparator
{
	public:
		XkorPointsRaceResultComparator(QString type, QHash<QString, QVariant> opt) : XkorBasicResultComparator(type, opt) {;;;}

		virtual bool operator() (XkorResult a, XkorResult b)
		{
			if(a.score() > b.score())
				return true;
			else if(a.score() == b.score() && a.value("laps").toDouble() > b.value("laps").toDouble())
				return true;
			else if(a.score() == b.score() && a.value("laps") == b.value("laps") && a.value("time").toDouble() > b.value("time").toDouble())
				return true;
			else
				return false;
		}

		virtual void sort(QVector<XkorResult> & res)
		{
			std::sort(res.begin(), res.end(), *this);
		}
};

#endif // POINTSRACERESULTCOMPARATOR_H
