#ifndef SHOOTINGCOMPETITIONOPTIONS_H
#define SHOOTINGCOMPETITIONOPTIONS_H

#include "abstractoptionswidget.h"
#include <QSpinBox>
#include <QLabel>
#include <QFormLayout>

class XkorShootingCompetitionOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT
	public:
		XkorShootingCompetitionOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
	private:
		QSpinBox * cutoff;
		QLabel * explanation;
	private slots:
		void setCutoff(int value);
};

#endif // SHOOTINGCOMPETITIONOPTIONS_H
