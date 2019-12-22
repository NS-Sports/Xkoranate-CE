#ifndef TABLEGENERATORWINDOW_H
#define TABLEGENERATORWINDOW_H

#include <QMainWindow>
#include "tablegenerator/tablegenerator.h"

class XkorTableGeneratorWindow : public QMainWindow
{
	Q_OBJECT
	public:
		XkorTableGeneratorWindow(QWidget * parent = 0);

	signals:
		void resultImportDirectoryChanged(QString dir);
		void tableDirectoryChanged(QString dir);

	protected:
		QAction * newAction, * openAction, * saveAction, * saveAsAction, * importAction, * generateAction;
		QToolBar * toolBar;
		XkorTableGenerator * generator;

		void closeEvent(QCloseEvent * event);

	protected slots:
		void setFileName(QString filename);
		void setModified(bool newModified = true);
		void setResultImportDirectory(QString dir);
		void setTableDirectory(QString dir);
};

#endif // TABLEGENERATORWINDOW_H
