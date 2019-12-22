#ifndef FOOTBA11ERPARADIGM_H
#define FOOTBA11ERPARADIGM_H

#include "paradigms/abstracth2hparadigm.h"
#include "paradigms/options/footba11erparadigmoptions.h"

class XkorFootba11erParadigm : public XkorAbstractH2HParadigm
{
	public:
		XkorFootba11erParadigm() : XkorAbstractH2HParadigm() {;;;}
		XkorFootba11erParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractH2HParadigm(sport, userOptions) {;;;}

		virtual bool hasOptionsWidget() { return true; }

		virtual XkorAbstractAthleteWidget * newAthleteWidget()
		{
			return XkorAbstractParadigm::newAthleteWidget();
		}

		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> paradigmOptions)
		{
			return new XkorFootba11erParadigmOptions(paradigmOptions);
		}

	protected:
		virtual QPair<XkorResult, XkorResult> generateFTScore(XkorAthlete home, XkorAthlete away)
		{
			// get the parameters
			double homeAdvantage = 0;
			if(userOpt.value("homeAdvantage") == "true")
				homeAdvantage = opt.value("homeAdvantage", 0.065).toDouble();
			double skillCoeff = opt.value("skillCoeff", 0.6).toDouble();
			double skillOffset = opt.value("skillOffset", 0.2).toDouble();
			bool listAttackStyle = (opt.value("attackStyle", "list") == "list");
			int maxAttacks = opt.value("maxAttacks", 25).toInt();
			QList<QVariant> pointValues = opt.value("pointValues", QList<QVariant>() << 1).toList();
			QList<QVariant> pointProbs = opt.value("pointProbs", QList<QVariant>() << 1).toList();

			// if the home and away skill are both 0, we can’t calculate the ratio, and use 0.5 instead
			double skillRatio = (home.rpSkill == away.rpSkill ? 0.5 : home.rpSkill / (home.rpSkill + away.rpSkill));
			double homeSkill = skillCoeff * skillRatio + skillOffset + homeAdvantage;

			int attacks;
			if(listAttackStyle)
			{
				QList<QVariant> attackProbs = opt.value("attackProbs", QList<QVariant>() << 0.08 << 0.2 << 0.24 << 0.2 << 0.12 << 0.08 << 0.04 << 0.0224 << 0.0096 << 0.0048 << 0.0016 << 0.00112 << 0.00048).toList();
				double rand = s->randUniform();
				double acc = 0;
				for(attacks = 0; attacks < attackProbs.size(); ++attacks)
				{
					acc += attackProbs[attacks].toDouble();
					if(acc > rand)
						break;
				}
			}
			else // attackStyle == "normal"
			{
				double meanAttacks = opt.value("meanAttacks", 8.11).toDouble();
				double stDevAttacks = opt.value("stDevAttacks", 2.85).toDouble();
				double tailCutoff = opt.value("attacksTailCutoff", 0.005).toDouble();
				attacks = s->randGaussianCapped(tailCutoff) * stDevAttacks + meanAttacks + 0.5; // 0.5 for rounding
			}

			QList<QVariant> homeAttacks, awayAttacks;
			for(int i = 0; i < attacks && i < maxAttacks; ++i)
			{
				// how many points will be scored
				int attackValue = 0;
				double rand = s->randUniform();
				double acc = 0;
				for(int j = 0; j < pointValues.size() && j < pointProbs.size(); ++j)
				{
					acc += pointProbs[j].toDouble();
					if(acc > rand)
					{
						attackValue = pointValues[j].toInt();
						break;
					}
				}

				// who gets those points?
				if(s->randUniform() < homeSkill)
				{
					homeAttacks.push_back(attackValue);
					awayAttacks.push_back(0);
				}
				else
				{
					homeAttacks.push_back(0);
					awayAttacks.push_back(attackValue);
				}
			}

			// add up the scores
			int homeScore = 0, awayScore = 0;
			for(int i = 0; i < attacks; ++i)
			{
				homeScore += homeAttacks[i % maxAttacks].toInt();
				awayScore += awayAttacks[i % maxAttacks].toInt();
			}

			// add up the subscores
			QList<QVariant> homeSubScores, awaySubScores;
			for(int i = 0; i < pointValues.size(); ++i)
			{
				int homeSubScore = 0, awaySubScore = 0;
				for(int j = 0; j < attacks; ++j)
				{
					if(homeAttacks[j % maxAttacks].toInt() == pointValues[i].toInt())
						++homeSubScore;
					else if(awayAttacks[j % maxAttacks].toInt() == pointValues[i].toInt())
						++awaySubScore;
				}
				homeSubScores.push_back(homeSubScore);
				awaySubScores.push_back(awaySubScore);
			}


			XkorResult hRes, aRes;
			hRes.athlete = home;
			aRes.athlete = away;

			hRes.setScore(homeScore);
			aRes.setScore(awayScore);
			hRes.result["subScores"] = homeSubScores;
			aRes.result["subScores"] = awaySubScores;

			return qMakePair(hRes, aRes);
		}

		virtual QPair<XkorResult, XkorResult> generateGGScore(XkorResult home, XkorResult away, QString str)
		{
			QString scoreType = (str.isEmpty() ? "score" : str);
			QString subScoreType = (str.isEmpty() ? "subScores" : str + "SubScores");

			// get the parameters
			double homeAdvantage = 0;
			if(userOpt.value("homeAdvantage") == "true")
				homeAdvantage = opt.value("homeAdvantage", 0.065).toDouble();
			double skillCoeff = opt.value("skillCoeff", 0.6).toDouble();
			double skillOffset = opt.value("skillOffset", 0.2).toDouble();
			QList<QVariant> pointValues = opt.value("pointValues", QList<QVariant>() << 1).toList();
			QList<QVariant> pointProbs = opt.value("pointProbs", QList<QVariant>() << 1).toList();

			// if the home and away skill are both 0, we can’t calculate the ratio, and use 0.5 instead
			double skillRatio = (home.athlete.rpSkill == away.athlete.rpSkill ? 0.5 : home.athlete.rpSkill / (home.athlete.rpSkill + away.athlete.rpSkill));
			double homeSkill = skillCoeff * skillRatio + skillOffset + homeAdvantage;

			// how many points will be scored?
			int attackValue = 0;
			double rand = s->randUniform();
			double acc = 0;
			for(int i = 0; i < pointValues.size() && i < pointProbs.size(); ++i)
			{
				acc += pointProbs[i].toDouble();
				if(acc > rand)
				{
					attackValue = pointValues[i].toInt();
					break;
				}
			}

			// who gets those points?
			bool homeWin = false;
			if(s->randUniform() < homeSkill)
				homeWin = true;

			// add the score
			if(homeWin)
				home.result[scoreType] = home.result[scoreType].toInt() + attackValue;
			else
				away.result[scoreType] = away.result[scoreType].toInt() + attackValue;

			// add the subscore
			QList<QVariant> homeSubScores = home.value(subScoreType).toList(), awaySubScores = away.value(subScoreType).toList();
			for(int i = 0; i < pointValues.size(); ++i)
				if(pointValues[i].toInt() == attackValue)
				{
					if(homeWin)
						homeSubScores[i] = homeSubScores[i].toInt() + 1;
					else
						awaySubScores[i] = awaySubScores[i].toInt() + 1;
				}
			home.result[subScoreType] = homeSubScores;
			away.result[subScoreType] = awaySubScores;

			return qMakePair(home, away);
		}

		virtual int generateScore(double, double, double, double, bool, double)
		{
			return -1; // we don’t use this
		}

};

#endif // FOOTBA11ERPARADIGM_H
