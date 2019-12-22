#ifndef TIMEDRESULTCOMPARATOR_H
#define TIMEDRESULTCOMPARATOR_H

#include "paradigms/comparators/basicresultcomparator.h"

class XkorTimedResultComparator : public XkorBasicResultComparator
{
	public:
		XkorTimedResultComparator(QString type, QHash<QString, QVariant> opt) : XkorBasicResultComparator(type, opt)
		{
			if(type == "noTiebreakers")
				useTiebreakers = false;
			else
				useTiebreakers = true;

			if(opt.contains("interheatTiebreakerDigits"))
			{
				useRounding = true;
				roundingValue = pow(10.0, -opt.value("interheatTiebreakerDigits").toDouble());
			}
			else
				useRounding = false;
		}

		virtual bool operator() (XkorResult a, XkorResult b)
		{
			if(isAscending)
				return sortAscending(a, b);
			else
				return sortAscending(b, a);
		}

		static bool compareVariant(QVariant a, QVariant b)
		{
			return a.toDouble() < b.toDouble();
		}

		static bool compareVariantDescending(QVariant a, QVariant b)
		{
			return b.toDouble() < a.toDouble();
		}

		virtual double roundScore(double score, double rounding)
		{
			if(score == numeric_limits<double>::max() || score == -numeric_limits<double>::max())
				return score;
			else
				return (int)(score / rounding) * rounding;
		}

		virtual bool sortAscending(XkorResult a, XkorResult b)
		{
			bool roundingApplies = useRounding && a.value("heat") != b.value("heat");
			if(!roundingApplies && a.score() - b.score() < -0.0000000001) // a.score() < b.score — we have to use this ridiculous form due to rounding issues
				return true;
			else if(roundingApplies && roundScore(a.score(), roundingValue) < roundScore(b.score(), roundingValue))
				return true;
			else if(!roundingApplies && a.score() - b.score() > 0.0000000001) // a.score() > b.score()
				return false;
			else if(roundingApplies && roundScore(a.score(), roundingValue) > roundScore(b.score(), roundingValue))
				return false;
			else if(useTiebreakers && a.result.contains("attempts"))
			{
				QList<QVariant> attemptsA = a.value("attempts").toList(), attemptsB = b.value("attempts").toList();

				if(attemptsA.size() < attemptsB.size())
					return !isAscending; // having more attempts is always better regardless of what order the numbers go
				else if(attemptsA.size() > attemptsB.size())
					return isAscending;

				if(isAscending)
				{
					std::sort(attemptsA.begin(), attemptsA.end(), &XkorTimedResultComparator::compareVariant);
					std::sort(attemptsB.begin(), attemptsB.end(), &XkorTimedResultComparator::compareVariant);
				}
				else
				{
					std::sort(attemptsA.begin(), attemptsA.end(), &XkorTimedResultComparator::compareVariantDescending);
					std::sort(attemptsB.begin(), attemptsB.end(), &XkorTimedResultComparator::compareVariantDescending);
				}
				for(int i = 0; i < attemptsA.size() && i < attemptsB.size(); ++i)
				{
					if(!roundingApplies && attemptsA[i].toDouble() - attemptsB[i].toDouble() < -0.0000000001)
						return true;
					else if(roundingApplies && roundScore(attemptsA[i].toDouble(), roundingValue) < roundScore(attemptsB[i].toDouble(), roundingValue))
						return true;
					if(!roundingApplies && attemptsA[i].toDouble() - attemptsB[i].toDouble() > 0.0000000001)
						return false;
					else if(roundingApplies && roundScore(attemptsA[i].toDouble(), roundingValue) > roundScore(attemptsB[i].toDouble(), roundingValue))
						return false;
				}
				return false;
			}
			else
				return false;
		}

		virtual void sort(QVector<XkorResult> & res)
		{
			std::stable_sort(res.begin(), res.end(), *this);
		}

	protected:
		double roundingValue;
		bool useRounding, useTiebreakers;
};

#endif // TIMEDRESULTCOMPARATOR_H
