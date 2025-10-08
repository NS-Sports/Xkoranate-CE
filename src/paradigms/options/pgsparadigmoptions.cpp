#include "paradigms/options/pgsparadigmoptions.h"
#include <QStyle>

XkorPGSParadigmOptions::XkorPGSParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
        showTLAs = new QCheckBox(tr("Show team names"));
	if(options.value("showTLAs") == "false")
		showTLAs->setCheckState(Qt::Unchecked);
	else
		showTLAs->setCheckState(Qt::Checked);
	setShowTLAs(showTLAs->checkState());
	connect(showTLAs, SIGNAL(stateChanged(int)), this, SLOT(setShowTLAs(int)));

	maxScore = new QDoubleSpinBox;
	maxScore->setRange(0, 1.5);
	maxScore->setSingleStep(0.01);
	maxScore->setValue(options.value("maxScore", 1.5).toDouble());
	connect(maxScore, SIGNAL(valueChanged(double)), this, SLOT(setMaxScore(double)));

	QFormLayout * form = new QFormLayout;
	form->addRow(tr("Maximum score after first run:"), maxScore);

	QGridLayout * layout = new QGridLayout(this);
	layout->addWidget(showTLAs, 0, 0, QFlags<Qt::AlignmentFlag>(style()->styleHint(QStyle::SH_FormLayoutFormAlignment)));
	layout->addLayout(form, 1, 0);
}

XkorPGSParadigmOptions::~XkorPGSParadigmOptions()
{
	delete showTLAs;
	delete maxScore;
}

void XkorPGSParadigmOptions::setMaxScore(double x)
{
	options.insert("maxScore", x);
	emit optionsChanged(options);
}

void XkorPGSParadigmOptions::setShowTLAs(int x)
{
	if(x == Qt::Checked)
		options.insert("showTLAs", "true");
	else
		options.insert("showTLAs", "false");
	emit optionsChanged(options);
}

