#ifndef SHOOTINGRESULTCOMPARATOR_H
#define SHOOTINGRESULTCOMPARATOR_H

#include "paradigms/comparators/basicresultcomparator.h"

class XkorShootingResultComparator : public XkorBasicResultComparator
{
	public:
		XkorShootingResultComparator(QString type, QHash<QString, QVariant> opt) : XkorBasicResultComparator(type, opt)
		{
			if(type == "qualifying")
				isQualifying = true;
			else
				isQualifying = false;
			if(opt.value("useBestGroups", "false") == "true")
				useBestGroups = true;
			else
				useBestGroups = false;
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
			else if(isQualifying && useBestGroups && a.score() == b.score())
			{ // figure out who had the best last group
				QList<QVariant> aAttempts = a.value("attempts").toList(), bAttempts = b.value("attempts").toList();
				for(int i = 0; i < aAttempts.size() && i < bAttempts.size(); ++i)
					if(aAttempts[aAttempts.size() - i - 1].toDouble() < bAttempts[bAttempts.size() - i - 1].toDouble())
						return true;
					else if(aAttempts[aAttempts.size() - i - 1].toDouble() > bAttempts[bAttempts.size() - i - 1].toDouble())
						return false;
			}
			if(a.score() == b.score())
			{
				QList<QVariant> aShootoffScores = a.value("shootoffScores").toList(), bShootoffScores = b.value("shootoffScores").toList();
				for(int i = 0; i < aShootoffScores.size() && i < bShootoffScores.size(); ++i)
					if(aShootoffScores[i].toDouble() < bShootoffScores[i].toDouble())
						return true;
					else if(aShootoffScores[i].toDouble() > bShootoffScores[i].toDouble())
						return false;
			}
			return false;
		}

	protected:
		bool isQualifying, useBestGroups;
};

#endif // SHOOTINGRESULTCOMPARATOR_H
