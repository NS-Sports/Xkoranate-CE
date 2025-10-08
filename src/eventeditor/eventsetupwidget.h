#ifndef EVENTSETUPWIDGET_H
#define EVENTSETUPWIDGET_H

#include <QAction>
#include <QToolButton>
#include <QStringList>
#include <vector>

#include "abstracttreewidget.h"
#include "athlete.h"
#include "group.h"
#include "signuplist.h"

#include <QAction>
#include <QTreeWidgetItem>

class XkorEventSetupWidget : public XkorAbstractTreeWidget
{
	Q_OBJECT
	
	protected:
		QAction * insertGroupAction, * insertAthleteAction, * insertAllAction, * randomizeAction;
		QToolButton * insertGroupButton, * insertAthleteButton, * insertAllButton;
		QStringList availableAthleteNames;
		QList<QUuid> availableAthletes;
		XkorSignupList sl;
		
		XkorAthlete getAthleteByID(QUuid id) noexcept(false);
		
		virtual QTreeWidgetItem * createAthlete(QTreeWidgetItem * parent);
		virtual void initAthlete(QTreeWidgetItem * athlete) { initAthlete(athlete, QUuid()); }
		virtual void initAthlete(QTreeWidgetItem * athlete, QUuid id);
		virtual void initItem(QTreeWidgetItem * group) { initItem(group, QString()); } // initItem is used for groups
		virtual void initItem(QTreeWidgetItem * group, QString groupName);
		virtual void setupLayout(QList<QAction *> actions);
		
	protected slots:
		void insertAll();
		void insertAthlete();
		void randomizeGroup();
		virtual void updateButtons();
	public:
		XkorEventSetupWidget();
		QList<XkorGroup> groups();
		void setGroups(QList<XkorGroup> g);
		void setSignupList(XkorSignupList l);
	signals:
		void slChanged();
	public slots:
        void deleteAthlete(QUuid id);
		void updateAvailableAthletes();
};

#endif
