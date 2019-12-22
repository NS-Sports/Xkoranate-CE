#ifndef NSFSGRIDIRONPARADIGM_H
#define NSFSGRIDIRONPARADIGM_H

#include "paradigms/abstracth2hparadigm.h"
#include "paradigms/options/nsfsgridironparadigmoptions.h"

class XkorNSFSGridironParadigm : public XkorAbstractH2HParadigm
{
	public:
		XkorNSFSGridironParadigm() : XkorAbstractH2HParadigm() {;;;}
		XkorNSFSGridironParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractH2HParadigm(sport, userOptions) {;;;}

		virtual bool hasOptionsWidget() { return true; }

		virtual XkorAbstractAthleteWidget * newAthleteWidget()
		{
			return XkorAbstractParadigm::newAthleteWidget();
		}

		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> paradigmOptions)
		{
			return new XkorNSFSGridironParadigmOptions(paradigmOptions);
		}

	protected:
		virtual int generateConversions(double count, int forceValue = -1)
		{
			int rval = 0;

			int conversionValue;
			double conversionProb;
			if(forceValue == 1) // third overtime
			{
				conversionValue = opt.value("otConversionValue").toInt();
				conversionProb = opt.value("otConversionProb").toDouble();
			}
			else
			{
				conversionValue = opt.value("conversionValue").toInt();
				conversionProb = opt.value("conversionProb").toDouble();
			}

			for(int i = 0; i < count; ++i)
			{
				double rand = s->randUniform();
				if(rand < conversionProb)
					rval += conversionValue;
			}

			return rval;
		}

		virtual QPair<XkorResult, XkorResult> generateFTScore(XkorAthlete home, XkorAthlete away)
		{
			XkorResult hRes, aRes;
			hRes.athlete = home;
			aRes.athlete = away;

			int periods = opt.value("periods").toInt();
			if(userOpt.value("usePeriods", "true") == "true")
			{
				for(int i = 0; i < periods; ++i)
				{
					QPair<int, int> result = generateScore(home.rpSkill, away.rpSkill);
					hRes.setScore(hRes.score() + result.first);
					aRes.setScore(aRes.score() + result.second);
					QList<QVariant> hResScores = hRes.result["subScores"].toList();
					QList<QVariant> aResScores = aRes.result["subScores"].toList();
					hResScores.append(result.first);
					aResScores.append(result.second);
					hRes.result["subScores"] = hResScores;
					aRes.result["subScores"] = aResScores;
				}
			}
			else
			{
				QPair<int, int> result = generateScore(home.rpSkill, away.rpSkill, periods);
				hRes.setScore(result.first);
				aRes.setScore(result.second);
			}

			return qMakePair(hRes, aRes);
		}

		virtual QPair<int, int> generateScore(double skill, double oppSkill, int attackMultiplier = 1)
		{
			// load constants
			QList<QVariant> pointValues = readOptionList("pointValues");
			QList<QVariant> scoringProbs = readOptionList("scoringProbs");
			QList<QVariant> scoringCoeffs = readOptionList("scoringCoeffs");
			double turnoverBase = opt.value("turnoverBase").toDouble();
			double turnoverCoeff = opt.value("turnoverCoeff").toDouble();
			double attacksBase = opt.value("attacksBase").toDouble();
			double attacksCoeff = opt.value("attacksCoeff").toDouble();

			double rankDiff = skill - oppSkill;

			int homeAttacks, awayAttacks;
			homeAttacks = attacksBase + attacksCoeff * rankDiff;
			awayAttacks = attacksBase + attacksCoeff * -rankDiff;
			homeAttacks *= attackMultiplier;
			awayAttacks *= attackMultiplier;

			// calculate P(score), P(turnover) on any given attack
			QList<double> homePScore, awayPScore;
			double homePAcc = 0, awayPAcc = 0;
			for(int i = 0; i < pointValues.size(); ++i)
			{
				double homeP = scoringProbs[i].toDouble() + scoringCoeffs[i].toDouble() * rankDiff;
				double awayP = scoringProbs[i].toDouble() + scoringCoeffs[i].toDouble() * -rankDiff;
				homePScore.append(homeP / (1 - homePAcc)); // probability of this score once we know
				awayPScore.append(awayP / (1 - awayPAcc)); // we haven’t gotten the previous scores
				homePAcc += homeP;
				awayPAcc += awayP;
			}
			double homePTurnover = (turnoverBase + turnoverCoeff * rankDiff) / (1 - homePAcc);
			double awayPTurnover = (turnoverBase + turnoverCoeff * -rankDiff) / (1 - awayPAcc);

			int homeScore = 0, awayScore = 0;
			while(homeAttacks > 0 || awayAttacks > 0)
			{
				// home scores
				for(int i = 0; i < pointValues.size(); ++i)
				{
					double rand = s->randUniform();
					double acc = 0;
					double pJScores = pow(1 - homePScore[i], homeAttacks); // probability of 0 goals
					for(int j = 0; j <= homeAttacks; ++j)
					{
						acc += pJScores;
						if(rand < acc)
						{
							homeScore += j * pointValues[i].toInt();
							qDebug() << j << rand << acc;
							homeAttacks -= j; // use the remaining attacks for other purposes
							if(i == 0)
								homeScore += generateConversions(j);
							break;
						}
						// calculate probability of j + 1 goals
						pJScores *= (homeAttacks - j) * homePScore[i] / ((j + 1) * (1 - homePScore[i]));
					}
				}
				// away scores
				for(int i = 0; i < pointValues.size(); ++i)
				{
					double rand = s->randUniform();
					double acc = 0;
					double pJScores = pow(1 - awayPScore[i], awayAttacks); // probability of 0 goals
					for(int j = 0; j <= awayAttacks; ++j)
					{
						acc += pJScores;
						if(rand < acc)
						{
							awayScore += j * pointValues[i].toInt();
							qDebug() << j << rand << acc;
							awayAttacks -= j; // use the remaining attacks for other purposes
							if(i == 0)
								awayScore += generateConversions(j);
							break;
						}
						// calculate probability of j + 1 goals
						pJScores *= (awayAttacks - j) * awayPScore[i] / ((j + 1) * (1 - awayPScore[i]));
					}
				}

				// home turnovers
				double rand = s->randUniform();
				double acc = 0;
				double pITurnovers = pow(1 - homePTurnover, homeAttacks); // probability of 0 turnovers
				for(int i = 0; i <= homeAttacks; ++i)
				{
					acc += pITurnovers;
					if(rand < acc)
					{
						awayAttacks = i; // give the other team turnovers
						break;
					}
					// calculate probability of i + 1 turnovers
					pITurnovers *= (homeAttacks - i) * homePTurnover / ((i + 1) * (1 - homePTurnover));
				}
				// away turnovers
				rand = s->randUniform();
				acc = 0;
				pITurnovers = pow(1 - awayPTurnover, awayAttacks); // probability of 0 turnovers
				for(int i = 0; i <= awayAttacks; ++i)
				{
					acc += pITurnovers;
					if(rand < acc)
					{
						homeAttacks = i; // give the other turnovers
						break;
					}
					// calculate probability of i + 1 turnovers
					pITurnovers *= (awayAttacks - i) * awayPTurnover / ((i + 1) * (1 - awayPTurnover));
				}
			}
			return qMakePair(homeScore, awayScore);
		}

		virtual int generateScore(double, double, double, double, bool, double)
		{
			return -1; // we don’t use this
		}

		virtual QPair<int, int> generateStyleModification(int, int, int, int)
		{
			return qMakePair(0, 0); // no style modifiers (yet?)
		}

};

#endif // NSFSGRIDIRONPARADIGM_H
