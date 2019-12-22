#ifndef SHOOTINGCOMPETITION_H
#define SHOOTINGCOMPETITION_H

#include "competitions/abstractcompetition.h"
#include "competitions/options/shootingcompetitionoptions.h"

class XkorShootingCompetition : public XkorAbstractCompetition
{
	public:
		virtual bool hasOptionsWidget() { return true; }
		virtual int matchdays() { return 2; }
		virtual QStringList matchdayNames() { return QStringList() << "Qualifying" << "Final"; }
		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> options) { return new XkorShootingCompetitionOptions(options); }
		virtual void scorinate(int matchday);
};

#endif // SHOOTINGCOMPETITION_H
