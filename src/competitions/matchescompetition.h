#ifndef MATCHESCOMPETITION_H
#define MATCHESCOMPETITION_H

#include "competitions/abstractcompetition.h"

class XkorMatchesCompetition : public XkorAbstractCompetition
{
	public:
		bool hasOptionsWidget() { return true; }
		virtual int matchdays() { return 1; }
		virtual QStringList matchdayNames() { return QStringList() << "Mass start"; }
		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> competitionOptions);
		void scorinate(int matchday);
};

#endif // MATCHESCOMPETITION_H
