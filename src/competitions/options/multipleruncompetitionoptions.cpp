#include "competitions/options/multipleruncompetitionoptions.h"
#include <QFormLayout>
#include <QGridLayout>
#include <QStyle>

XkorMultipleRunCompetitionOptions::XkorMultipleRunCompetitionOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
	runs = new QSpinBox;
	runs->setValue(options.value("runs", 2).toInt());
	runs->setRange(2, 99);
	connect(runs, SIGNAL(valueChanged(int)), this, SLOT(setRuns(int)));

	sortResults = new QCheckBox(tr("Sort results"));
	if(options.value("sortResults", "true") == "true")
		sortResults->setCheckState(Qt::Checked);
	connect(sortResults, SIGNAL(stateChanged(int)), this, SLOT(setSortResults(int)));

	sortByBestResult = new QCheckBox(tr("Sort by best result when total result is tied"));
	if(options.value("sortByBestResult", "true") == "true")
		sortByBestResult->setCheckState(Qt::Checked);
	sortByBestResult->setEnabled(sortResults->checkState() == Qt::Checked);
	connect(sortByBestResult, SIGNAL(stateChanged(int)), this, SLOT(setSortByBestResult(int)));

	QFormLayout * form = new QFormLayout;
	form->addRow(tr("Runs:"), runs);

	QGridLayout * layout = new QGridLayout(this);
	layout->addLayout(form, 0, 1);
	layout->addWidget(sortResults, 1, 1);
	layout->addWidget(sortByBestResult, 2, 1);
	layout->setHorizontalSpacing(0);
	if(QFlags<Qt::AlignmentFlag>(style()->styleHint(QStyle::SH_FormLayoutFormAlignment)) & Qt::AlignHCenter)
	{ // center the check boxes, but leave them left-aligned with each other on Mac OS
		layout->setColumnStretch(0, 1093);
		layout->setColumnStretch(2, 1093);
	}
}

void XkorMultipleRunCompetitionOptions::setRuns(int value)
{
	options["runs"] = value;
	emit optionsChanged(options);
}

void XkorMultipleRunCompetitionOptions::setSortByBestResult(int value)
{
	if(value == Qt::Checked)
		options["sortByBestResult"] = "true";
	else
		options["sortByBestResult"] = "false";
	emit optionsChanged(options);
}

void XkorMultipleRunCompetitionOptions::setSortResults(int value)
{
	if(value == Qt::Checked)
	{
		options["sortResults"] = "true";
		sortByBestResult->setEnabled(true);
		setSortByBestResult(sortByBestResult->checkState());
	}
	else
	{
		options["sortResults"] = "false";
		sortByBestResult->setEnabled(false);
		options["sortByBestResult"] = "false";
	}
	emit optionsChanged(options);
}
