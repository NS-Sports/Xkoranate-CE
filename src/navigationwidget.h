#ifndef NAVIGATIONWIDGET_H
#define NAVIGATIONWIDGET_H

#include <QtGui>
#include <tr1/random>

#include "abstracttreewidget.h"
#include "event.h"
#include "rplist.h"
#include "signuplist.h"

class XkorNavigationWidget : public XkorAbstractTreeWidget
{
	Q_OBJECT

	public:
		XkorNavigationWidget();
		QList<QPair<QUuid, XkorEvent *> > events();
		void reset();
		XkorRPList * rpList() { return m_rpList; }
		void setEvents(QList<QPair<QUuid, XkorEvent *> > events);
		void setRPList(XkorRPList * rpList) { m_rpList = rpList; treeWidget->setCurrentItem(m_rpListItem, 0); emit editRPList(m_rpList); }
	signals:
		void editEvent(XkorEvent * event);
		void editRPList(XkorRPList * rpList);

	protected:
		QTreeWidgetItem * m_rpListItem, * m_eventsItem;
		QAction * m_insertSheetAction;

		std::tr1::mt19937 r;

		// store the actual data
		QHash<QUuid, XkorEvent *> m_events;
		XkorRPList * m_rpList;

		void createCategories();
		QTreeWidgetItem * createItem(QTreeWidgetItem * parent);
		void initEvent(QTreeWidgetItem * item);
	protected slots:
		void correctSelectionAfterDelete();
		void editItem();
		void insertItem();
		void updateButtons();
		void updateEventNames();
};

#endif // NAVIGATIONWIDGET_H
