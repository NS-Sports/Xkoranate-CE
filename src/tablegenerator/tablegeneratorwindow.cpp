#include "tablegenerator/tablegeneratorwindow.h"
#include <QToolBar>

XkorTableGeneratorWindow::XkorTableGeneratorWindow(QWidget * parent) : QMainWindow(parent)
{
	setWindowTitle(tr("untitled – xkoranate table generator"));

	generator = new XkorTableGenerator;
	setCentralWidget(generator);
	connect(generator, SIGNAL(fileChanged(QString)), this, SLOT(setFileName(QString)));
	connect(generator, SIGNAL(fileEdited(bool)), this, SLOT(setModified(bool)));
	connect(generator, SIGNAL(resultImportDirectoryChanged(QString)), this, SLOT(setResultImportDirectory(QString)));
	connect(generator, SIGNAL(tableDirectoryChanged(QString)), this, SLOT(setTableDirectory(QString)));

	newAction = new QAction(QIcon(":/icons/document-new"), tr("New file"), this);
	newAction->setShortcut(QKeySequence::New);
	connect(newAction, SIGNAL(triggered()), generator, SLOT(reset()));

	openAction = new QAction(QIcon(":/icons/document-open"), tr("Open file…"), this);
	openAction->setShortcut(QKeySequence::Open);
	connect(openAction, SIGNAL(triggered()), generator, SLOT(openFile()));

	saveAction = new QAction(QIcon(":/icons/document-save"), tr("Save file"), this);
	saveAction->setShortcut(QKeySequence::Save);
	connect(saveAction, SIGNAL(triggered()), generator, SLOT(saveFile()));

	saveAsAction = new QAction(QIcon(":/icons/document-save-as"), tr("Save file as…"), this);
	saveAsAction->setShortcut(QKeySequence::SaveAs);
	connect(saveAsAction, SIGNAL(triggered()), generator, SLOT(saveFileAs()));

	importAction = new QAction(QIcon(":/icons/document-import"), tr("Import match results…"), this);
	importAction->setShortcut(Qt::CTRL | Qt::Key_I);
	connect(importAction, SIGNAL(triggered()), generator, SLOT(importResults()));

	generateAction = new QAction(QIcon(":/icons/table-generator-refresh"), tr("Generate table"), this);
	generateAction->setShortcut(QKeySequence::Refresh);
	connect(generateAction, SIGNAL(triggered()), generator, SLOT(generateTable()));

	toolBar = new QToolBar;
	toolBar->setMovable(false);
	toolBar->addAction(newAction);
	toolBar->addAction(openAction);
	toolBar->addSeparator();
	toolBar->addAction(saveAction);
	toolBar->addAction(saveAsAction);
	toolBar->addSeparator();
	toolBar->addAction(importAction);
	toolBar->addAction(generateAction);

	addToolBar(toolBar);
	setUnifiedTitleAndToolBarOnMac(true);
}

void XkorTableGeneratorWindow::closeEvent(QCloseEvent * event)
{
	
		if(generator->okayToLoad())
			QMainWindow::closeEvent(event);
		else
			event->ignore();
}

void XkorTableGeneratorWindow::setFileName(QString filename)
{
	QString displayName = tr("untitled");
	if(filename != "")
		displayName = QFileInfo(filename).fileName();
	setWindowTitle(displayName + tr("[*] – xkoranate table generator"));
	setModified(false);
}

void XkorTableGeneratorWindow::setModified(bool newModified)
{
	setWindowModified(newModified);
}

void XkorTableGeneratorWindow::setResultImportDirectory(QString dir)
{
	emit resultImportDirectoryChanged(dir);
}

void XkorTableGeneratorWindow::setTableDirectory(QString dir)
{
	emit tableDirectoryChanged(dir);
}
