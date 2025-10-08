#ifndef ARCHERYPARADIGM_H
#define ARCHERYPARADIGM_H

#include "paradigms/abstractparadigm.h"
#include "paradigms/comparators/archeryresultcomparator.h"
#include "paradigms/options/timedparadigmoptions.h"

class XkorArcheryParadigm : public XkorAbstractParadigm
{
	public:
		XkorArcheryParadigm() : XkorAbstractParadigm()
		{
			supportedCompetitions.insert("archery", true);
			supportedCompetitions.insert("matches", true);
		}

		XkorArcheryParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractParadigm(sport, userOptions)
		{
			supportedCompetitions.insert("archery", true);
			supportedCompetitions.insert("matches", true);
		}

		virtual void breakTie(QList<XkorAthlete> athletes, QString type = QString())
		{	
			int maxScore = opt.value("maxScore", 0).toInt();
			bool scorePointForX = (opt.value("scorePointForX") == "true");
			if(type == "lots" || type == "closest")
			{
				for(QList<XkorAthlete>::iterator i = athletes.begin(); i != athletes.end(); ++i)
				{
					QVector<XkorResult>::iterator result = findResult(i->id);
					result->result["lots"] = s->randUniform();
					if(type == "closest")
						result->setOutput(result->output() + " (closest arrow)");
					else
						result->setOutput(result->output() + " (drawing of lots)");
				}
			}
			else if(userOpt.value("rankingRound") == "true")
			{
				for(QList<XkorAthlete>::iterator i = athletes.begin(); i != athletes.end(); ++i)
				{
					QVector<XkorResult>::iterator result = findResult(i->id);
					int score = 0;
					for(int j = 0; j < opt.value("arrowsPerTiebreak").toInt(); ++j)
					{
						if(maxScore != 0 && !scorePointForX)
							score += min(generateArrowScore(adjustSkill(i->skill)), maxScore);
						else
							score += generateArrowScore(adjustSkill(i->skill));
					}
					result->result["tbScore"] = result->result["tbScore"].toDouble() + score / pow(opt.value("maxScore").toDouble() * 2, result->result["tbArrows"].toDouble());
					result->result["tbArrows"] = result->result["tbArrows"].toDouble() + 1;
					if(result->output().right(1) == ")")
						result->setOutput(result->output().left(result->output().size() - 1) + " " + QString::number(score) + ")");
					else
						result->setOutput(result->output() + " (tiebreak: " + QString::number(score) + ")");

				}
			}
			else
			{
				int acc = 0;
				for(QList<XkorAthlete>::iterator away = athletes.begin(); away != athletes.end(); ++away)
				{
					if(acc & 1)
					{
						QList<XkorAthlete>::iterator home = away; // home = away - 1
						--home;

						int homeScore = 0, awayScore = 0, tries = 0;
						double homeLots = 0, awayLots = 0;
						while(homeScore == awayScore && tries < 3)
						{
							for(int i = 0; i < opt.value("arrowsPerTiebreak").toInt(); ++i)
							{
                                if(maxScore != 0 && !scorePointForX)
								{
									homeScore += min(generateArrowScore(adjustSkill(home->skill)), maxScore);
									awayScore += min(generateArrowScore(adjustSkill(away->skill)), maxScore);
								}
								else
								{
									homeScore += generateArrowScore(adjustSkill(home->skill));
									awayScore += generateArrowScore(adjustSkill(away->skill));
								}
							}
							++tries;
						}
						if(homeScore == awayScore)
						{
							homeLots = s->randUniform();
							awayLots = s->randUniform();
							findResult(home->id)->result["lots"] = homeLots;
							findResult(away->id)->result["lots"] = awayLots;
						}

						QVector<XkorResult>::iterator result = findResult(away->id);
						if(homeLots > awayLots)
							result->setOutput(result->output() + " (tiebreak: " + QString::number(homeScore) + "*–" + QString::number(awayScore) + ")");
						else if(awayLots > homeLots)
							result->setOutput(result->output() + " (tiebreak: " + QString::number(homeScore) + "–" + QString::number(awayScore) + "*)");
						else
							result->setOutput(result->output() + " (tiebreak: " + QString::number(homeScore) + "–" + QString::number(awayScore) + ")");

						// let the competition know who won
						findResult(home->id)->result["tbScore"] = homeScore;
						findResult(away->id)->result["tbScore"] = awayScore;
					}
					++acc;
				}
			}
			generateOutput();
		}

		virtual XkorAbstractResultComparator * comparisonFunction(QString type = QString())
		{
			if(userOpt.value("rankingRound") == "true")
				return new XkorArcheryResultComparator("rankingRound", opt);
			else
				return new XkorArcheryResultComparator(type, opt);
		}

		virtual bool hasOptionsWidget() { return true; }

		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> paradigmOptions)
		{
			return new XkorTimedParadigmOptions(paradigmOptions);
		}

		virtual void scorinate(QList<XkorAthlete> athletes, QList<XkorResult> = QList<XkorResult>())
		{
			for(QStringList::iterator i = requiredValues.begin(); i != requiredValues.end(); ++i)
				if(!(s->hasValue(*i)))
				{
					qDebug() << "missing parameter" << *i << "in XkorSQISParadigm::output(XkorResults *)";
					out.append(qMakePair(QString(""), QString("Sport does not support this paradigm")));
					return;
				}

			// load options
			int nameWidth = userOpt.value("nameWidth", 20).toInt() + 2;
			int resultWidth = opt.value("resultWidth", 3).toInt() + 2;

			bool isRankingRound = (userOpt.value("rankingRound") == "true");

			int arrows;
			if(isRankingRound)
				arrows = opt.value("rankingArrows").toInt();
			else
				arrows = opt.value("knockoutArrows").toInt();

			int maxScore = opt.value("maxScore").toInt();

			// initialize results
			out.clear();
			res.clear();

			int i = 0;
			QString homeScore;
			for(QList<XkorAthlete>::iterator athlete = athletes.begin(); athlete != athletes.end(); ++athlete)
			{
				int score = 0;
				int tens = 0;
				int Xs = 0;
				for(int j = 0; j < arrows; ++j)
				{
					int arrowScore = generateArrowScore(adjustSkill(athlete->rpSkill, isRankingRound));
					if(opt.contains("maxScore"))
					{
						if(opt.value("scorePointForX") == "true") // if an X counts at face value
							score += arrowScore;
						else // if an X counts the same as a 10
							score += min(maxScore, arrowScore);

						if(arrowScore >= maxScore)
							++tens;
						if(arrowScore > maxScore)
							++Xs;
					}
					else
						score += arrowScore;
				}

				QString result;
				if(userOpt.value("rankingRound") == "true")
				{
					result = formatName(*athlete).leftJustified(nameWidth);
					result += QString::number(score).rightJustified(resultWidth);
					if(opt.contains("maxScore"))
					{
						result += QString::number(tens).rightJustified(resultWidth);
						result += QString::number(Xs).rightJustified(resultWidth);
					}
				}
				else
				{
					if(i & 1)
						result = homeScore + "–" + QString::number(score) + " " + formatName(*athlete);
					else
						homeScore = formatName(*athlete) + " " + QString::number(score);
				}
				XkorResult r(score, *athlete);
				r.setOutput(result);
				r.result["tens"] = tens;
				r.result["Xs"] = Xs;
				res.append(r);
				++i;
			}
			generateOutput();
		}

	protected:
		virtual double adjustSkill(double realSkill, bool isRankingRound = false)
		{
			double rankModifier, rankAdjustmentFactor;
			if(isRankingRound)
			{
				rankModifier = opt.value("rankingRankModifier", 0).toDouble();
				rankAdjustmentFactor = opt.value("rankingRankAdjustmentFactor", 1).toDouble();
			}
			else
			{
				rankModifier = opt.value("knockoutRankModifier", 0).toDouble();
				rankAdjustmentFactor = opt.value("knockoutRankAdjustmentFactor", 1).toDouble();
			}

			// randomize the skill a bit to compensate for the fact that long sequences of arrows would otherwise be highly predictable
			double rval = (realSkill + s->randUniform() * rankModifier) / (1 + rankModifier);
			rval = rval * rankAdjustmentFactor + (1 - rankAdjustmentFactor) / 2;
			return rval;
		}

		virtual XkorResult individualResult(double, double) { return XkorResult(); } // unused
		virtual XkorResult individualResult(double, double, QString) { return XkorResult(); } // unused

		virtual int generateArrowScore(double skill)
		{
			return s->individualScore("score", skill);
		}
};

#endif // ARCHERYPARADIGM_H
