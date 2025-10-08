#ifndef ABSTRACTATHLETEWIDGET_H
#define ABSTRACTATHLETEWIDGET_H

#include <QtGui>
#include <random>

#include "abstracttreewidget.h"
#include "athlete.h"
#include "signuplisteditor/athletedelegate.h"

#include <QTreeWidgetItem>

class XkorAbstractAthleteWidget : public XkorAbstractTreeWidget
{
	Q_OBJECT

	protected:
		QStringList m_columnKeys, m_columnNames, m_columnTypes;
		double m_minDouble, m_maxDouble, m_doubleStep;

        std::mt19937 r;

		virtual void initItem(QTreeWidgetItem * item) { initItem(item, QString(), QUuid(r(), r(), r(), r(), r(), r(), r(), r(), r(), r(), r()), QString(), 0); }
		virtual void initItem(QTreeWidgetItem * item, QString athleteName, QUuid id, QString nation, double skill) { initItem(item, athleteName, id, nation, skill, QHash<QString, QVariant>()); }
		virtual void initItem(QTreeWidgetItem * item, QString athleteName, QUuid id, QString nation, double skill, QHash<QString, QVariant> properties) = 0;

    protected slots:
        void deleteItems()
        {
            QList<QTreeWidgetItem *> selection = treeWidget->selectedItems();
            for(QList<QTreeWidgetItem *>::iterator i = selection.begin(); i != selection.end(); ++i)
                emit itemDeleted(QUuid((*i)->data(m_columnKeys.indexOf("name"), Qt::UserRole).toString()));
            XkorAbstractTreeWidget::deleteItems();
        }

	public:
		XkorAbstractAthleteWidget(QStringList columnKeys, QStringList columnNames, QStringList columnTypes, double minDouble, double maxDouble, double doubleStep)
		{
			m_columnKeys = columnKeys;
			m_columnNames = columnNames;
			m_columnTypes = columnTypes;
			m_minDouble = minDouble;
			m_maxDouble = maxDouble;
			m_doubleStep = doubleStep;
		}

		virtual QList<XkorAthlete> athletes() = 0;
		virtual void setAthletes(QList<XkorAthlete> athletes) = 0;
		virtual void setMaxRank(double newMax) = 0;
		virtual void setMinRank(double newMin) = 0;

	signals:
        void itemDeleted(QUuid id);
		void signupListDirectoryChanged(QString dir);
};

#endif // ABSTRACTATHLETEWIDGET_H
