#ifndef COMPETITIONFACTORY_H
#define COMPETITIONFACTORY_H

#include "competitions/archerycompetition.h"
#include "competitions/matchescompetition.h"
#include "competitions/multipleruncompetition.h"
#include "competitions/roundrobincompetition.h"
#include "competitions/shootingcompetition.h"
#include "competitions/standardcompetition.h"

class XkorCompetitionFactory
{
	public:
		static XkorAbstractCompetition * newCompetition(const QString type)
		{
			XkorAbstractCompetition * rval;
			if(type == "standard")
				rval = new XkorMassStartCompetition;
			else if(type == "archery")
				rval = new XkorArcheryCompetition;
			else if(type == "matches")
				rval = new XkorMatchesCompetition;
			else if(type == "multipleRun")
				rval = new XkorMultipleRunCompetition;
			else if(type == "roundRobin")
				rval = new XkorRoundRobinCompetition;
			else if(type == "shooting")
				rval = new XkorShootingCompetition;
			else
				rval = new XkorRoundRobinCompetition;
			return rval;
		}
		
		static XkorAbstractCompetition * newCompetition(const QString type, XkorStartList sl, XkorSport s, QHash<QString, QVariant> paradigmOptions, QHash<QString, QVariant> competitionOptions, QHash<int, QString> results)
		{
			XkorAbstractCompetition * rval = newCompetition(type);
			rval->init(sl, s, paradigmOptions, competitionOptions, results);
			return rval;
		}
		
		static QHash<QString, QString> competitionTypes()
		{
			QHash<QString, QString> rval;
			rval["archery"] = QObject::tr("Archery ranking round");
			rval["matches"] = QObject::tr("Individual matches");
			rval["multipleRun"] = QObject::tr("Multiple-run competition");
			rval["shooting"] = QObject::tr("Shooting competition");
			rval["standard"] = QObject::tr("Mass start");
			rval["roundRobin"] = QObject::tr("Round robin");
			return rval;
		}
};

#endif
