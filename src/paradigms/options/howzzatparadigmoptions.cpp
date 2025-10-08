#include "paradigms/options/howzzatparadigmoptions.h"
#include <QStyle>

#include <QtDebug>

XkorHowzzatParadigmOptions::XkorHowzzatParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
	homeAdvantage = new QCheckBox(tr("Apply home advantage"));
	if(options.value("homeAdvantage") == "true")
		homeAdvantage->setCheckState(Qt::Checked);
	else
		homeAdvantage->setCheckState(Qt::Unchecked);
	setHomeAdvantage(homeAdvantage->checkState());
	connect(homeAdvantage, SIGNAL(stateChanged(int)), this, SLOT(setHomeAdvantage(int)));

	useStyleMods = new QCheckBox(tr("Apply style modifiers"));
	if(options.value("useStyleMods", "true") == "true")
		useStyleMods->setCheckState(Qt::Checked);
	else
		useStyleMods->setCheckState(Qt::Unchecked);
	setUseStyleMods(useStyleMods->checkState());
	connect(useStyleMods, SIGNAL(stateChanged(int)), this, SLOT(setUseStyleMods(int)));

	showTLAs = new QCheckBox(tr("Show team names"));
	if(options.value("showTLAs", "true") == "true")
		showTLAs->setCheckState(Qt::Checked);
	else
		showTLAs->setCheckState(Qt::Unchecked);
	setShowTLAs(showTLAs->checkState());
	connect(showTLAs, SIGNAL(stateChanged(int)), this, SLOT(setShowTLAs(int)));

	layout = new QGridLayout(this);
	layout->addWidget(homeAdvantage, 0, 1);
	layout->addWidget(useStyleMods, 1, 1);
	layout->addWidget(showTLAs, 2, 1);
	layout->setHorizontalSpacing(0);
	if(QFlags<Qt::AlignmentFlag>(style()->styleHint(QStyle::SH_FormLayoutFormAlignment)) & Qt::AlignHCenter)
	{ // center the check boxes, but leave them left-aligned with each other on Mac OS
		layout->setColumnStretch(0, 1093);
		layout->setColumnStretch(2, 1093);
	}
}

XkorHowzzatParadigmOptions::~XkorHowzzatParadigmOptions()
{
	delete homeAdvantage;
	delete useStyleMods;
	delete showTLAs;
}

void XkorHowzzatParadigmOptions::setHomeAdvantage(int x)
{
	if(x == Qt::Checked)
		options.insert("homeAdvantage", "true");
	else
		options.insert("homeAdvantage", "false");
	emit optionsChanged(options);
}

void XkorHowzzatParadigmOptions::setShowTLAs(int x)
{
	if(x == Qt::Checked)
		options.insert("showTLAs", "true");
	else
		options.insert("showTLAs", "false");
	emit optionsChanged(options);
}

void XkorHowzzatParadigmOptions::setUseStyleMods(int x)
{
	if(x == Qt::Checked)
		options.insert("useStyleMods", "true");
	else
		options.insert("useStyleMods", "false");
	emit optionsChanged(options);
}
