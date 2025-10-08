#include "paradigms/options/autoracingparadigmoptions.h"
#include <QButtonGroup>

#include <QtDebug>

XkorAutoRacingParadigmOptions::XkorAutoRacingParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
	laps = new QSpinBox;
	laps->setRange(1, 999);
	laps->setValue(options.value("laps", 50).toInt());
	connect(laps, SIGNAL(valueChanged(int)), this, SLOT(setLaps(int)));

	lapRecord = new QDoubleSpinBox;
	lapRecord->setDecimals(3);
	lapRecord->setRange(0.001, 999.999);
	lapRecord->setValue(options.value("lapRecord", 90).toDouble());
	connect(lapRecord, SIGNAL(valueChanged(double)), this, SLOT(setLapRecord(double)));

	lapVariance = new QDoubleSpinBox;
	lapVariance->setDecimals(1);
	lapVariance->setRange(1, 99);
	lapVariance->setValue(options.value("lapVariance", 15).toDouble());
	connect(lapVariance, SIGNAL(valueChanged(double)), this, SLOT(setLapVariance(double)));

	trackAcceleration = new QDoubleSpinBox;
	trackAcceleration->setDecimals(1);
	trackAcceleration->setRange(0, 10);
	trackAcceleration->setValue(options.value("trackAcceleration", 5).toDouble());
	trackAcceleration->setEnabled(false);
	connect(trackAcceleration, SIGNAL(valueChanged(double)), this, SLOT(setTrackAcceleration(double)));

	trackCornering = new QDoubleSpinBox;
	trackCornering->setDecimals(1);
	trackCornering->setRange(0, 10);
	trackCornering->setValue(options.value("trackCornering", 5).toDouble());
	trackCornering->setEnabled(false);
	connect(trackCornering, SIGNAL(valueChanged(double)), this, SLOT(setTrackCornering(double)));

	showTLAs = new QCheckBox(tr("Show team names"));
	if(options.value("showTLAs", "true") == "true")
		showTLAs->setCheckState(Qt::Checked);
	else
		showTLAs->setCheckState(Qt::Unchecked);
	setShowTLAs(showTLAs->checkState());
	connect(showTLAs, SIGNAL(stateChanged(int)), this, SLOT(setShowTLAs(int)));

	useStartingGrid = new QCheckBox(tr("Use starting grid"));
	if(options.value("useStartingGrid") == "true")
		useStartingGrid->setCheckState(Qt::Checked);
	else
		useStartingGrid->setCheckState(Qt::Unchecked);
	setUseStartingGrid(useStartingGrid->checkState());
	connect(useStartingGrid, SIGNAL(stateChanged(int)), this, SLOT(setUseStartingGrid(int)));

	// skill type box
	useSkill = new QRadioButton(tr("Single skill value"));
	useACR = new QRadioButton(tr("Acceleration, cornering, and reliability"));
	if(options.value("skillType") != "attributes")
		useSkill->setChecked(true);
	else
		useACR->setChecked(true);
	skillType = new QButtonGroup;
	skillType->addButton(useSkill);
	skillType->addButton(useACR);
	connect(skillType, &QButtonGroup::buttonClicked, this, &XkorAutoRacingParadigmOptions::setSkillType);

	QFormLayout * skillTypeForm = new QFormLayout;
	skillTypeForm->addRow(useSkill);
	skillTypeForm->addRow(useACR);

	// QFormLayout sucks, so we create our own label
	QLabel * label = new QLabel(tr("Skill type:"));
	label->setContentsMargins(0, -4, 0, 0);

	QGridLayout * lapLayout = new QGridLayout;
	lapLayout->addWidget(lapRecord, 0, 0);
	lapLayout->addWidget(new QLabel(tr("Variance:")), 0, 1);
	lapLayout->addWidget(lapVariance, 0, 2);
	lapLayout->setContentsMargins(0, 0, 0, 0);
	lapLayout->setColumnStretch(0, 1);
	lapLayout->setColumnStretch(1, 0); // don’t stretch the label
	lapLayout->setColumnStretch(2, 1);

	QGridLayout * modifiersLayout = new QGridLayout;
	modifiersLayout->addWidget(trackAcceleration, 0, 0);
	modifiersLayout->addWidget(new QLabel(tr("Cornering:")), 0, 1);
	modifiersLayout->addWidget(trackCornering, 0, 2);
	modifiersLayout->setContentsMargins(0, 0, 0, 0);
	modifiersLayout->setColumnStretch(0, 1);
	modifiersLayout->setColumnStretch(1, 0); // don’t stretch the label
	modifiersLayout->setColumnStretch(2, 1);

	QFormLayout * form = new QFormLayout(this);
	form->addRow(tr("Number of laps:"), laps);
	form->addRow(tr("Lap record:"), lapLayout);
	form->addRow(label, skillTypeForm);
	form->addRow(tr("Acceleration:"), modifiersLayout);
	form->addRow(QString(), useStartingGrid);
form->addRow(QString(), showTLAs);
}

XkorAutoRacingParadigmOptions::~XkorAutoRacingParadigmOptions()
{
	delete laps;
	delete lapRecord;
	delete lapVariance;
	delete trackAcceleration;
	delete trackCornering;
	delete showTLAs;
	delete useACR;
	delete useSkill;
	delete skillType;
}

void XkorAutoRacingParadigmOptions::setLaps(int x)
{
	options.insert("laps", x);
	emit optionsChanged(options);
}

void XkorAutoRacingParadigmOptions::setLapRecord(double x)
{
	options.insert("lapRecord", x);
	emit optionsChanged(options);
}

void XkorAutoRacingParadigmOptions::setLapVariance(double x)
{
	options.insert("lapVariance", x);
	emit optionsChanged(options);
}

void XkorAutoRacingParadigmOptions::setShowTLAs(int x)
{
	if(x == Qt::Checked)
		options.insert("showTLAs", "true");
	else
		options.insert("showTLAs", "false");
	emit optionsChanged(options);
}

void XkorAutoRacingParadigmOptions::setSkillType()
{
	QAbstractButton * checkedButton = skillType->checkedButton();
	if(checkedButton == useSkill)
	{
		options.insert("skillType", "skill");
		trackAcceleration->setEnabled(false);
		trackCornering->setEnabled(false);
	}
	else
	{
		options.insert("skillType", "attributes");
		trackAcceleration->setEnabled(true);
		trackCornering->setEnabled(true);
	}

	emit optionsChanged(options);
}

void XkorAutoRacingParadigmOptions::setTrackAcceleration(double x)
{
	options.insert("trackAcceleration", x);
	emit optionsChanged(options);
}

void XkorAutoRacingParadigmOptions::setTrackCornering(double x)
{
	options.insert("trackCornering", x);
	emit optionsChanged(options);
}

void XkorAutoRacingParadigmOptions::setUseStartingGrid(int x)
{
	if(x == Qt::Checked)
		options.insert("useStartingGrid", "true");
	else
		options.insert("useStartingGrid", "false");
	emit optionsChanged(options);
}
