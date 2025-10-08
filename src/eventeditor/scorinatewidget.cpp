#include "eventeditor/scorinatewidget.h"
#include <QToolBar>
#include <QMessageBox>

#include <QtGui>
#include <ctime>
#include "sport.h"
#include "xml/xmlindex.h"
#include "xml/xmlsportreader.h"

XkorScorinateWidget::XkorScorinateWidget(QWidget * parent) : QWidget(parent)
{
	c = 0;
	lastMatchday = -1;

	r.seed(time(0));
	dialog = 0;

	// label
	QFont headingFont;
	headingFont.setWeight(QFont::Bold);
	QLabel * label = new QLabel(tr("Scorinate event"));
	label->setFont(headingFont);

	// actions for scorinate widget buttons
	scorinateAction = new QAction(QIcon(":/icons/roll"), tr("Scorinate event"), this);
	connect(scorinateAction, SIGNAL(triggered()), this, SLOT(scorinate()));
	connect(scorinateAction, SIGNAL(triggered()), this, SLOT(updateButtons()));

	exportResultsAction = new QAction(QIcon(":/icons/document-export"), tr("Export to file"), this);
	exportResultsAction->setEnabled(false);
	connect(exportResultsAction, SIGNAL(triggered()), this, SLOT(exportResults()));

	// toolbar
	QToolBar * toolBar = new QToolBar;
	toolBar->setIconSize(QSize(this->style()->pixelMetric(QStyle::PM_SmallIconSize), this->style()->pixelMetric(QStyle::PM_SmallIconSize)));
	toolBar->addAction(scorinateAction);
	toolBar->addAction(exportResultsAction);

	matchday = new QComboBox;
	connect(matchday, SIGNAL(currentIndexChanged(int)), this, SLOT(updateButtons()));
	connect(matchday, SIGNAL(currentIndexChanged(int)), this, SLOT(updateResults(int)));

	textedit = new QPlainTextEdit;
	textedit->setReadOnly(true);
	QFont font;
	font.setStyleHint(QFont::TypeWriter);
	textedit->setFont(font.defaultFamily());
	
	layout = new QVBoxLayout(this);
	layout->addWidget(label, 0, Qt::AlignCenter);
	layout->addWidget(matchday, 0, Qt::AlignHCenter);
	layout->addWidget(textedit, 1);
	layout->addWidget(toolBar, 0, Qt::AlignCenter);

	layout->setContentsMargins(0, 0, 0, 0);
}

XkorScorinateWidget::~XkorScorinateWidget()
{
	delete layout;
	delete matchday;
	delete textedit;
	delete c;
}

void XkorScorinateWidget::clear()
{
	e = XkorEvent();
	rp = XkorRPList();
	s = XkorSport();
	sl = XkorStartList();
	textedit->setPlainText("");
	matchday->clear();
	matchday->hide();
}

void XkorScorinateWidget::exportResults()
{
	if(!dialog)
	{
		dialog = new QFileDialog(this);
		dialog->setWindowTitle(tr("Save results"));
		dialog->setNameFilter(tr("Text files (*.txt)"));
		dialog->setDefaultSuffix("txt");
		dialog->setWindowModality(Qt::WindowModal);
		dialog->setAcceptMode(QFileDialog::AcceptSave);
	}
	dialog->setDirectory("resultsExport:/");
	dialog->open(this, SLOT(exportResults(QString)));
}

void XkorScorinateWidget::exportResults(QString filename)
{
	if(filename != QString())
	{
		QFile f(filename);
		f.open(QIODevice::WriteOnly);
		QTextStream ost(&f);
		
		ost << textedit->document()->toPlainText();

		QDir path(filename);
		path.cdUp(); // go up from the file to the directory
		dialog->setDirectory(path);
		emit resultExportDirectoryChanged(path.canonicalPath());
	}
}

void XkorScorinateWidget::scorinate()
{
	if(matchday->currentIndex() <= lastMatchday)
	{
		int result;
		if(matchday->currentIndex() == lastMatchday)
		{
			QMessageBox warning(QMessageBox::NoIcon, tr("xkoranate"), tr("Are you sure you want to regenerate %1?").arg(matchday->currentText()), QMessageBox::Ok | QMessageBox::Cancel, this);
			int iconSize = style()->pixelMetric(QStyle::PM_MessageBoxIconSize);
			warning.setIconPixmap(QPixmap(":/icons/xkoranate").scaled(iconSize, iconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
			warning.setDefaultButton(QMessageBox::Cancel);
			warning.setEscapeButton(QMessageBox::Cancel);
			warning.setInformativeText(tr("You will lose your existing results for this round if you regenerate them."));
			warning.setWindowModality(Qt::WindowModal);
			result = warning.exec();
		}
		else
		{
			QMessageBox warning(QMessageBox::Warning, tr("xkoranate"), tr("Are you sure you want to regenerate %1 and lose all subsequent results?").arg(matchday->currentText()), QMessageBox::Ok | QMessageBox::Cancel, this);
			warning.setDefaultButton(QMessageBox::Cancel);
			warning.setEscapeButton(QMessageBox::Cancel);
			warning.setInformativeText(tr("You will lose your current results for %1 through %2.").arg(matchday->currentText()).arg(matchday->itemText(lastMatchday)));
			warning.setWindowModality(Qt::WindowModal);
			result = warning.exec();
		}
		if(result == QMessageBox::Cancel)
			return;
		else
		{
			// now that we’ve got permission, back up to the matchday we’re going to resim
			lastMatchday = matchday->currentIndex() - 1;
			QHash<QString, QVariant> resumeOpts = c->revertToMatchday(matchday->currentIndex());
			updateCompetition(resumeOpts, lastMatchday, c->results(lastMatchday));
		}
	}
	if(matchday->currentIndex() > 0 && c->results(matchday->currentIndex() - 1) == "")
	{
		QMessageBox warning(QMessageBox::NoIcon, tr("xkoranate"), tr("Do you want to generate results for %1 through %2?").arg(matchday->itemText(lastMatchday + 1)).arg(matchday->currentText()), QMessageBox::Ok | QMessageBox::Cancel, this);
		int iconSize = style()->pixelMetric(QStyle::PM_MessageBoxIconSize);
		warning.setIconPixmap(QPixmap(":/icons/xkoranate").scaled(iconSize, iconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
		warning.setDefaultButton(QMessageBox::Cancel);
		warning.setEscapeButton(QMessageBox::Cancel);
		warning.setInformativeText(tr("This will generate %1 rounds of results.").arg(matchday->currentIndex() - lastMatchday));
		warning.setWindowModality(Qt::WindowModal);
		int result = warning.exec();
		if(result == QMessageBox::Cancel)
			return;
	}

	for(int i = lastMatchday + 1; i <= matchday->currentIndex(); ++i)
		scorinateMatchday(i);
}

void XkorScorinateWidget::scorinateMatchday(int md)
{
	// scorinate
	c->scorinate(md);
	updateResults(matchday->currentIndex()); // we should show the results for the selected matchday, not those we just scorinated

	// resume file
	QHash<QString, QVariant> resumeOpts = c->resumeFileOptions();
	lastMatchday = md;

	// set resume file options
	emit resumeFileOptionsSet(resumeOpts);
	emit resultConfirmed(md, c->results(md));
	updateCompetition(resumeOpts, md, c->results(md));
	updateButtons();
}

void XkorScorinateWidget::setEvent(XkorEvent event, XkorRPList rpList, XkorSport sport)
{
	sport.setPRNG(&r);

	e = event;
	rp = rpList;
	s = sport;

	// create the list of participants
	sl = event.makeStartList(&rpList /*, (e.paradigmOptions().value("showTLAs", "true").toString() == "true")*/);

	// create the competition
	delete c;
	c = XkorCompetitionFactory::newCompetition(event.competition(), sl, sport, event.paradigmOptions(), event.competitionOptions(), event.results());

	matchday->clear();
	QStringList matchdayNames = c->matchdayNames();
	for(int i = 0; i < matchdayNames.size(); ++i)
		matchday->insertItem(i, matchdayNames[i]);
	if(matchdayNames.size() <= 1)
		matchday->hide();
	else
		matchday->show();

	lastMatchday = -1;
	for(int i = 0; i < matchdayNames.size(); ++i)
		if(c->results(i) != "")
			lastMatchday = i;
}

void XkorScorinateWidget::updateButtons()
{
	if(e.results().value(matchday->currentIndex()) == "")
		exportResultsAction->setEnabled(false);
	else
		exportResultsAction->setEnabled(true);
}

void XkorScorinateWidget::updateCompetition(QHash<QString, QVariant> resumeFileOptions, int matchday, QString result)
{
	e.replaceCompetitionOptions(resumeFileOptions);
	e.setResult(matchday, result);
	delete c;
	c = XkorCompetitionFactory::newCompetition(e.competition(), sl, s, e.paradigmOptions(), e.competitionOptions(), e.results());
}

void XkorScorinateWidget::updateResults(int matchday)
{
	textedit->setPlainText(c->results(matchday));
}
