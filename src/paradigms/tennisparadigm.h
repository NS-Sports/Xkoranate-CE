#ifndef TENNISPARADIGM_H
#define TENNISPARADIGM_H

#include "paradigms/abstractparadigm.h"
#include "paradigms/options/timedparadigmoptions.h"

class XkorTennisParadigm : public XkorAbstractParadigm
{
	public:
		XkorTennisParadigm() : XkorAbstractParadigm()
		{
			supportedCompetitions.insert("matches", true);
			supportedCompetitions.insert("roundRobin", true);
		}

		XkorTennisParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractParadigm(sport, userOptions)
		{
			supportedCompetitions.insert("matches", true);
			supportedCompetitions.insert("roundRobin", true);
		}

		virtual void breakTie(QList<XkorAthlete>)
		{
			qDebug() << "unexpected tie break request in tennis paradigm";
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
			int resultWidth = opt.value("resultWidth", 2).toInt() + 2;

			// initialize results
			out.clear();
			res.clear();

			int acc = 0;
			for(QList<XkorAthlete>::iterator away = athletes.begin(); away != athletes.end(); ++away)
			{
				if(acc & 1) // if we’re on the second team in a match
				{
					QList<XkorAthlete>::iterator home = away; // home = away - 1
					--home;

					int sets = opt.value("sets").toInt();

					QPair<int, int> score;
					QList<QPair<int, int> > setScores;
					for(int i = 0; score.first <= sets / 2 && score.second <= sets / 2; ++i)
					{
						bool isFinalSet = (i == sets - 1);
						QPair<int, int> currentSetScore = generateSetScore(home->rpSkill, away->rpSkill, isFinalSet);
						setScores.append(currentSetScore);

						if(sets > 1)
						{
							if(currentSetScore.first > currentSetScore.second)
								++score.first;
							else
								++score.second;
						}
						else
							score = currentSetScore;
					}

					// output the home score
					QString homeResult = formatName(*home).leftJustified(nameWidth);
					for(QList<QPair<int, int> >::iterator i = setScores.begin(); i != setScores.end(); ++i)
						homeResult += QString::number(i->first).rightJustified(resultWidth);
					out.append(qMakePair(home->name, homeResult));
					res.append(XkorResult(score.first, *home));

					// output the away score
					QString awayResult = formatName(*away).leftJustified(nameWidth);
					for(QList<QPair<int, int> >::iterator i = setScores.begin(); i != setScores.end(); ++i)
						awayResult += QString::number(i->second).rightJustified(resultWidth);
					awayResult += "\n";
					out.append(qMakePair(away->name, awayResult));
					res.append(XkorResult(score.second, *away));
				}
				++acc;
			}
		}

	protected:
		virtual QPair<QString, double> individualResult(double, double) { return QPair<QString, double>(); } // unused
		virtual QPair<QString, double> individualResult(double, double, QString) { return QPair<QString, double>(); } // unused

		virtual QPair<int, int> generateSetScore(double homeSkill, double awaySkill, bool isFinalSet)
		{
			QPair<int, int> rval = qMakePair(0, 0);

			double attackModifier = opt.value("attackModifier").toDouble();
			// determine how many points we’re playing to and so forth
			int winPoints, winMargin, endPoints;
			if(isFinalSet)
			{
				winPoints = opt.value("finalSetWinPoints").toInt();
				winMargin = opt.value("finalSetWinMargin").toInt();
				endPoints = opt.value("finalSetEndPoints").toInt();
			}
			else
			{
				winPoints = opt.value("winPoints").toInt();
				winMargin = opt.value("winMargin").toInt();
				endPoints = opt.value("endPoints").toInt();
			}

			if(endPoints == 0) endPoints = numeric_limits<int>::max(); // if endPoints is set to 0, play until a two-point lead or the computer explodes!

			// …until someone wins, play points
			for(; rval.first < endPoints && rval.second < endPoints && (rval.first < winPoints || rval.first - rval.second < winMargin) && (rval.second < winPoints || rval.second - rval.first < winMargin);)
			{
				double homeRand = s->randWeighted(homeSkill, awaySkill) + s->randUniform() * attackModifier;
				double awayRand = s->randWeighted(awaySkill, homeSkill) + s->randUniform() * attackModifier;
				if(homeRand > awayRand)
					++rval.first;
				else
					++rval.second;
			}

			return rval;
		}
};

#endif
