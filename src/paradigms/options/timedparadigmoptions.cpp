#include "paradigms/options/timedparadigmoptions.h"
#include <QStyle>

XkorTimedParadigmOptions::XkorTimedParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
        showTLAs = new QCheckBox(tr("Show team names"));
	if(options.value("showTLAs") == "false")
		showTLAs->setCheckState(Qt::Unchecked);
	else
		showTLAs->setCheckState(Qt::Checked);
	setShowTLAs(showTLAs->checkState());
	connect(showTLAs, SIGNAL(stateChanged(int)), this, SLOT(setShowTLAs(int)));

	// layout
	QGridLayout * layout = new QGridLayout(this);
	layout->addWidget(showTLAs, 0, 0, QFlags<Qt::AlignmentFlag>(style()->styleHint(QStyle::SH_FormLayoutFormAlignment)));
}

XkorTimedParadigmOptions::~XkorTimedParadigmOptions()
{
	delete showTLAs;
}

void XkorTimedParadigmOptions::setShowTLAs(int x)
{
	if(x == Qt::Checked)
		options.insert("showTLAs", "true");
	else
		options.insert("showTLAs", "false");
	emit optionsChanged(options);
}
