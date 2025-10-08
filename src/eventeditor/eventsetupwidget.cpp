#include "eventeditor/eventsetupwidget.h"
#include <QLabel>
#include <QToolBar>
#include <random>
#include <algorithm>

#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <algorithm>
#include "eventeditor/eventsetupdelegate.h"

XkorEventSetupWidget::XkorEventSetupWidget()
{
	treeWidget->setItemDelegate(new XkorEventSetupDelegate(&availableAthleteNames, &availableAthletes));
	treeWidget->setColumnCount(1);
	treeWidget->setHeaderHidden(true);
	treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
	treeWidget->invisibleRootItem()->setFlags(treeWidget->invisibleRootItem()->flags() & ~Qt::ItemIsDropEnabled);
	
	// set up actions
	insertGroupAction = new QAction(QIcon(":/icons/add"), tr("Create a group"), this);
	connect(insertGroupAction, SIGNAL(triggered()), this, SLOT(insertItem()));
	
	insertAthleteAction = new QAction(QIcon(":/icons/add-participant"), tr("Add a participant"), this);
	insertAthleteAction->setEnabled(false);
	connect(insertAthleteAction, SIGNAL(triggered()), this, SLOT(insertAthlete()));
	
	insertAllAction = new QAction(QIcon(":/icons/add-all-participants"), tr("Add all available participants"), this);
	insertAllAction->setEnabled(false);
	connect(insertAllAction, SIGNAL(triggered()), this, SLOT(insertAll()));

	randomizeAction = new QAction(QIcon(":/icons/roll"), tr("Randomize this group"), this);
	randomizeAction->setEnabled(false);
	connect(randomizeAction, SIGNAL(triggered()), this, SLOT(randomizeGroup()));
	
	QList<QAction *> actions;
	actions.append(insertGroupAction);
	actions.append(insertAthleteAction);
	actions.append(insertAllAction);
	actions.append(deleteAction);
	actions.append(randomizeAction);
	setupLayout(actions);
	
	connect(this, SIGNAL(slChanged()), this, SLOT(updateAvailableAthletes()));
}

QTreeWidgetItem * XkorEventSetupWidget::createAthlete(QTreeWidgetItem * parent)
{
	QTreeWidgetItem * item = new QTreeWidgetItem(parent);
	treeWidget->setCurrentItem(item, 0);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	return item;
}

void XkorEventSetupWidget::deleteAthlete(QUuid id)
{
    for(QTreeWidgetItemIterator i(treeWidget); *i; ++i)
    {
        // if this is an athlete, look up its ID
        if((*i)->parent())
        {
            QTreeWidgetItem * parent = (*i)->parent();
            if(QUuid((*i)->data(0, Qt::UserRole).toString()) == id)
            {
                QTreeWidgetItem * temp = parent->takeChild(parent->indexOfChild(*i));
                delete temp;
            }
        }
    }
    emit listChanged();
}

XkorAthlete XkorEventSetupWidget::getAthleteByID(QUuid id) noexcept(false)
{
	XkorAthlete rval;
	try
	{
		rval = sl.getAthleteByID(id);
	}
	catch(XkorSearchFailedException) {};

	if(rval == XkorAthlete())
	{
		std::string err = "No athlete with ID ";
		err += id.toString().toStdString();
		err += " in XkorEventEditor::getAthleteBySN(QString)";
		throw XkorSearchFailedException(err);
	}
	return rval;
}

QList<XkorGroup> XkorEventSetupWidget::groups()
{
	QList<XkorGroup> rval;
	for(int i = 0; i < treeWidget->topLevelItemCount(); ++i)
	{
		XkorGroup group;
		group.name = treeWidget->topLevelItem(i)->text(0);
		for(int j = 0; j < treeWidget->topLevelItem(i)->childCount(); ++j)
			group.athletes.append(QUuid(treeWidget->topLevelItem(i)->child(j)->data(0, Qt::UserRole).toString()));
		rval.push_back(group);
	}
	return rval;
}

void XkorEventSetupWidget::initAthlete(QTreeWidgetItem * athlete, QUuid id)
{
	// get the actual athlete so we can display its name
	XkorAthlete a;
	try
	{
		a = getAthleteByID(id);
		athlete->setText(0, a.name + " (" + a.nation + ")");
	}
	catch(XkorSearchFailedException)
	{
		athlete->setText(0, tr("<unknown participant>"));
	}
	
	athlete->setFlags(athlete->flags() & ~Qt::ItemIsDropEnabled);
	athlete->setData(0, Qt::UserRole, id.toString());
	
}

void XkorEventSetupWidget::initItem(QTreeWidgetItem * group, QString groupName)
{
	group->setFlags((group->flags() | Qt::ItemIsDropEnabled) & ~Qt::ItemIsDragEnabled);
	group->setText(0, groupName);
}

void XkorEventSetupWidget::insertAll()
{
	isInUse = true;
	QList<QTreeWidgetItem *> selection = treeWidget->selectedItems();
	QTreeWidgetItem * parent = (*(selection.begin()))->parent();

	for(int i = 0; i < availableAthletes.size(); ++i)
		initAthlete(createAthlete(parent ? parent : *(selection.begin())), availableAthletes[i]);
	isInUse = false;
	emit listChanged();
}

void XkorEventSetupWidget::insertAthlete()
{
	isInUse = true;
	QList<QTreeWidgetItem *> selection = treeWidget->selectedItems();
	
	treeWidget->setCurrentItem(*(selection.begin()), 0, QItemSelectionModel::Clear);
	QTreeWidgetItem * parent = (*(selection.begin()))->parent();
	
	QTreeWidgetItem * athlete = createAthlete(parent ? parent : *(selection.begin()));
	initAthlete(athlete);
	treeWidget->editItem(athlete, 0);
	isInUse = false;
	emit listChanged();
}

void XkorEventSetupWidget::randomizeGroup()
{
	isInUse = true;
	QList<QTreeWidgetItem *> selection = treeWidget->selectedItems();

	// iterate in reverse so that the first group will be selected when we’re done
	for(QList<QTreeWidgetItem *>::iterator i = selection.end(); i != selection.begin(); )
	{
		--i;
		QTreeWidgetItem * group = *i;
		if(group->parent())
			group = group->parent();

		QList<QTreeWidgetItem *> groupMembers = group->takeChildren();
		std::shuffle(groupMembers.begin(), groupMembers.end(), std::mt19937(std::random_device()()));
		group->addChildren(groupMembers);
		for(QList<QTreeWidgetItem *>::iterator j = groupMembers.begin(); j != groupMembers.end(); ++j)
			treeWidget->setCurrentItem(*j, 0, QItemSelectionModel::Select);
		treeWidget->setCurrentItem(group, 0, QItemSelectionModel::Select);
	}
	isInUse = false;
	emit listChanged();
}

void XkorEventSetupWidget::setGroups(QList<XkorGroup> g)
{
	isInUse = true;
	for(QList<XkorGroup>::iterator i = g.begin(); i != g.end(); ++i)
	{
		QTreeWidgetItem * group = createItem();
		group->setExpanded(true);
		initItem(group, i->name);
		for(QList<QUuid>::iterator j = i->athletes.begin(); j != i->athletes.end(); ++j)
		{
			QTreeWidgetItem * athlete = createAthlete(group);
			initAthlete(athlete, *j);
		}
	}
	isInUse = false;
	emit listChanged();
}

void XkorEventSetupWidget::setSignupList(XkorSignupList l)
{
	sl = l;
	emit slChanged();
}

void XkorEventSetupWidget::setupLayout(QList<QAction *> actions)
{
	// label
	QFont headingFont;
	headingFont.setWeight(QFont::Bold);
	QLabel * label = new QLabel(tr("Set up groups"));
	label->setFont(headingFont);

	// tool bar
	QToolBar * toolBar = new QToolBar;
	toolBar->setIconSize(QSize(this->style()->pixelMetric(QStyle::PM_SmallIconSize), this->style()->pixelMetric(QStyle::PM_SmallIconSize)));
	for(QList<QAction *>::iterator i = actions.begin(); i != actions.end(); ++i)
		if(*i == NULL)
			toolBar->addSeparator();
		else
			toolBar->addAction(*i);

	layout = new QGridLayout(this);
	layout->addWidget(label, 0, 0, Qt::AlignCenter);
	layout->addWidget(treeWidget, 1, 0);
	layout->addWidget(toolBar, 2, 0, Qt::AlignCenter);
	layout->setContentsMargins(0, 0, 0, 0);
}

void XkorEventSetupWidget::updateButtons()
{
	QList<QTreeWidgetItem *> selection = treeWidget->selectedItems();
	
	deleteAction->setEnabled(selection.size() > 0);
	insertAthleteAction->setEnabled(availableAthletes.size() > 0);
	insertAllAction->setEnabled(availableAthletes.size() > 0);
	randomizeAction->setEnabled(selection.size() > 0);
	
	if(selection.size() != 1)
	{
		// can’t manipulate athletes if more than one is selected
		insertAthleteAction->setEnabled(false);
		insertAllAction->setEnabled(false);
	}
}

void XkorEventSetupWidget::updateAvailableAthletes()
{
	availableAthletes.clear();
	availableAthleteNames.clear();

	QList<XkorAthlete> s = sl.athletes();
	for(QList<XkorAthlete>::iterator j = s.begin(); j != s.end(); ++j)
	{
		availableAthletes << j->id;
		availableAthleteNames << (j->name + " (" + j->nation + ")");
	}

	for(QTreeWidgetItemIterator i(treeWidget); *i; ++i)
	{
		// if this is an athlete, look up its ID
		if((*i)->parent())
		{
			try
			{
				XkorAthlete temp = getAthleteByID(QUuid((*i)->data(0, Qt::UserRole).toString()));
				(*i)->setText(0, temp.name + " (" + temp.nation + ")");
			}
			catch(XkorSearchFailedException)
			{
				(*i)->setText(0, tr("<unknown participant>"));
			}
		}
	}
	emit listChanged();
}
