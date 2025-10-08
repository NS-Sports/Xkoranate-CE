#ifndef PGSPARADIGMOPTIONS_H
#define PGSPARADIGMOPTIONS_H

#include <QCheckBox>
#include <QFormLayout>
#include "abstractoptionswidget.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>

class XkorPGSParadigmOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT

	private:
		QCheckBox * showTLAs;
		QDoubleSpinBox * maxScore;
	private slots:
		void setShowTLAs(int x);
		void setMaxScore(double x);

	public:
		XkorPGSParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
		~XkorPGSParadigmOptions();
};

#endif // PGSPARADIGMOPTIONS_H
