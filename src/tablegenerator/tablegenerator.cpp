#include "tablegenerator/tablegenerator.h"
#include <QLabel>
#include <QFormLayout>
#include <QGridLayout>
#include <QStyle>
#include <QRegularExpression>
#include <QMessageBox>

#include <algorithm>
#include "xml/xmltablereader.h"
#include "xml/xmltablewriter.h"

XkorTableGenerator::XkorTableGenerator(QWidget * parent) : QWidget(parent)
{
	dialog = new QFileDialog(nullptr, tr("Save table file"), "tables:/", tr("XML files (*.xml)"));
	dialog->setWindowModality(Qt::WindowModal);
	dialog->setDefaultSuffix("xml");

	importDialog = new QFileDialog(nullptr, tr("Import results file"), "resultsImport:/", tr("Text files (*.txt)"));
	importDialog->setWindowModality(Qt::WindowModal);
	importDialog->setAcceptMode(QFileDialog::AcceptOpen);

	pointsForWin = new QSpinBox;
	connect(pointsForWin, SIGNAL(valueChanged(int)), this, SLOT(setFileModified()));
	pointsForDraw = new QSpinBox;
	connect(pointsForDraw, SIGNAL(valueChanged(int)), this, SLOT(setFileModified()));
	pointsForLoss = new QSpinBox;
	connect(pointsForLoss, SIGNAL(valueChanged(int)), this, SLOT(setFileModified()));
	columnWidth = new QSpinBox;
	connect(columnWidth, SIGNAL(valueChanged(int)), this, SLOT(setFileModified()));

	scw = new XkorSortCriteriaWidget;
	connect(scw, SIGNAL(listChanged()), this, SLOT(setFileModified()));

	showDraws = new QCheckBox(tr("Draws"));
	connect(showDraws, SIGNAL(stateChanged(int)), this, SLOT(setFileModified()));
	showResultsGrid = new QCheckBox(tr("Results grid"));
	connect(showResultsGrid, SIGNAL(stateChanged(int)), this, SLOT(setFileModified()));

	goalName = new QComboBox;
	goalName->addItem(tr("Goals (G)"), "G");
	goalName->addItem(tr("Korfs (K)"), "K");
	goalName->addItem(tr("Points (P)"), "P");
	goalName->addItem(tr("Runs (R)"), "R");
	connect(goalName, SIGNAL(currentIndexChanged(int)), this, SLOT(setFileModified()));

	matches = new QPlainTextEdit;
	connect(matches, SIGNAL(textChanged()), this, SLOT(setFileModified()));
	connect(matches, SIGNAL(textChanged()), this, SLOT(setMatchesModified()));

	table = new QPlainTextEdit;
	QFont font;
	font.setStyleHint(QFont::TypeWriter);
	table->setFont(font.defaultFamily());
	table->setReadOnly(true);

	QGridLayout * pointsLayout = new QGridLayout;
	pointsLayout->addWidget(pointsForWin, 0, 0);
	pointsLayout->addWidget(new QLabel(tr("Draw:")), 0, 1);
	pointsLayout->addWidget(pointsForDraw, 0, 2);
	pointsLayout->addWidget(new QLabel(tr("Loss:")), 0, 3);
	pointsLayout->addWidget(pointsForLoss, 0, 4);
	pointsLayout->setContentsMargins(0, 0, 0, 0);
	pointsLayout->setColumnStretch(0, 1);
	pointsLayout->setColumnStretch(1, 0); // don’t stretch the labels
	pointsLayout->setColumnStretch(2, 1);
	pointsLayout->setColumnStretch(3, 0); // don’t stretch the labels
	pointsLayout->setColumnStretch(4, 1);

	QFormLayout * form = new QFormLayout;
	form->addRow(tr("Table sort rules:"), scw);
	form->addRow(tr("Points for win:"), pointsLayout);
	form->addRow(tr("Show columns:"), showDraws);
	form->addRow("", showResultsGrid);
	form->addRow(tr("Goals are called:"), goalName);
	form->addRow(tr("Column width:"), columnWidth);

	QLabel * matchResultsLabel = new QLabel("Match results:");

	QGridLayout * layout = new QGridLayout(nullptr);
	layout->addWidget(matchResultsLabel, 0, 0, Qt::AlignTop);
	layout->addWidget(matches, 0, 1);
	layout->addLayout(form, 0, 2);
	layout->addWidget(table, 1, 0, 1, 3); // rowspan = 3

	setFileModified(false);
	reset();
}

XkorTableGenerator::~XkorTableGenerator()
{
	delete matches;
	delete table;
	delete pointsForWin;
	delete pointsForDraw;
	delete pointsForLoss;
	delete columnWidth;
	delete scw;
	delete showDraws;
	delete showResultsGrid;
	delete goalName;
	delete dialog;
	delete importDialog;
}

void XkorTableGenerator::closeEvent(QCloseEvent * event)
{
	if(okayToLoad())
		
	event->ignore();
}

void XkorTableGenerator::generateMatches()
{
	matchesList.clear();
	teamsList.clear();

	QString text = matches->toPlainText();
	QTextStream ist(&text);

	while(!ist.atEnd())
	{
		QString line = ist.readLine();
		QRegularExpression rx("([0-9]+)[-–:]([0-9]+)"); // match scores of form Aquilla 3–1 Busby, with en dash, hyphen-minus, or colon as delimiter
		QRegularExpressionMatch match = rx.match(line);
		if(match.hasMatch())
		{
			QString homeTeam = line.left(match.capturedStart() - 1);
			QString awayTeam = line.right(line.size() - match.capturedStart() - match.capturedLength() - 1);
			double homeScore = match.captured(1).toDouble();
			double awayScore = match.captured(2).toDouble();
			matchesList.push_back(XkorTableMatch(homeTeam, awayTeam, homeScore, awayScore));
			if(!teamsList.contains(homeTeam))
				teamsList.append(homeTeam);
			if(!teamsList.contains(awayTeam))
				teamsList.append(awayTeam);
		}
	}
}

void XkorTableGenerator::generateTable()
{
	updateTable();
	t.generate();
	table->setPlainText(t.toText());
	matchesModified = false;
}

std::vector<XkorTableColumn> XkorTableGenerator::generateTableColumns()
{
	std::vector<XkorTableColumn> columns;

	int nameWidth = 20;
	for(QStringList::iterator i = teamsList.begin(); i != teamsList.end(); ++i)
		if(i->size() > nameWidth)
			nameWidth = i->size();
	int matchdayWidth = columnWidth->value();
	int teams = teamsList.size();
	int positionWidth = log10(teams) + 1;
	QString chosenGoalName = goalName->itemData(goalName->currentIndex()).toString();

	std::vector<QString> sortCriteria = scw->sortCriteria();
	bool usesGoalAverage = (std::find(sortCriteria.begin(), sortCriteria.end(), "goalAverage") != sortCriteria.end());
	bool usesGoalDifference = (std::find(sortCriteria.begin(), sortCriteria.end(), "goalDifference") != sortCriteria.end());
	bool usesGoalsAgainst = (std::find(sortCriteria.begin(), sortCriteria.end(), "goalsAgainst") != sortCriteria.end());
	bool usesGoalsFor = (std::find(sortCriteria.begin(), sortCriteria.end(), "goalsFor") != sortCriteria.end());
	bool usesWinPercent = (std::find(sortCriteria.begin(), sortCriteria.end(), "winPercent") != sortCriteria.end());
	bool usesWinPercentPure = (std::find(sortCriteria.begin(), sortCriteria.end(), "winPercentPure") != sortCriteria.end());
	bool usesWinPercentNFL = (std::find(sortCriteria.begin(), sortCriteria.end(), "winPercentNFL") != sortCriteria.end());
	bool usesPoints = (std::find(sortCriteria.begin(), sortCriteria.end(), "points") != sortCriteria.end());

	columns.push_back(XkorTableColumn("position", "", positionWidth));
	columns.push_back(XkorTableColumn("name", "", nameWidth));
	columns.push_back(XkorTableColumn("played", "Pld", matchdayWidth + 2));
	columns.push_back(XkorTableColumn("wins", "W", matchdayWidth + 2));
	if(showDraws->checkState() == Qt::Checked)
		columns.push_back(XkorTableColumn("draws", "D", matchdayWidth + 1));
	columns.push_back(XkorTableColumn("losses", "L", matchdayWidth + 1));
	if(usesGoalAverage || usesGoalDifference || usesGoalsAgainst || usesGoalsFor)
	{
		columns.push_back(XkorTableColumn("goalsFor", chosenGoalName + "F", matchdayWidth + 3));
		columns.push_back(XkorTableColumn("goalsAgainst", chosenGoalName + "A", matchdayWidth + 2));
	}
	if(usesGoalAverage)
		columns.push_back(XkorTableColumn("goalAverage", "Avg", 7));
	if(usesGoalDifference)
		columns.push_back(XkorTableColumn("goalDifference", chosenGoalName + "D", matchdayWidth + 2));
	if(usesWinPercent)
		columns.push_back(XkorTableColumn("winPercent", "Win %", 8));
	if(usesWinPercentPure)
		columns.push_back(XkorTableColumn("winPercentPure", "Win %", 8));
	if(usesWinPercentNFL)
		columns.push_back(XkorTableColumn("winPercentNFL", "Win %", 8));
	if(usesPoints)
		columns.push_back(XkorTableColumn("points", "Pts", matchdayWidth + 3));
	if(showResultsGrid->checkState() == Qt::Checked)
		columns.push_back(XkorTableColumn("resultsGrid", "", teams * 5));

	return columns;
}

void XkorTableGenerator::importResults()
{
	importDialog->setDirectory("resultsImport:/");
	importDialog->open(this, SLOT(importResults(QString)));
}

void XkorTableGenerator::importResults(QString filename)
{
	QFile f(filename);
	f.open(QFile::ReadOnly);
	matches->appendPlainText(QString::fromUtf8(f.readAll()));
	generateTable();

	QDir path(filename);
	path.cdUp();
	emit resultImportDirectoryChanged(path.canonicalPath());
}

bool XkorTableGenerator::okayToLoad()
{
	if(fileModified)
	{
		int r = showUnsavedDialog();
		if(r == QMessageBox::Cancel)
			return false;
	}
	return true; // if the current file hasn’t been modified, or if the user clicked Yes or No
}

void XkorTableGenerator::openFile()
{
	if(okayToLoad())
	{
		dialog->setAcceptMode(QFileDialog::AcceptOpen);
		dialog->setDirectory("tables:/");
		dialog->open(this, SLOT(openFile(QString)));
	}
}

void XkorTableGenerator::openFile(QString filename)
{
	XkorXmlTableReader r(filename);
	if(r.hasError())
		QMessageBox::critical(nullptr, tr("xkoranate"), r.error());
	else
	{
		t = r.table();

		matches->setPlainText(r.matches());
		pointsForWin->setValue(t.getPointsForWin());
		pointsForDraw->setValue(t.getPointsForDraw());
		pointsForLoss->setValue(t.getPointsForLoss());
		columnWidth->setValue(t.getColumnWidth());
		scw->setSortCriteria(t.getSortCriteria());
		showDraws->setCheckState(t.getShowDraws() ? Qt::Checked : Qt::Unchecked);
		showResultsGrid->setCheckState(t.getShowResultsGrid() ? Qt::Checked : Qt::Unchecked);
		goalName->setCurrentIndex(goalName->findData(t.getGoalName()));

		generateTable();

		currentFileName = filename;
		emit fileChanged(filename);
		setFileModified(false);

		QDir path(filename);
		path.cdUp();
		emit tableDirectoryChanged(path.canonicalPath());
	}
}

void XkorTableGenerator::reset()
{
	if(okayToLoad())
	{
		t = XkorTable();
		matches->setPlainText("");
		pointsForWin->setValue(3);
		pointsForDraw->setValue(1);
		pointsForLoss->setValue(0);
		columnWidth->setValue(2);
		scw->setSortCriteria(scw->defaultSortCriteria());
		showDraws->setCheckState(Qt::Checked);
		showResultsGrid->setCheckState(Qt::Unchecked);
		goalName->setCurrentIndex(goalName->findData("G"));

		table->setPlainText("");

		matchesList = std::vector<XkorTableMatch>();
		teamsList = QStringList();
		matchesModified = false;

		currentFileName = "";
		emit fileChanged("");
		setFileModified(false);
	}
}

void XkorTableGenerator::saveFile(QString filename)
{
	if(filename == QString()) // if no filename was specified
	{
		if(currentFileName == QString()) // still “Untitled”?
			saveFileAs();
		else
			filename = currentFileName;
	}

	updateTable();
	XkorXmlTableWriter w(filename, t);

	currentFileName = filename;
	emit fileChanged(filename);
	setFileModified(false);

	QDir path(filename);
	path.cdUp();
	emit tableDirectoryChanged(path.canonicalPath());
}

void XkorTableGenerator::saveFileAs()
{
	dialog->setAcceptMode(QFileDialog::AcceptSave);
	dialog->setDirectory("tables:/");
	dialog->open(this, SLOT(saveFile(QString)));
}

void XkorTableGenerator::setFileModified(bool isEdited)
{
	fileModified = isEdited;
	emit fileEdited(isEdited);
}

void XkorTableGenerator::setMatchesModified()
{
	matchesModified = true;
}

int XkorTableGenerator::showUnsavedDialog()
{
	QString displayFileName = currentFileName.isEmpty() ? "untitled" : QFileInfo(currentFileName).fileName();
	QMessageBox warning(QMessageBox::NoIcon, tr("xkoranate"), tr("Do you want to save the changes you made to “%1”?").arg(displayFileName), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, nullptr);
	
	
	warning.setInformativeText(tr("Your changes will be lost if you don’t save them."));
	warning.setWindowModality(Qt::WindowModal);
	return warning.exec();
}

void XkorTableGenerator::updateTable()
{
	if(matchesModified)
		generateMatches();
	t.setColumns(generateTableColumns());
	t.setMatches(matchesList);
	t.setPointsForWin(pointsForWin->value());
	t.setPointsForDraw(pointsForDraw->value());
	t.setPointsForLoss(pointsForLoss->value());
	t.setSortCriteria(scw->sortCriteria());
	t.setColumnWidth(columnWidth->value());
	t.setGoalName(goalName->itemData(goalName->currentIndex()).toString());
	t.setShowDraws(showDraws->checkState() == Qt::Checked);
	t.setShowResultsGrid(showResultsGrid->checkState() == Qt::Checked);
}
