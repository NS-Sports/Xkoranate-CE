#ifndef AUTORACINGPARADIGMOPTIONS_H
#define AUTORACINGPARADIGMOPTIONS_H

#include <QtGui>
#include "abstractoptionswidget.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QGridLayout>
#include <QLabel>
#include <QFormLayout>

class XkorAutoRacingParadigmOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT

	private:
		QSpinBox * laps;
		QDoubleSpinBox * lapRecord;
		QDoubleSpinBox * lapVariance;
		QDoubleSpinBox * trackAcceleration;
		QDoubleSpinBox * trackCornering;
		QCheckBox * showTLAs, * useStartingGrid;
		QRadioButton * useACR;
		QRadioButton * useSkill;
		QButtonGroup * skillType;
		QGridLayout * layout;

	private slots:
		void setLaps(int x);
		void setLapRecord(double x);
		void setLapVariance(double x);
		void setShowTLAs(int x);
		void setSkillType();
		void setTrackAcceleration(double x);
		void setTrackCornering(double x);
		void setUseStartingGrid(int x);

	public:
		XkorAutoRacingParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
		~XkorAutoRacingParadigmOptions();
};

#endif // AUTORACINGPARADIGMOPTIONS_H
