#include "competitions/options/matchescompetitionoptions.h"
#include <QStyle>

#include <QGridLayout>

XkorMatchesCompetitionOptions::XkorMatchesCompetitionOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
	allowDraws = new QCheckBox(tr("Allow draws"));
	if(options.contains("allowDraws") && options.value("allowDraws") == "false")
		allowDraws->setCheckState(Qt::Unchecked);
	else
		allowDraws->setCheckState(Qt::Checked);
	connect(allowDraws, SIGNAL(stateChanged(int)), this, SLOT(setAllowDraws(int)));

	QGridLayout * layout = new QGridLayout(this);
	layout->addWidget(allowDraws, 0, 0, QFlags<Qt::AlignmentFlag>(style()->styleHint(QStyle::SH_FormLayoutFormAlignment)));
}

void XkorMatchesCompetitionOptions::setAllowDraws(int value)
{
	if(value == Qt::Checked)
		options["allowDraws"] = "true";
	else
		options["allowDraws"] = "false";
	emit optionsChanged(options);
}
