#ifndef MULTIPLERUNCOMPETITIONOPTIONS_H
#define MULTIPLERUNCOMPETITIONOPTIONS_H

#include "abstractoptionswidget.h"

#include <QSpinBox>
#include <QCheckBox>

class XkorMultipleRunCompetitionOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT

	public:
		XkorMultipleRunCompetitionOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
	private:
		QCheckBox * sortResults, * sortByBestResult;
		QSpinBox * runs;
	private slots:
		void setRuns(int value);
		void setSortByBestResult(int value);
		void setSortResults(int value);
};

#endif // MULTIPLERUNCOMPETITIONOPTIONS_H
