#ifndef SHORTTRACKPARADIGMOPTIONS_H
#define SHORTTRACKPARADIGMOPTIONS_H

#include <QCheckBox>
#include <QFormLayout>
#include "abstractoptionswidget.h"

#include <QCheckBox>
#include <QFormLayout>

class XkorShortTrackParadigmOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT

	private:
		QCheckBox * showTLAs;
		QCheckBox * allowAdvancement;
	private slots:
		void setShowTLAs(int x);
		void setAllowAdvancement(int x);

	public:
		XkorShortTrackParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
		~XkorShortTrackParadigmOptions();
};

#endif // SHORTTRACKPARADIGMOPTIONS_H
