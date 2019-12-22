#include "shootingcompetitionoptions.h"

XkorShootingCompetitionOptions::XkorShootingCompetitionOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
	cutoff = new QSpinBox;
	cutoff->setMinimum(0);
	cutoff->setMaximum(999);
	cutoff->setSpecialValueText(tr("all"));
	if(options.contains("cutoff"))
		cutoff->setValue(options.value("cutoff").toInt());
	connect(cutoff, SIGNAL(valueChanged(int)), this, SLOT(setCutoff(int)));

	explanation = new QLabel(tr("<small>In the qualifying round, all ties will be unresolved.<br/>In the final round, only ties for medal positions will be settled by shoot-offs.</small>"));
	explanation->setWordWrap(true);

	QFormLayout * form = new QFormLayout;
	form->addRow(tr("Shooters advancing to final"), cutoff);

	QGridLayout * layout = new QGridLayout(this);
	layout->addLayout(form, 0, 0);
	layout->addWidget(explanation, 1, 0);
}

void XkorShootingCompetitionOptions::setCutoff(int value)
{
	if(value == 0)
		explanation->setText(tr("<small>In the qualifying round, all ties will be left unresolved.<br/>In the final round, only ties for medal positions will be settled by shoot-offs.</small>"));
	else if(value % 10 == 1 && value != 11)
		explanation->setText(tr("<small>In the qualifying round, only a tie for %1st place will be settled by a shoot-off.<br/>In the final round, only ties for medal positions will be settled by shoot-offs.</small>").arg(value));
	else if(value % 10 == 2 && value != 12)
		explanation->setText(tr("<small>In the qualifying round, only a tie for %1nd place will be settled by a shoot-off.<br/>In the final round, only ties for medal positions will be settled by shoot-offs.</small>").arg(value));
	else if(value % 10 == 3 && value != 13)
		explanation->setText(tr("<small>In the qualifying round, only a tie for %1rd place will be settled by a shoot-off.<br/>In the final round, only ties for medal positions will be settled by shoot-offs.</small>").arg(value));
	else
		explanation->setText(tr("<small>In the qualifying round, only a tie for %1th place will be settled by a shoot-off.<br/>In the final round, only ties for medal positions will be settled by shoot-offs.</small>").arg(value));
	options["cutoff"] = value;
	emit optionsChanged(options);
}
