#ifndef TIMEDPARADIGMOPTIONS_H
#define TIMEDPARADIGMOPTIONS_H

#include <QCheckBox>
#include <QFormLayout>
#include "abstractoptionswidget.h"

class XkorTimedParadigmOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT

	private:
		QCheckBox * showTLAs;
	private slots:
		void setShowTLAs(int x);

	public:
		XkorTimedParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
		~XkorTimedParadigmOptions();
};

#endif // TIMEDPARADIGMOPTIONS_H
