#include "paradigms/options/nsfsparadigmoptions.h"

XkorNSFSParadigmOptions::XkorNSFSParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
	homeAdvantage = new QCheckBox(tr("Apply home advantage"));
	if(options.value("homeAdvantage") == "true")
		homeAdvantage->setCheckState(Qt::Checked);
	setHomeAdvantage(homeAdvantage->checkState());
	connect(homeAdvantage, SIGNAL(stateChanged(int)), this, SLOT(setHomeAdvantage(int)));

	showTLAs = new QCheckBox(tr("Show team names"));
	if(options.value("showTLAs", "true") == "true")
		showTLAs->setCheckState(Qt::Checked);
	setShowTLAs(showTLAs->checkState());
	connect(showTLAs, SIGNAL(stateChanged(int)), this, SLOT(setShowTLAs(int)));

	// style modifiers options
	xkorStyleMods = new QRadioButton(tr("xkoranate-style (additive)"));
	nsfsStyleMods = new QRadioButton(tr("NSFS3-style (multiplicative)"));
	noStyleMods = new QRadioButton(tr("Disable style modifiers"));
	if(options.value("NSFSStyleMods") != "false")
		nsfsStyleMods->setChecked(true);
	else if(options.value("styleMods") == "true")
		xkorStyleMods->setChecked(true);
	else
		noStyleMods->setChecked(true);
	styleMods = new QButtonGroup;
	styleMods->addButton(xkorStyleMods);
	styleMods->addButton(nsfsStyleMods);
	styleMods->addButton(noStyleMods);
	connect(styleMods, SIGNAL(buttonClicked(int)), this, SLOT(setStyleMods()));

	QFormLayout * styleModsForm = new QFormLayout;
	styleModsForm->addRow(xkorStyleMods);
	styleModsForm->addRow(nsfsStyleMods);
	styleModsForm->addRow(noStyleMods);

	// QFormLayout sucks, so we create our own label
	QLabel * label = new QLabel(tr("Style modifiers:"));
	label->setContentsMargins(0, -4, 0, 0);

	QFormLayout * form = new QFormLayout(this);
	form->addRow(QString(), homeAdvantage);
	form->addRow(QString(), showTLAs);
	form->addRow(label, styleModsForm);
}

XkorNSFSParadigmOptions::~XkorNSFSParadigmOptions()
{
	delete homeAdvantage;
	delete showTLAs;
}

void XkorNSFSParadigmOptions::setHomeAdvantage(int x)
{
	if(x == Qt::Checked)
		options.insert("homeAdvantage", "true");
	else
		options.insert("homeAdvantage", "false");
	emit optionsChanged(options);
}

void XkorNSFSParadigmOptions::setShowTLAs(int x)
{
	if(x == Qt::Checked)
		options.insert("showTLAs", "true");
	else
		options.insert("showTLAs", "false");
	emit optionsChanged(options);
}

void XkorNSFSParadigmOptions::setStyleMods()
{
	QAbstractButton * checkedButton = styleMods->checkedButton();
	if(checkedButton == nsfsStyleMods)
	{
		options.insert("styleMods", "false");
		options.insert("NSFSStyleMods", "true");
	}
	else if(checkedButton == xkorStyleMods)
	{
		options.insert("styleMods", "true");
		options.insert("NSFSStyleMods", "false");
	}
	else
	{
		options.insert("styleMods", "false");
		options.insert("NSFSStyleMods", "false");
	}
	emit optionsChanged(options);
}
