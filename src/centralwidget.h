#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QLineEdit>
#include <QStackedWidget>
#include <QTreeWidget>

#include "eventeditor/eventeditor.h"
#include "rpeditor/rpeditor.h"
#include "navigationwidget.h"

#include <QLineEdit>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QWidget>
#include <QFileDialog>

class XkorCentralWidget : public QWidget
{
	Q_OBJECT

	public:
		XkorCentralWidget(QWidget * parent = 0);
        void loadSports(const QString &sportPath);
	signals:
		void eventDirectoryChanged(QString dir);
		void fileChanged(QString filename);
		void fileEdited();
		void resultExportDirectoryChanged(QString dir);
		void signupListDirectoryChanged(QString dir);
	public slots:
		void newFile();
		void openFile();
		void openFile(QString filename);
		void saveFile(QString filename = "");
		void saveFileAs();
	protected:
		QString currentFileName;
		bool modified;

		XkorNavigationWidget * nw;
		QStackedWidget * editor;
		QFileDialog * dialog;

		// editor widgets
		XkorEventEditor * ee;
		XkorRpEditor * rpe;

		XkorEvent * currentEvent;

		void closeEvent(QCloseEvent * event);
		QList<QPair<QUuid, XkorEvent *> > events();
		bool okayToLoad();
		XkorRPList * rpList();
		int showUnsavedDialog();
	protected slots:
		void setModified(bool isModified = true);
		void setResultExportDirectory(QString dir);
		void setSignupListDirectory(QString dir);
		void showEventEditor(XkorEvent * e);
		void showRPEditor();
		void updateCurrentEvent();
};

#endif // CENTRALWIDGET_H
