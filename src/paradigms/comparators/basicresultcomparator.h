#ifndef BASICRESULTCOMPARATOR_H
#define BASICRESULTCOMPARATOR_H

#include <algorithm>
#include "paradigms/comparators/abstractresultcomparator.h"

class XkorBasicResultComparator : public XkorAbstractResultComparator
{
	public:
		XkorBasicResultComparator(QString type, QHash<QString, QVariant> opt) : XkorAbstractResultComparator(type, opt)
		{
			if(opt.value("sortOrder", "ascending") == "ascending")
				isAscending = true;
			else
				isAscending = false;
		}

		virtual bool operator() (XkorResult a, XkorResult b)
		{
			if(isAscending)
				return (a.score() < b.score());
			else
				return (b.score() < a.score());
		}

		virtual void sort(QVector<XkorResult> & res)
		{
			std::sort(res.begin(), res.end(), *this);
		}
	protected:
		bool isAscending;
};

#endif // BASICRESULTCOMPARATOR_H
