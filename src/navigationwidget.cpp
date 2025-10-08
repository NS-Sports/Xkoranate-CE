#include "navigationwidget.h"

#include <ctime>

XkorNavigationWidget::XkorNavigationWidget()
{
	r.seed(time(0));

	m_rpList = new XkorRPList;

	m_rpListItem = m_eventsItem = 0;

	QPalette palette = this->palette();
	palette.setColor(QPalette::Base, palette.alternateBase().color());
	setPalette(palette);
	setBackgroundRole(QPalette::Base);
	setAutoFillBackground(true);
	treeWidget->setPalette(palette);
	treeWidget->setFrameStyle(QFrame::NoFrame);
	treeWidget->setAttribute(Qt::WA_MacShowFocusRect, false);
	treeWidget->setAutoFillBackground(true);

	treeWidget->setColumnCount(1);
	treeWidget->setHeaderHidden(true);
	treeWidget->setAlternatingRowColors(false);
	treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
	treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	m_insertSheetAction = new QAction(QIcon(":/icons/add"), tr("New event"), this);
	m_insertSheetAction->setEnabled(true);
	connect(m_insertSheetAction, SIGNAL(triggered()), this, SLOT(insertItem()));

	deleteAction->setToolTip(tr("Remove event"));
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(correctSelectionAfterDelete()));

	QList<QAction *> actions;
	actions.append(m_insertSheetAction);
	actions.append(deleteAction);
	setupLayout(actions);

	createCategories();

	connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(editItem()));
}

void XkorNavigationWidget::correctSelectionAfterDelete()
{
	if(treeWidget->currentItem() == m_eventsItem)
		treeWidget->setCurrentItem(m_rpListItem, 0);
	editItem();
}

void XkorNavigationWidget::createCategories()
{
	m_rpListItem = new QTreeWidgetItem(treeWidget, QStringList(tr("BONUSES")));
	treeWidget->addTopLevelItem(m_rpListItem);
	m_rpListItem->setSelected(true);
	m_rpListItem->setFlags(m_rpListItem->flags() & ~Qt::ItemIsDragEnabled & ~Qt::ItemIsDropEnabled);

	m_eventsItem = new QTreeWidgetItem(treeWidget, QStringList(tr("EVENTS")));
	treeWidget->addTopLevelItem(m_eventsItem);
	m_eventsItem->setFlags(m_eventsItem->flags() & ~Qt::ItemIsDragEnabled & ~Qt::ItemIsEditable);

	treeWidget->invisibleRootItem()->setFlags(treeWidget->invisibleRootItem()->flags() & ~Qt::ItemIsDropEnabled);

	QFont font;
	font.setBold(true);
	font.setPointSize(font.pointSize() * 0.95);
	m_rpListItem->setFont(0, font);
	m_eventsItem->setFont(0, font);

	
m_rpList = new XkorRPList;
	emit editRPList(m_rpList);
}

QTreeWidgetItem * XkorNavigationWidget::createItem(QTreeWidgetItem * parent)
{
	QTreeWidgetItem * item = new QTreeWidgetItem(parent);
	treeWidget->setCurrentItem(item, 0);
	item->setFlags((item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled) & ~Qt::ItemIsDropEnabled);
	return item;
}

void XkorNavigationWidget::editItem()
{
	if(isInUse)
		return;

	QList<QTreeWidgetItem *> selection = treeWidget->selectedItems();

	if(selection.size() == 1)
	{
		QTreeWidgetItem * item = *(selection.begin());
		if(item == m_rpListItem)
			emit editRPList(m_rpList);
		else if(m_events[QUuid(item->data(0, Qt::UserRole).toString())])
			emit editEvent(m_events[QUuid(item->data(0, Qt::UserRole).toString())]); // look up the event with the unique ID
	}
}

QList<QPair<QUuid, XkorEvent *> > XkorNavigationWidget::events()
{
	updateEventNames();
	QList<QPair<QUuid, XkorEvent *> > rval;
	for(int i = 0; i < m_eventsItem->childCount(); ++i)
	{
		QUuid uniqueID = QUuid(m_eventsItem->child(i)->data(0, Qt::UserRole).toString());
		rval.append(qMakePair(uniqueID, m_events[uniqueID]));
	}
	return rval;
}

void XkorNavigationWidget::initEvent(QTreeWidgetItem * item)
{
	isInUse = true;

	QUuid uniqueID = QUuid(r(), r(), r(), r(), r(), r(), r(), r(), r(), r(), r());

	// create the actual event
	XkorEvent * event = new XkorEvent;
	m_events.insert(uniqueID, event);

	item->setData(0, Qt::UserRole, uniqueID.toString());
	isInUse = false;

	emit editEvent(event);
}

void XkorNavigationWidget::insertItem()
{
	isInUse = true;
	QList<QTreeWidgetItem *> selection = treeWidget->selectedItems();

	if(selection.size() > 0) // this operation is dangerous if there is no selection
		treeWidget->setCurrentItem(*(selection.begin()), 0, QItemSelectionModel::Clear);
	QTreeWidgetItem * item = createItem(m_eventsItem);

	initEvent(item);

	initItem(item);
	treeWidget->editItem(item, 0);
	isInUse = false;
	emit listChanged();
}

void XkorNavigationWidget::reset()
{
	treeWidget->clear();

	delete m_rpList;
	m_rpList = new XkorRPList;

	for(QHash<QUuid, XkorEvent *>::iterator i = m_events.begin(); i != m_events.end(); ++i)
		delete i.value();
	m_events.clear();

	createCategories();
}

void XkorNavigationWidget::setEvents(QList<QPair<QUuid, XkorEvent *> > events)
{
	for(QList<QPair<QUuid, XkorEvent *> >::iterator i = events.begin(); i != events.end(); ++i)
	{
		m_events.insert(i->first, i->second);
		QTreeWidgetItem * item = createItem(m_eventsItem);
		item->setText(0, i->second->name());
		item->setData(0, Qt::UserRole, i->first.toString());
	}
	treeWidget->setCurrentItem(m_rpListItem, 0);
}

void XkorNavigationWidget::updateButtons()
{
	QList<QTreeWidgetItem *> selection = treeWidget->selectedItems();

	deleteAction->setEnabled(selection.size() > 0);

	if(m_eventsItem && selection.size() == 1)
	{
		// don’t delete or move the top-level items
		if((*(selection.begin()))->parent() != m_eventsItem)
		{
			deleteAction->setEnabled(false);
			moveUpAction->setEnabled(false);
			moveDownAction->setEnabled(false);
		}
		else
		{
			// can only move up/down if we’re not at the top/bottom
			QTreeWidgetItem * item = *(selection.begin());

			bool canMoveUp = true, canMoveDown = true;
			int index = m_eventsItem->indexOfChild(item);
			if(index == 0) // top
				canMoveUp = false;
			if(index == m_eventsItem->childCount() - 1) // bottom
				canMoveDown = false;

			moveUpAction->setEnabled(canMoveUp);
			moveDownAction->setEnabled(canMoveDown);
		}
	}
	else
	{
		// can’t manipulate items if more than one is selected
		moveUpAction->setEnabled(false);
		moveDownAction->setEnabled(false);
	}
}

void XkorNavigationWidget::updateEventNames()
{
	for(int i = 0; i < m_eventsItem->childCount(); ++i)
	{
		QUuid uniqueID = QUuid(m_eventsItem->child(i)->data(0, Qt::UserRole).toString());
		m_events[uniqueID]->setName(m_eventsItem->child(i)->text(0));
	}
}
