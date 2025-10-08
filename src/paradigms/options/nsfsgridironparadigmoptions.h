#ifndef NSFSGRIDIRONPARADIGMOPTIONS_H
#define NSFSGRIDIRONPARADIGMOPTIONS_H

#include <QtGui>
#include "abstractoptionswidget.h"

#include <QCheckBox>

class XkorNSFSGridironParadigmOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT

	private:
		QCheckBox * usePeriods;
		QCheckBox * showTLAs;

	private slots:
		void setUsePeriods(int x);
		void setShowTLAs(int x);

	public:
		XkorNSFSGridironParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
		~XkorNSFSGridironParadigmOptions();
};

#endif // NSFSGRIDIRONPARADIGMOPTIONS_H
