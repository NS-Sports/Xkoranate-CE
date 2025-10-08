#include "eventeditor/eventeditor.h"
#include <QPushButton>

#include "xml/xmlindex.h"
#include "xml/xmlsportreader.h"
#include "eventeditor/sportmodel.h"
#include "paradigms/paradigmfactory.h"
#include "competitions/competitionfactory.h"

XkorEventEditor::XkorEventEditor(QWidget * parent) : QWidget(parent)
{
	selectionModel = 0;
	stack = 0;
	signupListEditor = 0;
	isLoading = false;

	initCompetitionSelector();
	initSportSelector();
	initSignupListEditor();
	initScorinateWidget();

	initEventSetupWidget();
	
	initLayout();
}

void XkorEventEditor::goNext()
{
	prev->setDisabled(false);
	if(stack->currentIndex() == stack->count() - 2) // we’re headed for the scorinate widget
	{
		updateData();
		scorinateWidget->setEvent(m_data, m_rpList, sport);
		next->setDisabled(true);
	}

	if(stack->currentIndex() < stack->count() - 1) // go to the next widget
		stack->setCurrentIndex(stack->currentIndex() + 1);
}

void XkorEventEditor::goPrev()
{
	if(stack->currentIndex() > 0)
		stack->setCurrentIndex(stack->currentIndex() - 1);

	next->setDisabled(false);
	if(stack->currentIndex() == 0)
		prev->setDisabled(true);
}

void XkorEventEditor::initCompetitionSelector()
{
	competitionSelector = new XkorCompetitionSelector;
	connect(competitionSelector, SIGNAL(competitionChanged(QString)), this, SLOT(updateCompetition(QString)));
	connect(competitionSelector, SIGNAL(competitionOptionsChanged(QHash<QString, QVariant>)), this, SLOT(updateCompetitionOptions(QHash<QString, QVariant>)));
}

void XkorEventEditor::initEventSetupWidget()
{
	eventSetupWidget = new XkorEventSetupWidget;
	connect(eventSetupWidget, SIGNAL(listChanged()), this, SLOT(setDataChanged()));
    connect(signupListEditor, SIGNAL(itemDeleted(QUuid)), eventSetupWidget, SLOT(deleteAthlete(QUuid)));
}

void XkorEventEditor::initLayout()
{
	
	// stacked layout
	stack = new QStackedLayout;
	stack->addWidget(sportSelector);
	stack->addWidget(signupListEditor);
	stack->addWidget(competitionSelector);
	stack->addWidget(eventSetupWidget);
	stack->addWidget(scorinateWidget);
	stack->setCurrentWidget(sportSelector);

	// navigation bar
	prev = new QPushButton(tr("Go Back"));
	prev->setDisabled(true);
	connect(prev, SIGNAL(clicked()), this, SLOT(goPrev()));
	next = new QPushButton(tr("Continue"));
	next->setDisabled(true);
	connect(next, &QPushButton::clicked, this, &XkorEventEditor::goNext);

	// main layout
	layout = new QGridLayout(this);
	layout->addLayout(stack, 0, 0, 1, 3);
	layout->addWidget(prev, 1, 1);
	layout->addWidget(next, 1, 2);
	layout->setColumnStretch(0, 1093);
	layout->setColumnStretch(1, 0);
	layout->setColumnStretch(2, 0);
}

void XkorEventEditor::initScorinateWidget()
{
	scorinateWidget = new XkorScorinateWidget();
	connect(scorinateWidget, SIGNAL(resumeFileOptionsSet(QHash<QString, QVariant>)), this, SLOT(setResumeFileOptions(QHash<QString, QVariant>)));
	connect(scorinateWidget, SIGNAL(resultConfirmed(int, QString)), this, SLOT(setResult(int, QString)));
	connect(scorinateWidget, SIGNAL(resultExportDirectoryChanged(QString)), this, SLOT(setResultExportDirectory(QString)));
}

void XkorEventEditor::initSignupListEditor()
{
	signupListEditor = new XkorSignupListEditor();
    connect(signupListEditor, SIGNAL(dataChanged()), this, SLOT(updateSignupList()));
	connect(signupListEditor, SIGNAL(signupListDirectoryChanged(QString)), this, SLOT(setSignupListDirectory(QString)));
}

void XkorEventEditor::initSportSelector()
{
	sportSelector = new XkorSportSelector;
	connect(sportSelector, SIGNAL(sportChanged(XkorSport)), this, SLOT(updateSport(XkorSport)));
	connect(sportSelector, SIGNAL(paradigmOptionsChanged(QHash<QString, QVariant>)), this, SLOT(updateParadigmOptions(QHash<QString, QVariant>)));
}

void XkorEventEditor::loadSports(const QString &sportPath)
{
    sportSelector->updateSportList(sportPath);
}

void XkorEventEditor::setData(XkorEvent data, XkorRPList rpList)
{
	isLoading = true; // prevent dataChanged from being emitted

	m_rpList = rpList;

	// hit the reset button
	eventSetupWidget->clear();
	sportSelector->setSelectedSport("");
	signupListEditor->setData(XkorSignupList());
	competitionSelector->setSport(XkorSport(), QHash<QString, QVariant>());
	eventSetupWidget->setGroups(QList<XkorGroup>());
	scorinateWidget->clear();
	stack->setCurrentIndex(0);

	m_data = data;

	sportSelector->setParadigmOptions(data.paradigmOptions());
	sportSelector->setSelectedSport(data.sport());
	// updateSport is called implicitly by sportSelector->setSelectedSport
	competitionSelector->setSport(sport, data.competitionOptions());
	competitionSelector->setCompetition(data.competition());
	scorinateWidget->setEvent(m_data, m_rpList, sport);

	// signup lists
	signupListEditor->setData(data.signupList());

	// athletes
	eventSetupWidget->setSignupList(data.signupList());
	eventSetupWidget->setGroups(data.groups());

	isLoading = false; // allow dataChanged to be emitted if the user does stuff
}

void XkorEventEditor::setResult(int matchday, QString result)
{
	m_data.setResult(matchday, result);
	setDataChanged();
}

void XkorEventEditor::setResultExportDirectory(QString dir)
{
	emit resultExportDirectoryChanged(dir);
}

void XkorEventEditor::setResumeFileOptions(QHash<QString, QVariant> options)
{
	m_data.replaceCompetitionOptions(options);
	setDataChanged();
}

void XkorEventEditor::setSignupListDirectory(QString dir)
{
	emit signupListDirectoryChanged(dir);
}

void XkorEventEditor::updateCompetition(QString competition)
{
	m_data.setCompetition(competition);
	setDataChanged();
}

void XkorEventEditor::updateCompetitionOptions(QHash<QString, QVariant> options)
{
	m_data.setCompetitionOptions(options);
	setDataChanged();
}

void XkorEventEditor::updateData()
{
	// bring the event up-to-date with whatever’s been going on in the GUI
	m_data.setSignupList(signupListEditor->data());
	m_data.setGroups(eventSetupWidget->groups());
}

void XkorEventEditor::updateParadigmOptions(QHash<QString, QVariant> options)
{
	// update the signup list editor
	signupListEditor->setSport(sport, options);
	m_data.setParadigmOptions(options);
	setDataChanged();
}

void XkorEventEditor::updateSignupList()
{
	m_data.setSignupList(signupListEditor->data());
	eventSetupWidget->setSignupList(signupListEditor->data());
	setDataChanged();
}

void XkorEventEditor::updateSport(XkorSport s)
{
	sport = s;
	m_data.setSport(sport.name(), sport.paradigm());
	if(sport.name() != "")
		next->setEnabled(true);
	else
		next->setEnabled(false);

	// update the signup list editor
	signupListEditor->setSport(sport, m_data.paradigmOptions());
	competitionSelector->setSport(sport, m_data.competitionOptions());
	setDataChanged();
}


