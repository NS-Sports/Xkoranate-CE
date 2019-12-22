#ifndef ELIMINATIONRACERESULTCOMPARATOR_H
#define ELIMINATIONRACERESULTCOMPARATOR_H

#include "paradigms/comparators/basicresultcomparator.h"

class XkorEliminationRaceResultComparator : public XkorBasicResultComparator
{
	public:
		XkorEliminationRaceResultComparator(QString type, QHash<QString, QVariant> opt) : XkorBasicResultComparator(type, opt) {;;;}

		virtual bool operator() (XkorResult a, XkorResult b)
		{
			if(a.contains("eliminationOrder") && !b.contains("eliminationOrder"))
				return false;
			else if(!a.contains("eliminationOrder") && b.contains("eliminationOrder"))
				return true;
			else if(a.contains("eliminationOrder") && b.contains("eliminationOrder"))
				return (a.value("eliminationOrder").toInt() > b.value("eliminationOrder").toInt());
			else if(a.contains("points") && !b.contains("points"))
				return true;
			else if(!a.contains("points") && b.contains("points"))
				return false;
			else if(a.contains("points") && b.contains("points") && a.value("points").toInt() != b.value("points").toInt())
				return (a.value("points").toInt() > b.value("points").toInt());
			else
				return (a.score() < b.score());
		}

		virtual void sort(QVector<XkorResult> & res)
		{
			std::sort(res.begin(), res.end(), *this);
		}
};

#endif // ELIMINATIONRACERESULTCOMPARATOR_H
