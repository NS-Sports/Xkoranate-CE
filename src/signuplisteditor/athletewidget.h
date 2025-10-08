#ifndef ATHLETEWIDGET_H
#define ATHLETEWIDGET_H

#include <QtGui>
#include <random>

#include "abstracttreewidget.h"
#include "athlete.h"
#include "signuplisteditor/athletedelegate.h"
#include "signuplisteditor/abstractathletewidget.h"

#include <QFileDialog>

#include <QAction>
#include <QFileDialog>
#include <QTreeWidgetItem>

class XkorAthleteWidget : public XkorAbstractAthleteWidget
{
	Q_OBJECT
	
	protected:
		XkorAthleteDelegate * delegate;
		QAction * importAction;
		QFileDialog * dialog;

		virtual void init();
		virtual void initItem(QTreeWidgetItem * item, QString athleteName, QUuid id, QString nation, double skill) { XkorAbstractAthleteWidget::initItem(item, athleteName, id, nation, skill); }
		virtual void initItem(QTreeWidgetItem * item, QString athleteName, QUuid id, QString nation, double skill, QHash<QString, QVariant> properties);
		virtual QString insertionText() { return tr("Add athlete"); }
		virtual QString deletionText() { return tr("Remove athletes"); }
		
	protected slots:
		void importAthletes();
		void importAthletes(QString filename);
	
	public:
		XkorAthleteWidget(QStringList columnKeys, QStringList columnNames, QStringList columnTypes, double minDouble = 0, double maxDouble = 0, double doubleStep = 1);
		~XkorAthleteWidget();

		QList<XkorAthlete> athletes();
		void setAthletes(QList<XkorAthlete> athletes);
		void setMaxRank(double newMax);
		void setMinRank(double newMin);
};

#endif
