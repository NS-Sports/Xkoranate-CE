#include "archerycompetitionoptions.h"

#include <QFormLayout>

XkorArcheryCompetitionOptions::XkorArcheryCompetitionOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
	cutoff = new QSpinBox;
	cutoff->setMinimum(0);
	cutoff->setMaximum(999);
	cutoff->setSpecialValueText(tr("all"));
	if(options.contains("cutoff"))
		cutoff->setValue(options.value("cutoff").toInt());
	connect(cutoff, SIGNAL(valueChanged(int)), this, SLOT(setCutoff(int)));

	explanation = new QLabel(tr("<small>All ties will be settled by lots.</small>"));
	explanation->setWordWrap(true);

	QFormLayout * form = new QFormLayout;
	form->addRow(tr("Archers advancing to next round"), cutoff);

	QGridLayout * layout = new QGridLayout(this);
	layout->addLayout(form, 0, 0);
	layout->addWidget(explanation, 1, 0);
}

void XkorArcheryCompetitionOptions::setCutoff(int value)
{
	if(value == 0)
		explanation->setText(tr("<small>All ties will be settled by lots.</small>"));
	else if(value % 10 == 1 && value != 11)
		explanation->setText(tr("<small>A tie for %1st place will be settled by tiebreak arrows. All other ties will be settled by drawing lots.").arg(value));
	else if(value % 10 == 2 && value != 12)
		explanation->setText(tr("<small>A tie for %1nd place will be settled by tiebreak arrows. All other ties will be settled by drawing lots.").arg(value));
	else if(value % 10 == 3 && value != 13)
		explanation->setText(tr("<small>A tie for %1rd place will be settled by tiebreak arrows. All other ties will be settled by drawing lots.").arg(value));
	else
		explanation->setText(tr("<small>A tie for %1th place will be settled by tiebreak arrows. All other ties will be settled by drawing lots.").arg(value));
	options["cutoff"] = value;
	emit optionsChanged(options);
}
