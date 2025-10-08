#include "competitions/options/massstartcompetitionoptions.h"
#include <QGridLayout>
#include <QStyle>

XkorMassStartCompetitionOptions::XkorMassStartCompetitionOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
	overallRanking = new QCheckBox(tr("Show overall ranking across all groups"));
	if(options.value("overallRanking", "false") == "true")
		overallRanking->setCheckState(Qt::Checked);
	connect(overallRanking, SIGNAL(stateChanged(int)), this, SLOT(setOverallRanking(int)));

	sortResults = new QCheckBox(tr("Sort results"));
	if(options.value("sortResults", "true") == "true")
		sortResults->setCheckState(Qt::Checked);
	connect(sortResults, SIGNAL(stateChanged(int)), this, SLOT(setSortResults(int)));

	QGridLayout * layout = new QGridLayout(this);
	layout->addWidget(sortResults, 0, 1);
	layout->addWidget(overallRanking, 1, 1);
	layout->setHorizontalSpacing(0);
	if(QFlags<Qt::AlignmentFlag>(style()->styleHint(QStyle::SH_FormLayoutFormAlignment)) & Qt::AlignHCenter)
	{ // center the check boxes, but leave them left-aligned with each other on Mac OS
		layout->setColumnStretch(0, 1093);
		layout->setColumnStretch(2, 1093);
	}
}

void XkorMassStartCompetitionOptions::setOverallRanking(int value)
{
	if(value == Qt::Checked)
		options["overallRanking"] = "true";
	else
		options["overallRanking"] = "false";
	emit optionsChanged(options);
}

void XkorMassStartCompetitionOptions::setSortResults(int value)
{
	if(value == Qt::Checked)
	{
		options["sortResults"] = "true";
		overallRanking->setEnabled(true);
		setOverallRanking(overallRanking->checkState());
	}
	else
	{
		options["sortResults"] = "false";
		overallRanking->setEnabled(false);
		options["overallRanking"] = "false";
	}
	emit optionsChanged(options);
}
