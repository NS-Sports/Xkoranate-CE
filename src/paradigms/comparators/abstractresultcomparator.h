#ifndef ABSTRACTRESULTCOMPARATOR_H
#define ABSTRACTRESULTCOMPARATOR_H

#include "result.h"

class XkorAbstractResultComparator
{
	public:
		XkorAbstractResultComparator(QString, QHash<QString, QVariant>) {;;;}

		virtual bool operator() (XkorResult a, XkorResult b) = 0;

		virtual bool isRankable(XkorResult r)
		{
			if(r.score() == numeric_limits<double>::max() || r.score() == -numeric_limits<double>::max())
				return false;
			else
				return true;
		}

		QList<QVariant> readOptionList(QHash<QString, QVariant> opt, QString name)
		{
			QVariant val = opt.value(name);
			if(val.type() == QVariant::List)
				return val.toList();
			else
				return QList<QVariant>() << val; // create a single-item list
		}

		virtual void sort(QVector<XkorResult> & res) = 0;
};

#endif // ABSTRACTRESULTCOMPARATOR_H
