#ifndef SQISPARADIGM_H
#define SQISPARADIGM_H

#include "paradigms/abstracth2hparadigm.h"
#include "paradigms/options/sqisparadigmoptions.h"

class XkorSQISParadigm : public XkorAbstractH2HParadigm
{
	public:
		XkorSQISParadigm() : XkorAbstractH2HParadigm() {;;;}
		XkorSQISParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractH2HParadigm(sport, userOptions) {;;;}
		
		virtual bool hasOptionsWidget() { return true; }
		
		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> paradigmOptions)
		{
			return new XkorSQISParadigmOptions(paradigmOptions);
		}
	
	protected:
		virtual int generateScore(double skill, double oppSkill, double, double, bool homeAdvantage = false, double attackMultiplier = 1)
		{
			int attacks = static_cast<int>(opt.value("attacks").toDouble() * attackMultiplier + 0.5); // add 0.5 so that values can be rounded up

			double a = opt.value("constantA").toDouble();
			double b = opt.value("constantB").toDouble();
			double homeAdvValue = (homeAdvantage ? opt.value("homeAdvantage").toDouble() : 1);

			// calculate P(goal) on any given attack
			double pGoal = (a + (b - (skill == oppSkill ? 1 : min(skill, oppSkill) / max(skill, oppSkill)) * b) * (skill > oppSkill ? 1 : -1)) * homeAdvValue;
			
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
};

#endif
