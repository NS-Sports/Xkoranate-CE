#include "paradigms/options/nsfsgridironparadigmoptions.h"
#include <QGridLayout>
#include <QStyle>

XkorNSFSGridironParadigmOptions::XkorNSFSGridironParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
	usePeriods = new QCheckBox(tr("Show period-by-period results"));
	if(options.value("usePeriods", "true") == "true")
		usePeriods->setCheckState(Qt::Checked);
	setUsePeriods(usePeriods->checkState());
	connect(usePeriods, SIGNAL(stateChanged(int)), this, SLOT(setUsePeriods(int)));

	showTLAs = new QCheckBox(tr("Show team names"));
	if(options.value("showTLAs", "true") == "true")
		showTLAs->setCheckState(Qt::Checked);
	setShowTLAs(showTLAs->checkState());
	connect(showTLAs, SIGNAL(stateChanged(int)), this, SLOT(setShowTLAs(int)));

	// layout
	QGridLayout * layout = new QGridLayout(this);
	layout->addWidget(usePeriods, 0, 1);
	layout->addWidget(showTLAs, 1, 1);
	layout->setHorizontalSpacing(0);
	if(QFlags<Qt::AlignmentFlag>(style()->styleHint(QStyle::SH_FormLayoutFormAlignment)) & Qt::AlignHCenter)
	{ // center the check boxes, but leave them left-aligned with each other on Mac OS
		layout->setColumnStretch(0, 1093);
		layout->setColumnStretch(2, 1093);
	}
}

XkorNSFSGridironParadigmOptions::~XkorNSFSGridironParadigmOptions()
{
	delete usePeriods;
	delete showTLAs;
}

void XkorNSFSGridironParadigmOptions::setShowTLAs(int x)
{
	if(x == Qt::Checked)
		options.insert("showTLAs", "true");
	else
		options.insert("showTLAs", "false");
	emit optionsChanged(options);
}

void XkorNSFSGridironParadigmOptions::setUsePeriods(int x)
{
	if(x == Qt::Checked)
		options.insert("usePeriods", "true");
	else
		options.insert("usePeriods", "false");
	emit optionsChanged(options);
}
