#ifndef APPLICATION_H
#define APPLICATION_H

#include <QAction>
#include <QApplication>
#include <QMainWindow>
#include <QSettings>
#include <QTabWidget>
#include <QToolBar>

#include "centralwidget.h"
#include "mainwindow.h"
#include "tablegenerator/tablegeneratorwindow.h"

class XkorApplication : public QApplication
{
	Q_OBJECT
	
	private:
		XkorMainWindow * mainWindow;
		QAction * newAction, * openAction, * saveAction, * saveAsAction, * tableAction, * settingsAction;
		bool modified;
		XkorCentralWidget * cw;
		QToolBar * toolBar;
		QSettings * settings;
		XkorTableGeneratorWindow * tg;
	private slots:
		void refreshSearchPaths();
		void setDefaultSettings();
		void setEventDirectory(QString dir);
		void setFileName(QString filename);
		void setModified(bool newModified = true);
		void setResultExportDirectory(QString dir);
		void setResultImportDirectory(QString dir);
		void setSignupListDirectory(QString dir);
		void setTableDirectory(QString dir);
		void tableGenerator();
	
	public:
		XkorApplication(int & c, char * * v);
		void loadSports(const QString &sportPath);
	
	private:
		QString m_sportPath;
		bool notify(QObject * rec, QEvent * ev);
};

#endif
