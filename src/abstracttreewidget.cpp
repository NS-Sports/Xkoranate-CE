#include "abstracttreewidget.h"
#include <QToolBar>

XkorAbstractTreeWidget::XkorAbstractTreeWidget()
{	
	isInUse = true;
	treeWidget = new XkorTreeWidget;
	treeWidget->setRootIsDecorated(false);
	treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	treeWidget->setAlternatingRowColors(true);
	treeWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons())); // disable Delete if there is no selection, et cetera
	connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(listIsEdited())); // if we change the list, we need to tell the table generator
	
	insertAction = new QAction(QIcon(":/icons/add"), insertionText(), this);
	insertAction->setEnabled(true);
	connect(insertAction, SIGNAL(triggered()), this, SLOT(insertItem()));
	
	deleteAction = new QAction(QIcon(":/icons/remove"), deletionText(), this);
	deleteAction->setEnabled(false);
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItems()));
	
	moveUpAction = new QAction(movingUpText(), this);
	moveUpAction->setEnabled(false);
	connect(moveUpAction, SIGNAL(triggered()), this, SLOT(moveUp()));
	
	moveDownAction = new QAction(movingDownText(), this);
	moveDownAction->setEnabled(false);
	connect(moveDownAction, SIGNAL(triggered()), this, SLOT(moveDown()));
	
	connect(this, SIGNAL(listChanged()), this, SLOT(updateButtons()));
	
	isInUse = false;
}

QTreeWidgetItem * XkorAbstractTreeWidget::createItem()
{
	QTreeWidgetItem * item = new QTreeWidgetItem(treeWidget);
	treeWidget->setCurrentItem(item, 0);
	item->setFlags((item->flags() | Qt::ItemIsEditable) & ~Qt::ItemIsDropEnabled);
	return item;
}

void XkorAbstractTreeWidget::deleteItems()
{
	isInUse = true;
	QList<QTreeWidgetItem *> selection = treeWidget->selectedItems();
	for(QList<QTreeWidgetItem *>::iterator i = selection.begin(); i != selection.end(); ++i)
	{
		QTreeWidgetItem * parent = (*i)->parent();
		if(parent)
		{
			QTreeWidgetItem * temp = parent->takeChild(parent->indexOfChild(*i));
			delete temp;
		}
		else
		{
			QTreeWidgetItem * temp = treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(*i));
			delete temp;
		}
	}
	isInUse = false;
	emit listChanged();
}

void XkorAbstractTreeWidget::insertItem()
{
	isInUse = true;
	QTreeWidgetItem * item = createItem();
	initItem(item);
	treeWidget->editItem(item, 0);
	isInUse = false;
	emit listChanged();
}


void XkorAbstractTreeWidget::keyPressEvent(QKeyEvent * event)
{
	if(event->key() == Qt::Key_Up && event->modifiers() & Qt::ControlModifier)
		moveUpAction->trigger();
	else if(event->key() == Qt::Key_Down && event->modifiers() & Qt::ControlModifier)
		moveDownAction->trigger();
}

void XkorAbstractTreeWidget::listIsEdited()
{
	if(!isInUse)
		emit listChanged();
}

void XkorAbstractTreeWidget::moveDown()
{
	isInUse = true;
	QList<QTreeWidgetItem *> selection = treeWidget->selectedItems();
	QTreeWidgetItem * item = *(selection.begin());
	QTreeWidgetItem * parent = item->parent();
	if(parent && parent->indexOfChild(item) < parent->childCount() - 1)
	{
		int index = parent->indexOfChild(item);
		parent->takeChild(index);
		parent->insertChild(index + 1, item);
		treeWidget->setCurrentItem(item, 0);
	}
	else if(treeWidget->indexOfTopLevelItem(item) < treeWidget->topLevelItemCount() - 1)
	{
		int index = treeWidget->indexOfTopLevelItem(item);
		treeWidget->takeTopLevelItem(index);
		treeWidget->insertTopLevelItem(index + 1, item);
		treeWidget->setCurrentItem(item, 0);
	}
	treeWidget->setExpanded(treeWidget->currentIndex(), true);
	isInUse = false;
	emit listChanged();
}

void XkorAbstractTreeWidget::moveUp()
{
	isInUse = true;
	QList<QTreeWidgetItem *> selection = treeWidget->selectedItems();
	QTreeWidgetItem * item = *(selection.begin());
	QTreeWidgetItem * parent = item->parent();
	if(parent && parent->indexOfChild(item) > 0)
	{
		int index = parent->indexOfChild(item);
		parent->takeChild(index);
		parent->insertChild(index - 1, item);
		treeWidget->setCurrentItem(item, 0);

	}
	else if(treeWidget->indexOfTopLevelItem(item) > 0)
	{
		int index = treeWidget->indexOfTopLevelItem(item);
		treeWidget->takeTopLevelItem(index);
		treeWidget->insertTopLevelItem(index - 1, item);
		treeWidget->setCurrentItem(item, 0);
	}
	treeWidget->setExpanded(treeWidget->currentIndex(), true);
	isInUse = false;
	emit listChanged();
}

void XkorAbstractTreeWidget::setupLayout(QList<QAction *> actions, bool isVertical)
{
	// tool bar
	QToolBar * toolBar = new QToolBar;
	toolBar->setIconSize(QSize(this->style()->pixelMetric(QStyle::PM_SmallIconSize), this->style()->pixelMetric(QStyle::PM_SmallIconSize)));
	if(isVertical)
		toolBar->setOrientation(Qt::Vertical);
	for(QList<QAction *>::iterator i = actions.begin(); i != actions.end(); ++i)
		if(*i == NULL)
			toolBar->addSeparator();
		else
			toolBar->addAction(*i);

	layout = new QGridLayout(this);
	layout->addWidget(treeWidget, 0, 0);
	if(isVertical)
		layout->addWidget(toolBar, 0, 1, Qt::AlignCenter);
	else
		layout->addWidget(toolBar, 1, 0, Qt::AlignCenter);
	layout->setContentsMargins(0, 0, 0, 0);
}

void XkorAbstractTreeWidget::updateButtons()
{
	QList<QTreeWidgetItem *> selection = treeWidget->selectedItems();
	deleteAction->setEnabled(selection.size() > 0);
	
	if(selection.size() == 1)
	{		
		// can only move up/down if we’re not at the top/bottom
		QTreeWidgetItem * item = *(selection.begin());
		QTreeWidgetItem * parent = item->parent();
		
		bool canMoveUp = true, canMoveDown = true;
		if(parent)
		{
			int index = parent->indexOfChild(item);
			if(index == 0) // top
				canMoveUp = false;
			if(index == parent->childCount() - 1) // bottom
				canMoveDown = false;
		}
		else
		{
			int index = treeWidget->indexOfTopLevelItem(item);
			if(index == 0) // top
				canMoveUp = false;
			if(index == treeWidget->topLevelItemCount() - 1) // bottom
				canMoveDown = false;
		}
		moveUpAction->setEnabled(canMoveUp);
		moveDownAction->setEnabled(canMoveDown);
	}
	else
	{
		// can’t manipulate athletes if more than one is selected
		moveUpAction->setEnabled(false);
		moveDownAction->setEnabled(false);
	}
}
