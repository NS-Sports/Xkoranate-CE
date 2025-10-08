#ifndef MASSSTARTCOMPETITIONOPTIONS_H
#define MASSSTARTCOMPETITIONOPTIONS_H

#include "abstractoptionswidget.h"

#include <QCheckBox>

class XkorMassStartCompetitionOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT

	public:
		XkorMassStartCompetitionOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
	private:
		QCheckBox * overallRanking, * sortResults;
	private slots:
		void setOverallRanking(int value);
		void setSortResults(int value);
};

#endif // MASSSTARTCOMPETITIONOPTIONS_H
