#include "competitions/options/roundrobincompetitionoptions.h"
#include <QLabel>
#include <QFormLayout>

#include <algorithm>

XkorRoundRobinCompetitionOptions::XkorRoundRobinCompetitionOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
	// set up basic options
	bool draws = options.value("allowDraws", true).toBool();
	int legs = options.value("numberOfLegs", 1).toInt();
	bool grid = options.value("showResultsGrid", false).toBool();
	
	numberOfLegs = new QSpinBox;
	numberOfLegs->setValue(legs);
	numberOfLegs->setMinimum(1);
	numberOfLegs->setMaximum(99999);
	setNumberOfLegs(numberOfLegs->value());
	connect(numberOfLegs, SIGNAL(valueChanged(int)), this, SLOT(setNumberOfLegs(int)));
	connect(numberOfLegs, SIGNAL(valueChanged(int)), this, SLOT(checkResultsGridUsability(int)));

	allowDraws = new QCheckBox(tr("Allow draws"));
	allowDraws->setChecked(draws);
	connect(allowDraws, SIGNAL(stateChanged(int)), this, SLOT(setAllowDraws(int)));
	
	// set up table options
	int pointsForWin = (options.contains("pointsForWin") ? options.value("pointsForWin").toInt() : 3);
	int pointsForDraw = (options.contains("pointsForDraw") ? options.value("pointsForDraw").toInt() : 1);
	int pointsForLoss = (options.contains("pointsForLoss") ? options.value("pointsForLoss").toInt() : 0);
	
	std::vector<QString> sortCriteria;
	if(options.contains("sortCriteria"))
	{
		QList<QVariant> oldCriteria = options.value("sortCriteria").toList();
		for(QList<QVariant>::iterator i = oldCriteria.begin(); i != oldCriteria.end(); ++i)
			sortCriteria.push_back(i->toString());
	}
	else
		sortCriteria = XkorSortCriteriaWidget::defaultSortCriteria();

	sc = new XkorSortCriteriaWidget;
	sc->setSortCriteria(sortCriteria);
	connect(sc, SIGNAL(listChanged()), this, SLOT(setSortCriteria()));

	ptsWin = new QSpinBox;
	ptsWin->setValue(pointsForWin);
	connect(ptsWin, SIGNAL(valueChanged(int)), this, SLOT(setPointsForWin(int)));

	ptsDraw = new QSpinBox;
	ptsDraw->setValue(pointsForDraw);
	connect(ptsDraw, SIGNAL(valueChanged(int)), this, SLOT(setPointsForDraw(int)));

	ptsLoss = new QSpinBox;
	ptsLoss->setValue(pointsForLoss);
	connect(ptsLoss, SIGNAL(valueChanged(int)), this, SLOT(setPointsForLoss(int)));

	showResultsGrid = new QCheckBox(tr("Show results grid"));
	showResultsGrid->setChecked(grid);
	checkResultsGridUsability(legs);
	connect(showResultsGrid, SIGNAL(stateChanged(int)), this, SLOT(setShowResultsGrid(int)));
	
	// layout
	QGridLayout * pointsLayout = new QGridLayout;
	pointsLayout->addWidget(ptsWin, 0, 0);
	pointsLayout->addWidget(new QLabel(tr("Draw:")), 0, 1);
	pointsLayout->addWidget(ptsDraw, 0, 2);
	pointsLayout->addWidget(new QLabel(tr("Loss:")), 0, 3);
	pointsLayout->addWidget(ptsLoss, 0, 4);
	pointsLayout->setContentsMargins(0, 0, 0, 0);
	pointsLayout->setColumnStretch(0, 1);
	pointsLayout->setColumnStretch(1, 0); // don’t stretch the labels
	pointsLayout->setColumnStretch(2, 1);
	pointsLayout->setColumnStretch(3, 0); // don’t stretch the labels
	pointsLayout->setColumnStretch(4, 1);

	QFormLayout * layout = new QFormLayout(this);
	layout->addRow(tr("Number of legs:"), numberOfLegs);
	layout->addRow(QString(), allowDraws);
layout->addRow(QString(), showResultsGrid);
	layout->addRow(tr("Table sort rules:"), sc);
	layout->addRow(tr("Points for win:"), pointsLayout);
}

void XkorRoundRobinCompetitionOptions::checkResultsGridUsability(int legs)
{
	// we want to preserve the state of the check box even if it’s disabled, but we don’t want the results grid to actually be shown if it should be disabled
	if(legs == 2)
	{
		showResultsGrid->setEnabled(true);
		options["showResultsGrid"] = (showResultsGrid->checkState() == Qt::Checked);
	}
	else
	{
		showResultsGrid->setEnabled(false);
		options["showResultsGrid"] = "false";
	}
}

void XkorRoundRobinCompetitionOptions::setAllowDraws(int value)
{
	if(value == Qt::Checked)
		options["allowDraws"] = "true";
	else
		options["allowDraws"] = "false";
	emit optionsChanged(options);
}

void XkorRoundRobinCompetitionOptions::setNumberOfLegs(int value)
{
	options.insert("numberOfLegs", QString::number(value));
	emit optionsChanged(options);
}

void XkorRoundRobinCompetitionOptions::setPointsForDraw(int value)
{
	options.insert("pointsForDraw", QString::number(value));
	emit optionsChanged(options);
}

void XkorRoundRobinCompetitionOptions::setPointsForLoss(int value)
{
	options.insert("pointsForLoss", QString::number(value));
	emit optionsChanged(options);
}

void XkorRoundRobinCompetitionOptions::setPointsForWin(int value)
{
	options.insert("pointsForWin", QString::number(value));
	emit optionsChanged(options);
}

void XkorRoundRobinCompetitionOptions::setShowResultsGrid(int value)
{
	if(value == Qt::Checked)
		options["showResultsGrid"] = "true";
	else
		options["showResultsGrid"] = "false";
	emit optionsChanged(options);
}

void XkorRoundRobinCompetitionOptions::setSortCriteria()
{
	QList<QVariant> sortCriteria;
	std::vector<QString> criteriaVector = sc->sortCriteria();
	for(std::vector<QString>::iterator i = criteriaVector.begin(); i != criteriaVector.end(); ++i)
		sortCriteria.append(*i);
	options.insert("sortCriteria", sortCriteria);
	emit optionsChanged(options);
}
