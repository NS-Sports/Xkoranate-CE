#ifndef SCORINATEWIDGET_H
#define SCORINATEWIDGET_H

#include <QtGui>
#include <random>

#include "competitions/competitionfactory.h"
#include "event.h"

#include <QComboBox>
#include <QPlainTextEdit>

#include <QWidget>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QAction>
#include <QFileDialog>

class XkorScorinateWidget : public QWidget
{
	Q_OBJECT
	
	private:
		QVBoxLayout * layout;
		QComboBox * matchday;
		QPlainTextEdit * textedit;
		int lastMatchday;
        std::mt19937 r;
		XkorAbstractCompetition * c;

		QAction * scorinateAction, * exportResultsAction;
		QFileDialog * dialog;

		XkorEvent e;
		XkorRPList rp;
		XkorSport s;
		XkorStartList sl;

	private slots:
		void exportResults();
		void exportResults(QString filename);
		void updateButtons();
	
	public:
		XkorScorinateWidget(QWidget * parent = 0);
		~XkorScorinateWidget();
	public slots:
		void clear();
		void scorinate();
		void scorinateMatchday(int md);
		void setEvent(XkorEvent event, XkorRPList rpList, XkorSport sport);
		void updateCompetition(QHash<QString, QVariant> resumeFileOptions, int matchday, QString result);
		void updateResults(int matchday);

	signals:
		void resultConfirmed(int matchday, QString results);
		void resultExportDirectoryChanged(QString dir);
		void resumeFileOptionsSet(QHash<QString, QVariant> resumeFileOptions);
};

#endif
