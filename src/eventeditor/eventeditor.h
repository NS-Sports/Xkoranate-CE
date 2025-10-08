#ifndef EVENTEDITOR_H
#define EVENTEDITOR_H

#include <QtGui>

#include "abstractoptionswidget.h"
#include "event.h"
#include "rplist.h"
#include "signuplist.h"
#include "eventeditor/eventsetupwidget.h"
#include "eventeditor/scorinatewidget.h"
#include "eventeditor/sportselector.h"
#include "paradigms/abstractparadigm.h"
#include "signuplisteditor/signuplisteditor.h"
#include "eventeditor/competitionselector.h"

#include <QStackedLayout>

class XkorEventEditor : public QWidget
{
	Q_OBJECT
	
	public:
		XkorEventEditor(QWidget * parent = 0);
        void loadSports(const QString &sportPath);
	public slots:
        XkorEvent data() { updateData(); return m_data; }
		void setData(XkorEvent data, XkorRPList rpList);
		void updateData();

	signals:
		void dataChanged();
		void resultExportDirectoryChanged(QString dir);
		void signupListDirectoryChanged(QString dir);
	
	private:
		XkorEvent m_data;
		XkorRPList m_rpList;
		XkorSport sport;
		
		// event settings widgets
		XkorSportSelector * sportSelector;
		XkorCompetitionSelector * competitionSelector;
		
		// event data widgets
		XkorSignupListEditor * signupListEditor;
		XkorEventSetupWidget * eventSetupWidget;
		
		// scorination widgets
		XkorScorinateWidget * scorinateWidget;

		// buttons
		QPushButton * prev, * next;
		
		// layout widgets
		QStackedLayout * stack;
		QGridLayout * layout;

		QItemSelectionModel * selectionModel;

		bool isLoading;
		
		void initCompetitionSelector();
		void initEventSetupWidget();
		void initLayout();
		void initScorinateWidget();
		void initSignupListEditor();
		void initSportSelector();
	private slots:
		void goPrev();
		void goNext();
		void setDataChanged() { if(!isLoading) emit dataChanged(); }
		void setResult(int matchday, QString result);
		void setResumeFileOptions(QHash<QString, QVariant> options);
		void setResultExportDirectory(QString dir);
		void setSignupListDirectory(QString dir);
		void updateCompetition(QString competition);
		void updateCompetitionOptions(QHash<QString, QVariant> options);
		void updateParadigmOptions(QHash<QString, QVariant> options);
		void updateSignupList();
		void updateSport(XkorSport s);
};

#endif
