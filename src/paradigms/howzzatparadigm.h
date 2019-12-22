#ifndef HOWZZATPARADIGM_H
#define HOWZZATPARADIGM_H

#include "paradigms/abstracth2hparadigm.h"
#include "paradigms/options/howzzatparadigmoptions.h"

class XkorHowzzatParadigm : public XkorAbstractH2HParadigm
{
	public:
		XkorHowzzatParadigm() : XkorAbstractH2HParadigm() {;;;}
		XkorHowzzatParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractH2HParadigm(sport, userOptions) {;;;}

		virtual bool hasOptionsWidget() { return true; }

		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> paradigmOptions)
		{
			return new XkorHowzzatParadigmOptions(paradigmOptions);
		}

	protected:
		virtual QPair<XkorResult, XkorResult> generateFTScore(XkorAthlete home, XkorAthlete away)
		{
			// get the parameters
			double homeAdvantage = 0;
			if(userOpt.value("homeAdvantage") == "true")
				homeAdvantage = opt.value("homeAdvantage", 0.065).toDouble();
			bool useStyleMods = userOpt.value("useStyleMods", "true") == "true";
			double skillCoeff = opt.value("skillCoeff", 0.6).toDouble();
			double skillOffset = opt.value("skillOffset", 0.2).toDouble();
			double styleCoeff = opt.value("styleCoeff", 0.04).toDouble();
			int maxOvers = opt.value("maxOvers", 50).toInt();
			int maxBalls = opt.value("ballsPerOver", 6).toInt() * maxOvers;
			int maxWickets = opt.value("maxWickets", 10).toInt();
			QList<QVariant> meanRunRate = opt.value("meanRunRate", QList<QVariant>() << 3.52 << 4.33 << 4.88 << 5.47 << 6.55).toList();
			QList<QVariant> stDevRunRate = opt.value("stDevRunRate", QList<QVariant>() << 0.39 << 0.17 << 0.21 << 0.17 << 0.69).toList();
			QList<QVariant> meanWicketRate = opt.value("meanWicketRate", QList<QVariant>() << 23.63 << 29.31 << 35.45 << 47.37 << 96.6).toList();
			QList<QVariant> stDevWicketRate = opt.value("stDevWicketRate", QList<QVariant>() << 3.96 << 0.92 << 2.24 << 4.5 << 21).toList();
			int runRateSize = (meanRunRate.size() > stDevRunRate.size() ? stDevRunRate.size() : meanRunRate.size());
			int wicketRateSize = (meanWicketRate.size() > stDevWicketRate.size() ? stDevWicketRate.size() : meanWicketRate.size());

			// if the home and away skill are both 0, we can’t calculate the ratio, and use 0.5 instead
			double skillRatio = (home.rpSkill == away.rpSkill ? 0.5 : home.rpSkill / (home.rpSkill + away.rpSkill));
			double homeSkill = skillCoeff * skillRatio + skillOffset + homeAdvantage;
			double awaySkill = 1 - homeSkill;

			bool homeGetsStyleEffect = (s->randUniform() < homeSkill);
			bool homeBatsFirst = (s->randUniform() < 0.5);

			int homeRunRateModifier = s->randUniform() * (runRateSize - 1) + (s->randUniform() < homeSkill ? 1 : 0);
			int awayRunRateModifier = s->randUniform() * (runRateSize - 1) + (s->randUniform() < awaySkill ? 1 : 0);
			int homeWicketRateModifier = s->randUniform() * (wicketRateSize - 1) + 1 - (s->randUniform() < awaySkill ? 1 : 0);
			int awayWicketRateModifier = s->randUniform() * (wicketRateSize - 1) + 1 - (s->randUniform() < homeSkill ? 1 : 0);

			double homeRunRate = s->randGaussian() * stDevRunRate.at(homeRunRateModifier).toDouble() + meanRunRate.at(homeRunRateModifier).toDouble();
			double awayRunRate = s->randGaussian() * stDevRunRate.at(awayRunRateModifier).toDouble() + meanRunRate.at(awayRunRateModifier).toDouble();
			double homeWicketRate = s->randGaussian() * stDevWicketRate.at(homeWicketRateModifier).toDouble() + meanWicketRate.at(homeWicketRateModifier).toDouble();
			double awayWicketRate = s->randGaussian() * stDevWicketRate.at(awayWicketRateModifier).toDouble() + meanWicketRate.at(awayWicketRateModifier).toDouble();

			if(useStyleMods)
			{
				double homeStyleValue = (home.property("style").toDouble() + 5) / 2;
				double awayStyleValue = (away.property("style").toDouble() + 5) / 2;

				if(homeGetsStyleEffect)
				{
					homeRunRate *= 1 + styleCoeff * homeStyleValue;
					homeWicketRate *= 1 + styleCoeff * (5 - awayStyleValue);
				}
				else
				{
					awayRunRate *= 1 + styleCoeff * awayStyleValue;
					awayWicketRate *= 1 + styleCoeff * (5 - homeStyleValue);
				}
			}

			double baseHomeRuns = homeRunRate * maxOvers;
			double baseAwayRuns = awayRunRate * maxOvers;
			double homeWickets = maxBalls / homeWicketRate;
			double awayWickets = maxBalls / awayWicketRate;

			double homeRuns = (int)(baseHomeRuns * (homeWickets > maxWickets ? maxWickets / homeWickets : 1));
			double awayRuns = (int)(baseAwayRuns * (awayWickets > maxWickets ? maxWickets / awayWickets : 1));
			homeWickets = (int)(homeWickets < maxWickets ? homeWickets : maxWickets);
			awayWickets = (int)(awayWickets < maxWickets ? awayWickets : maxWickets);

			// if the team batting second won, they couldn’t have won by many runs
			int winningRuns = s->randUniform() * 4 + 1;
			if(homeBatsFirst && awayRuns > homeRuns + winningRuns)
				awayRuns = homeRuns + winningRuns;
			else if(!homeBatsFirst && homeRuns > awayRuns + winningRuns)
				homeRuns = awayRuns + winningRuns;

			// if the team batting second won, they couldn’t have been all out
			if(homeBatsFirst && awayRuns > homeRuns && awayWickets > maxWickets - 1)
				awayWickets = maxWickets - 1;
			else if(!homeBatsFirst && homeRuns > awayRuns && homeWickets > maxWickets - 1)
				homeWickets = maxWickets - 1;

			int homeBalls = maxBalls, awayBalls = maxBalls;
			if(homeWickets == maxWickets || (!homeBatsFirst && homeRuns > awayRuns))
				homeBalls *= homeRuns / baseHomeRuns;
			if(awayWickets == maxWickets || (homeBatsFirst && awayRuns > homeRuns))
				awayBalls *= awayRuns / baseAwayRuns;

			XkorResult hRes, aRes;
			hRes.athlete = home;
			aRes.athlete = away;

			hRes.setScore(homeRuns);
			aRes.setScore(awayRuns);
			hRes.result["wickets"] = homeWickets;
			aRes.result["wickets"] = awayWickets;
			hRes.result["balls"] = homeBalls;
			aRes.result["balls"] = awayBalls;

			return qMakePair(hRes, aRes);
		}

		virtual int generateScore(double, double, double, double, bool, double)
		{
			return -1; // we don’t use this
		}

		virtual QString outputLine(XkorResult res)
		{
			int maxWickets = opt.value("maxWickets", 10).toInt();
			int ballsPerOver = opt.value("ballsPerOver", 6).toInt();

			QString rval = res.athlete.name + " ";
			rval += QString::number(res.score());
			if(res.value("wickets").toInt() < maxWickets)
				rval += "/" + res.value("wickets").toString();
			int balls = res.value("balls").toInt();
			if(balls % ballsPerOver == 0)
				rval += " (" + QString::number(balls / ballsPerOver) + " overs)";
			else
				rval += " (" + QString::number(balls / ballsPerOver) + "." + QString::number(balls % ballsPerOver) + " overs)";
			return rval;
		}

		virtual QString outputLine(XkorResult home, XkorResult away)
		{
			return outputLine(home) + "\n" + outputLine(away) + "\n";
		}

};

#endif // HOWZZATPARADIGM_H
