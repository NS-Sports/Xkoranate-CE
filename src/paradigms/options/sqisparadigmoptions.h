#ifndef SQISPARADIGMOPTIONS_H
#define SQISPARADIGMOPTIONS_H

#include <QtGui>
#include "abstractoptionswidget.h"

#include <QCheckBox>
#include <QRadioButton>
#include <QFormLayout>
#include <QLabel>

class XkorSQISParadigmOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT
	
	private:
		QCheckBox * homeAdvantage;
		QCheckBox * showTLAs;
		QRadioButton * xkorStyleMods;
		QRadioButton * nsfsStyleMods;
		QRadioButton * noStyleMods;
		QButtonGroup * styleMods;
		QGridLayout * layout;
		
	private slots:
		void setHomeAdvantage(int x);
		void setShowTLAs(int x);
		void setStyleMods();
		
	public:
		XkorSQISParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
		~XkorSQISParadigmOptions();
};

#endif
