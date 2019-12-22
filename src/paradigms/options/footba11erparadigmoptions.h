#ifndef FOOTBA11ERPARADIGMOPTIONS_H
#define FOOTBA11ERPARADIGMOPTIONS_H

#include <QtWidgets>
#include "abstractoptionswidget.h"

class XkorFootba11erParadigmOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT

	private:
		QCheckBox * homeAdvantage;
		QCheckBox * showTLAs;
		QGridLayout * layout;

	private slots:
		void setHomeAdvantage(int x);
		void setShowTLAs(int x);

	public:
		XkorFootba11erParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
		~XkorFootba11erParadigmOptions();
};

#endif // FOOTBA11ERPARADIGMOPTIONS_H
