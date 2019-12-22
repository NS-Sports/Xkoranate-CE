#ifndef NSFSBASEBALLPARADIGM_H
#define NSFSBASEBALLPARADIGM_H

#include "paradigms/nsfsgridironparadigm.h"

class XkorNSFSBaseballParadigm : public XkorNSFSGridironParadigm
{
	protected:
		virtual QPair<XkorResult, XkorResult> generateETScore(XkorResult hRes, XkorResult aRes, QString)
		{
			QPair<int, int> result = generateScore(hRes.athlete.skill, aRes.athlete.skill);
			hRes.setScore(hRes.score() + result.first);
			aRes.setScore(aRes.score() + result.second);
			QList<QVariant> hResScores = hRes.result["subScores"].toList();
			QList<QVariant> aResScores = aRes.result["subScores"].toList();
			hResScores.append(result.first);
			aResScores.append(result.second);
			hRes.result["subScores"] = hResScores;
			aRes.result["subScores"] = aResScores;

			return qMakePair(hRes, aRes);
		}

		virtual QPair<XkorResult, XkorResult> generateFTScore(XkorAthlete away, XkorAthlete home)
		{
			// note that we’ve reversed the parameters, because the home team bats second
			XkorResult hRes, aRes;
			hRes.athlete = home;
			aRes.athlete = away;

			int periods = opt.value("periods").toInt();
			if(userOpt.value("usePeriods", "true") == "true")
			{
				for(int i = 0; i < periods; ++i)
				{
					QPair<int, int> result = generateScore(home.rpSkill, away.rpSkill);

					// is this the ninth inning?
					bool homeMustBat = true;
					if(i == periods - 1 && aRes.score() + result.second < hRes.score())
						homeMustBat = false;

					if(homeMustBat)
						hRes.setScore(hRes.score() + result.first);
					aRes.setScore(aRes.score() + result.second);
					QList<QVariant> hResScores = hRes.result["subScores"].toList();
					QList<QVariant> aResScores = aRes.result["subScores"].toList();
					if(homeMustBat)
						hResScores.append(result.first);
					aResScores.append(result.second);
					hRes.result["subScores"] = hResScores;
					aRes.result["subScores"] = aResScores;
				}
			}
			else
			{
				// because of the special ninth inning rule, we need to generate it separately
				QPair<int, int> result1 = generateScore(home.rpSkill, away.rpSkill, periods - 1);
				QPair<int, int> result2 = generateScore(home.rpSkill, away.rpSkill);

				if(result1.second + result2.second < result1.first)
					hRes.setScore(result1.first); // home did not bat in ninth inning
				else
					hRes.setScore(result1.first + result2.first);
				aRes.setScore(result1.second + result2.second);
			}

			return qMakePair(aRes, hRes);
		}

		virtual QPair<int, int> generateScore(double skill, double oppSkill, int attackMultiplier = 1)
		{
			// load constants
			double baseScoringProb = opt.value("baseScoringProb").toDouble();
			double baseScoringCoeff = opt.value("baseScoringCoeff").toDouble();
			double extraScoringProb = opt.value("extraScoringProb").toDouble();
			double extraScoringCoeff = opt.value("extraScoringCoeff").toDouble();

			double rankDiff = oppSkill - skill;

			// calculate base probability and multiplier
			double homeP = baseScoringProb + baseScoringCoeff * rankDiff;
			double awayP = baseScoringProb + baseScoringCoeff * -rankDiff;

			double homePMultiplier = extraScoringProb + extraScoringCoeff * rankDiff;
			double awayPMultiplier = extraScoringProb + extraScoringCoeff * -rankDiff;

			int homeScore = 0, awayScore = 0;

			for(int i = 0; i < attackMultiplier; ++i)
			{
				// home score
				double rand = s->randUniform();
				double acc = 0;
				double pJRuns = 1 - homeP; // probability of 0 runs
				for(int j = 0; ; ++j)
				{
					acc += pJRuns;
					if(rand < acc)
					{
						homeScore += j;
						break;
					}
					// calculate probability of i + 1 runs
					pJRuns = homeP * pow(homePMultiplier, j) * (1 - homePMultiplier);
				}

				// away score
				rand = s->randUniform();
				acc = 0;
				pJRuns = 1 - awayP; // probability of 0 runs
				for(int j = 0; ; ++j)
				{
					acc += pJRuns;
					if(rand < acc)
					{
						awayScore += j;
						break;
					}
					// calculate probability of i + 1 runs
					pJRuns = awayP * pow(awayPMultiplier, j) * (1 - awayPMultiplier);
				}
			}
			return qMakePair(awayScore, homeScore);
		}

};

#endif // NSFSBASEBALLPARADIGM_H
