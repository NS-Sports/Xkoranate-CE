#ifndef NSFSPARADIGM_H
#define NSFSPARADIGM_H

#include "paradigms/abstracth2hparadigm.h"
#include "paradigms/options/nsfsparadigmoptions.h"

class XkorNSFSParadigm : public XkorAbstractH2HParadigm
{
	public:
		XkorNSFSParadigm() : XkorAbstractH2HParadigm() {;;;}
		XkorNSFSParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractH2HParadigm(sport, userOptions) {;;;}

		virtual bool hasOptionsWidget() { return true; }

		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> paradigmOptions)
		{
			return new XkorNSFSParadigmOptions(paradigmOptions);
		}

	protected:
		virtual int generateScore(double skill, double oppSkill, double, double, bool homeAdvantage = false, double attackMultiplier = 1)
		{
			// load constants
			double baseAttackCoeff = opt.value("baseAttackCoeff").toDouble();
			double rankDiffModifier = opt.value("rankDiffModifier").toDouble();
			double rankCoeff = opt.value("rankCoeff").toDouble();
			double rankScalar = opt.value("rankScalar").toDouble();
			double bast = opt.value("baseAttackSuccessThreshold").toDouble();
			int baseAttacksSuperior = opt.value("baseAttacksSuperior").toInt();
			int baseAttacksInferior = opt.value("baseAttacksInferior").toInt();
			double attackCoeffSuperior = opt.value("attackCoeffSuperior").toDouble();
			double attackCoeffInferior = opt.value("attackCoeffInferior").toDouble();
			double homeAdvValue = (homeAdvantage ? opt.value("homeAdvantage").toDouble() : 1);

			int attacks;
			if(skill > oppSkill)
				attacks = static_cast<int>((baseAttacksSuperior + ((skill - oppSkill) * attackCoeffSuperior)) * attackMultiplier);
			else
				attacks = static_cast<int>((baseAttacksInferior + ((skill - oppSkill) * attackCoeffInferior)) * attackMultiplier);

			// calculate P(goal) on any given attack
			double pGoal = 1 - (bast + rankDiffModifier * (pow(oppSkill * rankCoeff, rankScalar) - pow(skill * rankCoeff, rankScalar))) / (baseAttackCoeff + homeAdvValue);

			// score
			double rand = s->randUniform();
			double acc = 0;
			double pIGoals = pow(1 - pGoal, attacks); // probability of 0 goals
			for(int i = 0; i <= attacks; ++i)
			{
				acc += pIGoals;
				if(rand < acc)
					return i;
				// calculate probability of i + 1 goals
				pIGoals *= (attacks - i) * pGoal / ((i + 1) * (1 - pGoal));
			}
			return -1;
		}

		virtual QPair<int, int> generateStyleModification(int homeScore, int awayScore, double homeStyle, double awayStyle)
		{
			if(userOpt.value("NSFSStyleMods", "true").toString() == "true")
			{
				double styleCoeffA = opt.value("NSFSStyleCoeffA", 2.0991677816057).toDouble();
				double styleCoeffB = opt.value("NSFSStyleCoeffB", 1.2442581729602).toDouble();
				double styleExponent = opt.value("NSFSStyleExponent", -0.42705203296846).toDouble();
				double styleOffset = opt.value("NSFSStyleOffset", 0.072435335725325).toDouble();

				double styleModifier = (homeStyle + awayStyle) / 2.0 + s->randGaussian();
				int result = homeScore - awayScore;

				double styleMultiplier = styleCoeffA / (1 + styleCoeffB * pow(M_E, styleExponent * styleModifier)) + styleOffset;

				homeScore *= styleMultiplier;
				awayScore *= styleMultiplier;

				// if a negative style modifier changed the result to a draw, fix it
				if(homeScore == awayScore && result > 0)
					++homeScore;
				else if(homeScore == awayScore && result < 0)
					++awayScore;

				return qMakePair(homeScore, awayScore);
			}
			else
			{
				int styleEffect = 0;
				int maxStyleEffect = (homeScore == awayScore ? 0 : abs(homeScore - awayScore) - 1) + min(homeScore, awayScore); // what’s the maximum change we can make that will preserve W/D/L?
				styleEffect = (int)(s->individualScore("style", (homeStyle + awayStyle) / 20 + 0.5));
				if(styleEffect < -maxStyleEffect)
					styleEffect = -maxStyleEffect;
				homeScore += max(-homeScore, styleEffect); // don’t drop the score below 0
				awayScore += max(-awayScore, styleEffect); // don’t drop the score below 0
				return qMakePair(homeScore, awayScore);
			}
		}

};

#endif
