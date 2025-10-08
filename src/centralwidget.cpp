#include "centralwidget.h"
#include <QMessageBox>

#include <QtGui>
#include "exceptions.h"
#include "thinsplitter.h"
#include "xml/xmlreader.h"
#include "xml/xmlwriter.h"

XkorCentralWidget::XkorCentralWidget(QWidget * parent) : QWidget(parent)
{
	currentEvent = 0;
	modified = false;

	dialog = new QFileDialog(this, tr("Save scorinator file"), "events:/", tr("XML files (*.xml)"));
	dialog->setDefaultSuffix("xml");
	dialog->setWindowModality(Qt::WindowModal);

	nw = new XkorNavigationWidget;
	editor = new QStackedWidget;

	// set up editor widgets
	rpe = new XkorRpEditor;
	rpe->setData(nw->rpList());
	connect(rpe, SIGNAL(dataChanged()), this, SLOT(setModified()));
	editor->addWidget(rpe);
	editor->setCurrentWidget(rpe);
	connect(nw, SIGNAL(listChanged()), this, SLOT(setModified()));
	connect(nw, SIGNAL(editRPList(XkorRPList *)), rpe, SLOT(setData(XkorRPList *)));
	connect(nw, SIGNAL(editRPList(XkorRPList *)), this, SLOT(showRPEditor()));

	ee = new XkorEventEditor;
	connect(ee, SIGNAL(dataChanged()), this, SLOT(setModified()));
	connect(ee, SIGNAL(resultExportDirectoryChanged(QString)), this, SLOT(setResultExportDirectory(QString)));
	connect(ee, SIGNAL(signupListDirectoryChanged(QString)), this, SLOT(setSignupListDirectory(QString)));
	editor->addWidget(ee);
	connect(nw, SIGNAL(editEvent(XkorEvent *)), this, SLOT(showEventEditor(XkorEvent *)));

	XkorThinSplitter * splitter = new XkorThinSplitter;
	splitter->addWidget(nw);
	splitter->addWidget(editor);
	splitter->setStretchFactor(0, 0);
	splitter->setStretchFactor(1, 1093); // only the main screen should stretch

	QGridLayout * layout = new QGridLayout(this);
	layout->addWidget(splitter);
	layout->setContentsMargins(0, 0, 0, 0);
}

void XkorCentralWidget::closeEvent(QCloseEvent * event)
{
	if(okayToLoad())
		QWidget::closeEvent(event);
	else
		event->ignore();
}

QList<QPair<QUuid, XkorEvent *> > XkorCentralWidget::events()
{
	updateCurrentEvent();
	return nw->events();
}

void XkorCentralWidget::loadSports(const QString &sportPath)
{
    ee->loadSports(sportPath);
}

void XkorCentralWidget::newFile()
{
	if(okayToLoad())
	{
		XkorRPList * rpl = new XkorRPList;
		QList<QPair<QUuid, XkorEvent *> > events;

		rpe->setData(rpl);
		ee->setData(XkorEvent(), XkorRPList());
		nw->reset();
		currentEvent = 0;

		nw->setRPList(rpl);
		nw->setEvents(events);

		editor->setCurrentWidget(rpe);

		currentFileName = "";
		emit fileChanged(currentFileName);
		setModified(false);
	}
}

bool XkorCentralWidget::okayToLoad()
{
	if(modified)
	{
		int result = showUnsavedDialog();
		if(result == QMessageBox::Save)
			saveFile();
		else if(result == QMessageBox::Cancel)
			return false;
	}
	return true;
}

void XkorCentralWidget::openFile()
{
	if(okayToLoad())
	{
		dialog->setAcceptMode(QFileDialog::AcceptOpen);
		dialog->setDirectory("events:/");
		dialog->open(this, SLOT(openFile(QString)));
	}
}

void XkorCentralWidget::openFile(QString filename)
{
	if(filename != "")
	{
		XkorXmlReader r(filename);
		if(r.hasError())
			QMessageBox::critical(this, tr("xkoranate"), r.error());
		else
		{
			XkorRPList * rpl = r.rpList();
			QList<QPair<QUuid, XkorEvent *> > events = r.events();

			// wipe the slate clean
			rpe->setData(NULL);
			ee->setData(XkorEvent(), XkorRPList());
			nw->reset();
			currentEvent = 0;

			// load the new data
			nw->setRPList(rpl);
			nw->setEvents(events);

			//editor->setCurrentWidget(rpe); // show the default screen, which is the RP editor

			currentFileName = filename;

			emit fileChanged(filename);
			setModified(false);

			QDir path(filename);
			path.cdUp();
			emit eventDirectoryChanged(path.canonicalPath());
		}
	}
}

XkorRPList * XkorCentralWidget::rpList()
{
	rpe->updateData();
	return nw->rpList();
}

void XkorCentralWidget::saveFile(QString filename)
{
	if(filename == QString()) // if no filename was specified
	{
		if(currentFileName == QString()) // still “Untitled”?
			saveFileAs();
		else
			filename = currentFileName;
	}
	QFile f(filename);

	XkorXmlWriter w(filename, rpList(), events());

	currentFileName = filename;
	emit fileChanged(filename);
	setModified(false);

	QDir path(filename);
	path.cdUp();
	emit eventDirectoryChanged(path.canonicalPath());
}

void XkorCentralWidget::saveFileAs()
{
	dialog->setAcceptMode(QFileDialog::AcceptSave);
	dialog->setDirectory("events:/");
	dialog->open(this, SLOT(saveFile(QString)));
}

void XkorCentralWidget::setModified(bool isModified)
{
	modified = isModified;
	if(isModified)
		emit fileEdited();
}

void XkorCentralWidget::setResultExportDirectory(QString dir)
{
	emit resultExportDirectoryChanged(dir);
}

void XkorCentralWidget::setSignupListDirectory(QString dir)
{
	emit signupListDirectoryChanged(dir);
}

void XkorCentralWidget::showEventEditor(XkorEvent * e)
{
	updateCurrentEvent();
	currentEvent = e;
	ee->setData(*e, *rpList());
	editor->setCurrentWidget(ee);
}

void XkorCentralWidget::showRPEditor()
{
	editor->setCurrentWidget(rpe);
}

int XkorCentralWidget::showUnsavedDialog()
{
	QString displayFileName = currentFileName.isEmpty() ? "untitled" : QFileInfo(currentFileName).fileName();
	QMessageBox warning(QMessageBox::NoIcon, tr("xkoranate"), tr("Do you want to save the changes you made to “%1”?").arg(displayFileName), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, this);
	int iconSize = style()->pixelMetric(QStyle::PM_MessageBoxIconSize);
	warning.setIconPixmap(QPixmap(":/icons/xkoranate").scaled(iconSize, iconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	
	
	warning.setInformativeText(tr("Your changes will be lost if you don’t save them."));
	warning.setWindowModality(Qt::WindowModal);
	return warning.exec();
}

void XkorCentralWidget::updateCurrentEvent()
{
	if(currentEvent)
		*currentEvent = ee->data();
}
