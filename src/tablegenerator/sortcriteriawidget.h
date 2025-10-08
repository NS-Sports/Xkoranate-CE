#ifndef SORTCRITERIAWIDGET_H
#define SORTCRITERIAWIDGET_H

#include "abstracttreewidget.h"

#include <QTreeWidgetItem>

class XkorSortCriteriaWidget : public XkorAbstractTreeWidget
{
	Q_OBJECT
	
	protected:
		QStringList sortTypes, sortNames;
		
		virtual void initItem(QTreeWidgetItem * item) { initItem(item, QString()); }
		virtual void initItem(QTreeWidgetItem * item, QString sortType);
		virtual QString insertionText() { return tr("Add sort criterion"); }
		virtual QString deletionText() { return tr("Remove sort criteria"); }
		virtual QString movingUpText() { return tr("Move criterion up"); }
		virtual QString movingDownText() { return tr("Move criterion down"); }
	
	public:
		XkorSortCriteriaWidget();
		static std::vector<QString> defaultSortCriteria();
		std::vector<QString> sortCriteria() const;
		void setSortCriteria(std::vector<QString> sc);
};

#endif
