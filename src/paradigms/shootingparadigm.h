#ifndef SHOOTINGPARADIGM_H
#define SHOOTINGPARADIGM_H

#include "paradigms/scoredparadigm.h"
#include "paradigms/comparators/shootingresultcomparator.h"
#include "paradigms/options/timedparadigmoptions.h"

class XkorShootingParadigm : public XkorScoredParadigm
{
	public:
		XkorShootingParadigm() : XkorScoredParadigm()
		{
			supportedCompetitions.clear();
			supportedCompetitions.insert("shooting", true);
		}

		XkorShootingParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorScoredParadigm(sport, userOptions)
		{
			supportedCompetitions.clear();
			supportedCompetitions.insert("shooting", true);
		}

		virtual void breakTie(QList<XkorAthlete> athletes, QString = QString())
		{
			for(QList<XkorAthlete>::iterator i = athletes.begin(); i != athletes.end(); ++i)
			{
				QVector<XkorResult>::iterator result = findResult(i->id);

				XkorAthlete ath = *i;
				ath.rpSkill = adjustSkill(ath.rpSkill, "shootoff");
				XkorResult soResult = individualResult(ath, "shootoffScore", "shootoff");

				QList<QVariant> shootoffScores = result->value("shootoffScores").toList();
				shootoffScores.append(soResult.score());
				result->result["shootoffScores"] = shootoffScores;
				result->result["shootoffShots"] = result->result["shootoffShots"].toDouble() + 1;
				if(result->output().right(1) == ")")
					result->setOutput(result->output().left(result->output().size() - 1) + " " + formatShootoffScore(soResult.score()) + ")");
				else
					result->setOutput(result->output() + " (shoot-off: " + formatShootoffScore(soResult.score()) + ")");

			}
			generateOutput();
		}

		virtual int compare(XkorResult a, XkorResult b, QString type = QString())
		{
			XkorAbstractResultComparator * f = comparisonFunction(type);
			if((*f)(a, b)) // if(a < b)
				return -1;
			else if((*f)(b, a)) // if(b < a)
				return 1;
			else
				return 0;
		}

		virtual XkorAbstractResultComparator * comparisonFunction(QString type = QString())
		{
			if(userOpt.value("qualifying") == "true")
				return new XkorShootingResultComparator("qualifying", opt);
			else
				return new XkorShootingResultComparator(type, opt);
		}

		virtual void scorinate(QList<XkorAthlete> athletes, QList<XkorResult> previousResults = QList<XkorResult>())
		{
			for(QStringList::iterator i = requiredValues.begin(); i != requiredValues.end(); ++i)
				if(!(s->hasValue(*i)))
				{
					qDebug() << "missing parameter" << *i << "in XkorShootingParadigm::output(XkorResults *)";
					out.append(qMakePair(QString(""), QString("Sport does not support this paradigm")));
					return;
				}

			// load options
			bool isQualifying = (userOpt.value("qualifying") == "true");

			int attempts;
			QString attemptType, attemptTypes, roundingType;
			if(isQualifying)
			{
				attempts = opt.value("qualifyingAttempts").toInt();
				attemptType = "qualifyingScore";
				attemptTypes = "qualifyingAttemptTypes";
				roundingType = "qualifying";
			}
			else
			{
				attempts = opt.value("finalAttempts").toInt();
				attemptType = "finalScore";
				attemptTypes = "finalAttemptTypes";
				roundingType = "final";
			}

			// initialize results
			out.clear();
			res.clear();

			for(QList<XkorAthlete>::iterator i = athletes.begin(); i != athletes.end(); ++i)
			{
				XkorResult r;
				r.athlete = *i;

				// load up the previous result, if any
				for(QList<XkorResult>::iterator j = previousResults.begin(); j != previousResults.end(); ++j)
					if(j->athlete == *i)
					{
						QList<QVariant> attempts = r.result["attempts"].toList(), attemptStrings = r.result["attemptStrings"].toList();
						attempts.append(j->score());
						attemptStrings.append(j->scoreString());
						r.result["attempts"] = attempts;
						r.result["attemptStrings"] = attemptStrings;
					}

				for(int j = 0; j < attempts; ++j)
				{
					XkorResult result;

					// generate the result with a modified skill
					XkorAthlete ath = *i;
					ath.rpSkill = adjustSkill(ath.rpSkill, attemptType);
					if(opt.contains(attemptTypes))
						result = individualResult(ath, readOptionList(attemptTypes).at(j).toString());
					else
						result = individualResult(ath, attemptType, roundingType);

					QList<QVariant> attempts = r.result["attempts"].toList(), attemptStrings = r.result["attemptStrings"].toList();
					attempts.append(result.score());
					attemptStrings.append(result.scoreString());
					r.result["attempts"] = attempts;
					r.result["attemptStrings"] = attemptStrings;
				}
				calculateTotal(r);

				res.append(r);
			}

			generateOutput();
		}

	protected:
		virtual double adjustSkill(double realSkill, QString roundType)
		{
			double rankModifier = opt.value(roundType + "RankModifier", 0).toDouble(), rankAdjustmentFactor = opt.value(roundType + "RankAdjustmentFactor", 1).toDouble();

			// randomize the skill a bit to compensate for the fact that long sequences of arrows would otherwise be highly predictable
			double rval = (realSkill + s->randUniform() * rankModifier) / (1 + rankModifier);
			rval = rval * rankAdjustmentFactor + (1 - rankAdjustmentFactor) / 2;
			return rval;
		}

		QString formatScore(double score)
		{
			bool isQualifying = (userOpt.value("qualifying") == "true");
			int displayDigits = (isQualifying ? opt.value("qualifyingDisplayDigits").toInt() : opt.value("finalDisplayDigits").toInt());
			return QString::number(score, 'f', displayDigits);
		}

		QString formatShootoffScore(double score)
		{
			int displayDigits = opt.value("shootoffDisplayDigits").toInt();
			return QString::number(score, 'f', displayDigits);
		}

		virtual QString outputLine(XkorResult r)
		{
			bool isQualifying = (userOpt.value("qualifying") == "true");

			int nameWidth = userOpt.value("nameWidth", 20).toInt() + 2;
			int resultWidth;
			if(isQualifying)
				resultWidth = opt.value("resultWidth", 3).toInt() + 2;
			else
				resultWidth = opt.value("resultWidth", 5).toInt() + 2;

			QString rval = formatName(r.athlete.name, r.athlete.nation).leftJustified(nameWidth);
			QList<QVariant> attemptStrings = r.value("attemptStrings").toList();
			for(QList<QVariant>::iterator i = attemptStrings.begin(); i != attemptStrings.end(); ++i)
				rval += i->toString().rightJustified(resultWidth);

			if(isQualifying)
			{
				if(opt.value("qualifyingAttempts").toInt() > 1)
					rval += r.scoreString().rightJustified(2 + resultWidth * (opt.value("qualifyingAttempts").toInt() - r.value("attempts").toList().size() + 1));
			}
			else
				rval += r.scoreString().rightJustified(2 + resultWidth * (opt.value("finalAttempts").toInt() - r.value("attempts").toList().size() + 2));

			return rval;
		}
};

#endif // SHOOTINGPARADIGM_H
