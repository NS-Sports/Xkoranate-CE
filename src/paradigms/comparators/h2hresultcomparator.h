#ifndef H2HRESULTCOMPARATOR_H
#define H2HRESULTCOMPARATOR_H

#include "paradigms/comparators/basicresultcomparator.h"

class XkorH2HResultComparator : public XkorBasicResultComparator
{
	public:
		XkorH2HResultComparator(QString type, QHash<QString, QVariant> opt) : XkorBasicResultComparator(type, opt)
		{
			tb = readOptionList(opt, "tiebreakerNames");
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
			else if(a.score() == b.score() && b.result.contains("decision"))
				return true;
			else if(a.score() == b.score() && a.result.contains("decision"))
				return false;
			else if(a.score() == b.score())
			{
				for(QList<QVariant>::iterator i = tb.begin(); i != tb.end(); ++i)
					if(a.value(i->toString()).isNull() && !b.value(i->toString()).isNull())
						return true;
					else if(!a.value(i->toString()).isNull() && b.value(i->toString()).isNull())
						return false;
					else if(a.value(i->toString()).toDouble() < b.value(i->toString()).toDouble())
						return true;
					else if(a.value(i->toString()).toDouble() > b.value(i->toString()).toDouble())
						return false;
			}
			return false;
		}

	protected:
		QList<QVariant> tb;
};

#endif // H2HRESULTCOMPARATOR_H
