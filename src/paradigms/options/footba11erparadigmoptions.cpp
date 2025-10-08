#include "paradigms/options/footba11erparadigmoptions.h"
#include <QStyle>

#include <QtDebug>

XkorFootba11erParadigmOptions::XkorFootba11erParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
	homeAdvantage = new QCheckBox(tr("Apply home advantage"));
	if(options.value("homeAdvantage") == "true")
		homeAdvantage->setCheckState(Qt::Checked);
	else
		homeAdvantage->setCheckState(Qt::Unchecked);
	setHomeAdvantage(homeAdvantage->checkState());
	connect(homeAdvantage, SIGNAL(stateChanged(int)), this, SLOT(setHomeAdvantage(int)));

	showTLAs = new QCheckBox(tr("Show team names"));
	if(options.value("showTLAs", "true") == "true")
		showTLAs->setCheckState(Qt::Checked);
	else
		showTLAs->setCheckState(Qt::Unchecked);
	setShowTLAs(showTLAs->checkState());
	connect(showTLAs, SIGNAL(stateChanged(int)), this, SLOT(setShowTLAs(int)));

	layout = new QGridLayout(this);
	layout->addWidget(homeAdvantage, 0, 1);
	layout->addWidget(showTLAs, 1, 1);
	layout->setHorizontalSpacing(0);
	if(QFlags<Qt::AlignmentFlag>(style()->styleHint(QStyle::SH_FormLayoutFormAlignment)) & Qt::AlignHCenter)
	{ // center the check boxes, but leave them left-aligned with each other on Mac OS
		layout->setColumnStretch(0, 1093);
		layout->setColumnStretch(2, 1093);
	}
}

XkorFootba11erParadigmOptions::~XkorFootba11erParadigmOptions()
{
	delete homeAdvantage;
	delete showTLAs;
}

void XkorFootba11erParadigmOptions::setHomeAdvantage(int x)
{
	if(x == Qt::Checked)
		options.insert("homeAdvantage", "true");
	else
		options.insert("homeAdvantage", "false");
	emit optionsChanged(options);
}

void XkorFootba11erParadigmOptions::setShowTLAs(int x)
{
	if(x == Qt::Checked)
		options.insert("showTLAs", "true");
	else
		options.insert("showTLAs", "false");
	emit optionsChanged(options);
}
