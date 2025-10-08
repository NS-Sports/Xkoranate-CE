#include "paradigms/options/shorttrackparadigmoptions.h"
#include <QStyle>

XkorShortTrackParadigmOptions::XkorShortTrackParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
        showTLAs = new QCheckBox(tr("Show team names"));
	if(options.value("showTLAs") == "false")
		showTLAs->setCheckState(Qt::Unchecked);
	else
		showTLAs->setCheckState(Qt::Checked);
	setShowTLAs(showTLAs->checkState());
	connect(showTLAs, SIGNAL(stateChanged(int)), this, SLOT(setShowTLAs(int)));

	allowAdvancement = new QCheckBox(tr("Allow athletes to be advanced"));
	if(options.value("allowAdvancement") == "false")
		allowAdvancement->setCheckState(Qt::Unchecked);
	else
		allowAdvancement->setCheckState(Qt::Checked);
	setAllowAdvancement(allowAdvancement->checkState());
	connect(allowAdvancement, SIGNAL(stateChanged(int)), this, SLOT(setAllowAdvancement(int)));

	// layout
	QGridLayout * layout = new QGridLayout(this);
	layout->addWidget(showTLAs, 0, 1);
	layout->addWidget(allowAdvancement, 1, 1);
	layout->setHorizontalSpacing(0);
	if(QFlags<Qt::AlignmentFlag>(style()->styleHint(QStyle::SH_FormLayoutFormAlignment)) & Qt::AlignHCenter)
	{ // center the check boxes, but leave them left-aligned with each other on Mac OS
		layout->setColumnStretch(0, 1093);
		layout->setColumnStretch(2, 1093);
	}
}

XkorShortTrackParadigmOptions::~XkorShortTrackParadigmOptions()
{
	delete showTLAs;
	delete allowAdvancement;
}

void XkorShortTrackParadigmOptions::setAllowAdvancement(int x)
{
	if(x == Qt::Checked)
		options.insert("allowAdvancement", "true");
	else
		options.insert("allowAdvancement", "false");
	emit optionsChanged(options);
}

void XkorShortTrackParadigmOptions::setShowTLAs(int x)
{
	if(x == Qt::Checked)
		options.insert("showTLAs", "true");
	else
		options.insert("showTLAs", "false");
	emit optionsChanged(options);
}

