#ifndef ABSTRACTCOMPETITIONFORMAT_H
#define ABSTRACTCOMPETITIONFORMAT_H

#include <QBuffer>
#include "abstractoptionswidget.h"
#include "result.h"
#include "startlist.h"
#include "paradigms/comparators/basicresultcomparator.h"

class XkorAbstractCompetition
{
	public:
		XkorAbstractCompetition() {;;;}
		XkorAbstractCompetition(XkorStartList & sl, XkorSport & s, QHash<QString, QVariant> paradigmOptions, QHash<QString, QVariant> competitionOptions, QHash<int, QString> results) { init(sl, s, paradigmOptions, competitionOptions, results); }
		virtual ~XkorAbstractCompetition() {;;;}
		virtual bool hasOptionsWidget() { return false; }
		void init(XkorStartList & sl, XkorSport & s, QHash<QString, QVariant> paradigmOptions, QHash<QString, QVariant> competitionOptions, QHash<int, QString> results);
		virtual int matchdays() = 0;
		virtual QStringList matchdayNames() = 0;
		virtual int nameWidth();
		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant>) { return 0; }
		virtual QString rankedListOutput(QString title, QVector<XkorResult> results, XkorAbstractResultComparator * comparator);
		virtual QString results(int matchday);
		virtual QHash<QString, QVariant> resumeFileOptions();
		virtual QHash<QString, QVariant> revertToMatchday(int) { return QHash<QString, QVariant>(); } // given matchday is the first that will be erased
		virtual void scorinate(int matchday) = 0;
		
	protected:
		QHash<int, QString> resultsBuf;
		QHash<QString, QVariant> resumeOpt;
		QHash<QString, QVariant> userOpt;
		XkorStartList startList;
		XkorSport sport;
		QHash<QString, QVariant> paradigmOpt;
};

#endif
