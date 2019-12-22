#ifndef MULTIPLERUNCOMPETITION_H
#define MULTIPLERUNCOMPETITION_H

#include "competitions/abstractcompetition.h"
#include "competitions/options/multipleruncompetitionoptions.h"

class XkorMultipleRunCompetition : public XkorAbstractCompetition
{
	public:
		virtual bool hasOptionsWidget() { return true; }
		virtual int matchdays() { return userOpt.value("runs", 2).toInt(); }
		virtual QStringList matchdayNames();
		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> options) { return new XkorMultipleRunCompetitionOptions(options); }
		virtual QHash<QString, QVariant> revertToMatchday(int matchday);
		virtual void scorinate(int matchday);
};

#endif // MULTIPLERUNCOMPETITION_H
