#ifndef FENCINGPARADIGM_H
#define FENCINGPARADIGM_H

#include "paradigms/abstractparadigm.h"
#include "paradigms/options/timedparadigmoptions.h"

class XkorFencingParadigm : public XkorAbstractParadigm
{
	public:
		XkorFencingParadigm() : XkorAbstractParadigm()
		{
			supportedCompetitions.insert("matches", true);
			supportedCompetitions.insert("roundRobin", true);
		}

		XkorFencingParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractParadigm(sport, userOptions)
		{
			supportedCompetitions.insert("matches", true);
			supportedCompetitions.insert("roundRobin", true);
		}

		virtual void breakTie(QList<XkorAthlete>)
		{
			qDebug() << "unexpected tie break request in fencing paradigm";
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

					double matchLength = opt.value("matchLength").toDouble();
					int winPoints = opt.value("winPoints").toInt();

					// calculate number of points
					double pointsRate = s->individualScore("pointRate");
					int pointsPlayed = static_cast<int>(matchLength / pointsRate);

					QPair<int, int> score;
					for(int i = 0; score.first < winPoints && score.second < winPoints && (score.first + score.second < pointsPlayed || score.first == score.second); ++i)
					{
						QPair<double, double> currentPointScore = generatePointScore(home->rpSkill, away->rpSkill);

						if(currentPointScore.first > currentPointScore.second)
							++score.first;
						else
							++score.second;
					}

					QString result = formatName(home->name, home->nation) + " " + formatScore(score.first, score.second) + " " + formatName(away->name, away->nation);

					out.append(qMakePair(home->name, result));
					res.append(XkorResult(score.first, *home));
					res.append(XkorResult(score.second, *away));
				}
				++acc;
			}
		}

	protected:
		virtual QString formatScore(int score1, int score2)
		{
			return QString::number(score1) + "–" + QString::number(score2);
		}

		virtual QPair<double, double> generatePointScore(double homeSkill, double awaySkill)
		{
			QPair<double, double> rval = qMakePair(0.0, 0.0);

			double attackModifier = opt.value("attackModifier").toDouble();

			rval.first = s->randWeighted(homeSkill, awaySkill) + s->randUniform() * attackModifier;
			rval.second = s->randWeighted(awaySkill, homeSkill) + s->randUniform() * attackModifier;

			return rval;
		}
};

#endif // FENCINGPARADIGM_H
