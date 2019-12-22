#ifndef H2HPARADIGM_H
#define H2HPARADIGM_H

#include "paradigms/sqisparadigm.h"
#include "paradigms/options/timedparadigmoptions.h"

class XkorH2HParadigm : public XkorSQISParadigm
{
	public:
		XkorH2HParadigm() : XkorSQISParadigm()
		{
			requiredValues.clear();
			requiredValues << "score";
		}
		XkorH2HParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorSQISParadigm(sport, userOptions)
		{
			requiredValues.clear();
			requiredValues << "score";
		}

		virtual bool hasOptionsWidget() { return true; }

		virtual XkorAbstractAthleteWidget * newAthleteWidget()
		{
			return XkorAbstractParadigm::newAthleteWidget();
		}

		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> paradigmOptions)
		{
			return new XkorTimedParadigmOptions(paradigmOptions);
		}

	protected:
		virtual QPair<XkorResult, XkorResult> generateETScore(XkorResult home, XkorResult away, QString str)
		{
			QString scoreType = (str.isEmpty() ? "score" : str);
			home.result[scoreType] = home.value(scoreType).toDouble() + static_cast<int>(s->h2hScore("etScore", home.athlete.rpSkill, away.athlete.rpSkill));
			away.result[scoreType] = away.value(scoreType).toDouble() + static_cast<int>(s->h2hScore("etScore", away.athlete.rpSkill, home.athlete.rpSkill));

			return qMakePair(home, away);
		}

		virtual QPair<XkorResult, XkorResult> generateFTScore(XkorAthlete home, XkorAthlete away)
		{
			XkorResult hRes, aRes;
			hRes.athlete = home;
			aRes.athlete = away;

			QList<QVariant> statuses = readOptionList("statuses");
			QList<QVariant> statusProbs = readOptionList("statusProbs");

			for(int i = 0; i < statuses.size(); ++i)
			{
				if(s->randUniform() < statusProbs[i].toDouble()) // HOMETEAMISTRY FAIL
				{
					hRes.setScore(opt.value("sortOrder", "ascending") == "ascending" ? numeric_limits<double>::max() : -numeric_limits<double>::max());
					hRes.result["status"] = statuses[i].toString();
					aRes.setScore(opt.value("sortOrder", "ascending") == "ascending" ? -numeric_limits<double>::max() : numeric_limits<double>::max());
					return qMakePair(hRes, aRes);
				}
				else if(s->randUniform() < statusProbs[i].toDouble()) // AWAYTEAMISTRY FAIL
				{
					aRes.setScore(opt.value("sortOrder", "ascending") == "ascending" ? numeric_limits<double>::max() : -numeric_limits<double>::max());
					aRes.result["status"] = statuses[i].toString();
					hRes.setScore(opt.value("sortOrder", "ascending") == "ascending" ? -numeric_limits<double>::max() : numeric_limits<double>::max());
					return qMakePair(hRes, aRes);
				}
			}

			hRes.setScore(static_cast<int>(s->h2hScore("score", home.rpSkill, away.rpSkill)));
			aRes.setScore(static_cast<int>(s->h2hScore("score", away.rpSkill, home.rpSkill)));

			return qMakePair(hRes, aRes);
		}

		virtual QPair<XkorResult, XkorResult> generateGGScore(XkorResult home, XkorResult away, QString str)
		{
			double goldenGoalProb = opt.value("goldenGoalProb").toDouble();

			QString scoreType = (str.isEmpty() ? "score" : str);

			if(s->randUniform() < goldenGoalProb) // someone scored; who was it?
			{
				double otScore1 = s->randWeighted(home.athlete.rpSkill, away.athlete.rpSkill);
				double otScore2 = s->randWeighted(away.athlete.rpSkill, home.athlete.rpSkill);

				if(otScore1 > otScore2)
					home.result[scoreType] = home.value(scoreType).toDouble() + 1;
				else
					away.result[scoreType] = away.value(scoreType).toDouble() + 1;
			}
			return qMakePair(home, away);
		}
};

#endif // H2HPARADIGM_H
