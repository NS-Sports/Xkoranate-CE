#include "application.h"

#include <QDir>
#include <stdexcept>

XkorApplication::XkorApplication(int & c, char * * v) : QApplication(c, v)
{
    tg = NULL;

    settings = new QSettings("thirdgeek.com", "xkoranate");

    setDefaultSettings(); // in case we haven’t run the application before

    mainWindow = new XkorMainWindow;
        mainWindow->setUnifiedTitleAndToolBarOnMac(true);

    cw = new XkorCentralWidget;
    connect(cw, SIGNAL(eventDirectoryChanged(QString)), this, SLOT(setEventDirectory(QString)));
    connect(cw, SIGNAL(fileChanged(QString)), this, SLOT(setFileName(QString)));
    connect(cw, SIGNAL(fileEdited()), this, SLOT(setModified()));
    connect(cw, SIGNAL(resultExportDirectoryChanged(QString)), this, SLOT(setResultExportDirectory(QString)));
    connect(cw, SIGNAL(signupListDirectoryChanged(QString)), this, SLOT(setSignupListDirectory(QString)));

    // set up actions
    newAction = new QAction(QIcon(":/icons/document-new"), XkorApplication::tr("New file…"), this);
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, SIGNAL(triggered()), cw, SLOT(newFile()));

    openAction = new QAction(QIcon(":/icons/document-open"), XkorApplication::tr("Open file…"), this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), cw, SLOT(openFile()));

    saveAction = new QAction(QIcon(":/icons/document-save"), XkorApplication::tr("Save file…"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, SIGNAL(triggered()), cw, SLOT(saveFile()));

    saveAsAction = new QAction(QIcon(":/icons/document-save-as"), XkorApplication::tr("Save file as…"), this);
    saveAsAction->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_S);
    connect(saveAsAction, SIGNAL(triggered()), cw, SLOT(saveFileAs()));

    tableAction = new QAction(QIcon(":/icons/table-generator"), XkorApplication::tr("Table generator"), this);
    tableAction->setShortcut(Qt::CTRL | Qt::Key_T);
    connect(tableAction, SIGNAL(triggered()), this, SLOT(tableGenerator()));

    // toolbar
    toolBar = new QToolBar;
    toolBar->setMovable(false);
    toolBar->addAction(newAction);
    toolBar->addAction(openAction);
    toolBar->addSeparator();
    toolBar->addAction(saveAction);
    toolBar->addAction(saveAsAction);
    toolBar->addSeparator();
    toolBar->addAction(tableAction);

    mainWindow->setCentralWidget(cw);
    mainWindow->addToolBar(toolBar);
    mainWindow->setWindowTitle(tr("untitled[*] – xkoranate"));

#ifdef Q_WS_X11
    mainWindow->setWindowIcon(QIcon(":/icons/xkoranate"));
#endif

    mainWindow->show();

    refreshSearchPaths();
    loadSports(m_sportPath);
}

void XkorApplication::loadSports(const QString &sportPath)
{
    cw->loadSports(sportPath);
}

bool XkorApplication::notify(QObject * rec, QEvent * ev)
{
	try
	{
		return QApplication::notify(rec, ev);
	}
	catch(std::exception & e)
	{
                qFatal("%s", e.what());
		return false;
	}
	catch(...)
	{
		qFatal("Unknown exception!");
		abort();
	}
}



void XkorApplication::refreshSearchPaths()
{
    QDir sportDirectory(QCoreApplication::applicationDirPath());
#ifdef Q_OS_MACOS
    sportDirectory.cdUp(); // go from MacOS/ to Contents/ first
    sportDirectory.cd("Resources/sports/");
#else
    sportDirectory.cd("sports/");
#endif
    m_sportPath = sportDirectory.absolutePath();

    QDir::setSearchPaths("events", settings->value("eventDirectory").toStringList());
    QDir::setSearchPaths("resultsExport", settings->value("resultExportDirectory").toStringList());
    QDir::setSearchPaths("resultsImport", settings->value("resultImportDirectory").toStringList());
    QDir::setSearchPaths("signupLists", settings->value("signupListDirectory").toStringList());
    QDir::setSearchPaths("tables", settings->value("tableDirectory").toStringList());
}

void XkorApplication::setDefaultSettings()
{
	if(!settings->contains("eventDirectory"))
		setEventDirectory(QDir::homePath());
	if(!settings->contains("resultExportDirectory"))
		setResultExportDirectory(QDir::homePath());
	if(!settings->contains("resultImportDirectory"))
		setResultImportDirectory(QDir::homePath());
	if(!settings->contains("signupListDirectory"))
		setSignupListDirectory(QDir::homePath());
	if(!settings->contains("tableDirectory"))
		setTableDirectory(QDir::homePath());
}

void XkorApplication::setEventDirectory(QString dir)
{
	settings->setValue("eventDirectory", dir);
	refreshSearchPaths();
}

void XkorApplication::setFileName(QString filename)
{
	QString displayName = tr("untitled");
	if(filename != "")
		displayName = QFileInfo(filename).fileName();
	mainWindow->setWindowTitle(displayName + tr("[*] – xkoranate"));
	setModified(false);
}

void XkorApplication::setModified(bool newModified)
{
	modified = newModified;
	mainWindow->setWindowModified(newModified);
}

void XkorApplication::setResultExportDirectory(QString dir)
{
	settings->setValue("resultExportDirectory", dir);
	refreshSearchPaths();
}

void XkorApplication::setResultImportDirectory(QString dir)
{
	settings->setValue("resultImportDirectory", dir);
	refreshSearchPaths();
}

void XkorApplication::setSignupListDirectory(QString dir)
{
	settings->setValue("signupListDirectory", dir);
	refreshSearchPaths();
}

void XkorApplication::setTableDirectory(QString dir)
{
	settings->setValue("tableDirectory", dir);
	refreshSearchPaths();
}

void XkorApplication::tableGenerator()
{
	delete tg;
	tg = new XkorTableGeneratorWindow;
	connect(tg, SIGNAL(resultImportDirectoryChanged(QString)), this, SLOT(setResultImportDirectory(QString)));
	connect(tg, SIGNAL(tableDirectoryChanged(QString)), this, SLOT(setTableDirectory(QString)));
	tg->show();
}
