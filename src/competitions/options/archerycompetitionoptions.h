#ifndef ARCHERYCOMPETITIONOPTIONS_H
#define ARCHERYCOMPETITIONOPTIONS_H

#include "abstractoptionswidget.h"
#include <QCheckBox>

#include <QSpinBox>
#include <QLabel>

class XkorArcheryCompetitionOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT
	public:
		XkorArcheryCompetitionOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
	private:
		QSpinBox * cutoff;
		QLabel * explanation;
	private slots:
		void setCutoff(int value);
};

#endif // ARCHERYCOMPETITIONOPTIONS_H
