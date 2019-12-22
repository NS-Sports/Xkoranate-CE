#ifndef BESTOFPARADIGM_H
#define BESTOFPARADIGM_H

#include "paradigms/abstractparadigm.h"
#include "paradigms/options/timedparadigmoptions.h"

class XkorBestOfParadigm : public XkorAbstractParadigm
{
	public:
		XkorBestOfParadigm() : XkorAbstractParadigm()
		{
			supportedCompetitions.insert("matches", true);
			supportedCompetitions.insert("roundRobin", true);
		}

		XkorBestOfParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractParadigm(sport, userOptions)
		{
			supportedCompetitions.insert("matches", true);
			supportedCompetitions.insert("roundRobin", true);
		}

		virtual void breakTie(QList<XkorAthlete>)
		{
			qDebug() << "unexpected tie break request in best of paradigm";
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
					qDebug() << "missing parameter" << *i << "in XkorBestOfParadigm::output(XkorResults *)";
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

					int games = opt.value("games").toInt();

					QPair<int, int> score;
					for(int i = 0; score.first <= games / 2 && score.second <= games / 2; ++i)
					{
						QPair<double, double> currentGameScore = generateGameScore(home->rpSkill, away->rpSkill);

						if(currentGameScore.first > currentGameScore.second)
							++score.first;
						else
							++score.second;
					}

					QString result;
					if(games > 1)
						result = formatName(home->name, home->nation) + " " + formatScore(score.first, score.second) + " " + formatName(away->name, away->nation);
					else if(score.first > score.second) // if there’s only one game, show “def.”/“def. by”
						result = formatName(home->name, home->nation) + " def. " + formatName(away->name, away->nation);
					else
						result = formatName(home->name, home->nation) + " def. by " + formatName(away->name, away->nation);

					out.append(qMakePair(home->name, result));
					res.append(XkorResult(score.first, *home));
					res.append(XkorResult(score.second, *away));
				}
				++acc;
			}
		}

	protected:
		virtual XkorResult individualResult(double, double) { return XkorResult(); } // unused
		virtual XkorResult individualResult(double, double, QString) { return XkorResult(); } // unused

		virtual QString formatScore(int score1, int score2)
		{
			return QString::number(score1) + "–" + QString::number(score2);
		}

		virtual QPair<double, double> generateGameScore(double homeSkill, double awaySkill)
		{
			QPair<double, double> rval = qMakePair(0.0, 0.0);

			double attackModifier = opt.value("attackModifier").toDouble();

			rval.first = s->randWeighted(homeSkill, awaySkill) + s->randUniform() * attackModifier;
			rval.second = s->randWeighted(awaySkill, homeSkill) + s->randUniform() * attackModifier;

			return rval;
		}
};

#endif // BESTOFPARADIGM_H
