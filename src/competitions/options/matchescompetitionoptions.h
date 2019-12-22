#ifndef MATCHESCOMPETITIONOPTIONS_H
#define MATCHESCOMPETITIONOPTIONS_H

#include "abstractoptionswidget.h"
#include <QCheckBox>

class XkorMatchesCompetitionOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT
	public:
		XkorMatchesCompetitionOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
	private:
		QCheckBox * allowDraws;
	private slots:
		void setAllowDraws(int value);
};

#endif // MATCHESCOMPETITIONOPTIONS_H
