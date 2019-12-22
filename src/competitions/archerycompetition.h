#ifndef ARCHERYCOMPETITION_H
#define ARCHERYCOMPETITION_H

#include "competitions/abstractcompetition.h"
#include "competitions/options/archerycompetitionoptions.h"

class XkorArcheryCompetition : public XkorAbstractCompetition
{
	public:
		virtual bool hasOptionsWidget() { return true; }
		virtual int matchdays() { return 1; }
		virtual QStringList matchdayNames() { return QStringList() << "Ranking round"; }
		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> options) { return new XkorArcheryCompetitionOptions(options); }
		virtual void scorinate(int matchday);
};

#endif // ARCHERYCOMPETITION_H
