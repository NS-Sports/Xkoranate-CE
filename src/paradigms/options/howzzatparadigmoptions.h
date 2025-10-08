#ifndef HOWZZATPARADIGMOPTIONS_H
#define HOWZZATPARADIGMOPTIONS_H

#include <QtGui>
#include "abstractoptionswidget.h"

#include <QCheckBox>
#include <QGridLayout>

class XkorHowzzatParadigmOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT

	private:
		QCheckBox * homeAdvantage;
		QCheckBox * showTLAs;
		QCheckBox * useStyleMods;
		QGridLayout * layout;

	private slots:
		void setHomeAdvantage(int x);
		void setShowTLAs(int x);
		void setUseStyleMods(int x);

	public:
		XkorHowzzatParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
		~XkorHowzzatParadigmOptions();
};

#endif // HOWZZATPARADIGMOPTIONS_H
