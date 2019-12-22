#include "sportmodel.h"

QModelIndex XkorSportModel::findSport(QString discipline, QString event, QString scorinator)
{
	// find the discipline
	QStandardItem * disciplineItem = 0;
	QStandardItem * root = invisibleRootItem();
	for(int i = 0; i < root->rowCount(); ++i)
		if(root->child(i) && root->child(i)->text() == discipline)
			disciplineItem = root->child(i);
	if(disciplineItem)
	{
		// find the event
		QStandardItem * eventItem = 0;
		for(int i = 0; i < disciplineItem->rowCount(); ++i)
			if(disciplineItem->child(i) && disciplineItem->child(i)->text() == event)
				eventItem = disciplineItem->child(i);
		if(eventItem)
			// find the scorinator
			for(int i = 0; i < eventItem->rowCount(); ++i)
				if(eventItem->child(i) && eventItem->child(i)->text() == scorinator)
					return eventItem->child(i)->index();
	}
	return QModelIndex();
}

QModelIndex XkorSportModel::findSport(QString fullName)
{
	QStandardItem * root = invisibleRootItem();
	for(int i = 0; i < root->rowCount(); ++i)
	{
		QStandardItem * child = root->child(i);
		if(child && child->data(SPORT_NAME) == fullName)
			return child->index();
		for(int j = 0; j < child->rowCount(); ++j)
		{
			QStandardItem * grandchild = child->child(j);
			if(grandchild && grandchild->data(SPORT_NAME) == fullName)
				return grandchild->index();
			for(int k = 0; k < grandchild->rowCount(); ++k)
			{
				QStandardItem * greatGrandchild = grandchild->child(k);
				if(greatGrandchild && greatGrandchild->data(SPORT_NAME) == fullName)
					return greatGrandchild->index();
			}
		}
	}
	return QModelIndex();
}

void XkorSportModel::initItem(QStandardItem * item, XkorSport sport)
{
	item->setData(sport.name(), SPORT_NAME);
	item->setData(sport.paradigm(), SPORT_PARADIGM);
	QVariant sportVariant;
	sportVariant.setValue(sport);
	item->setData(sportVariant, SPORT_DATA);
}

void XkorSportModel::insertSport(XkorSport s)
{
	// find or create the discipline item
	QStandardItem * disciplineItem = 0;
	QStandardItem * root = invisibleRootItem();
	for(int i = 0; i < root->rowCount(); ++i)
		if(root->child(i) && root->child(i)->text() == s.discipline())
			disciplineItem = root->child(i);
	if(!disciplineItem)
	{
		disciplineItem = new QStandardItem(s.discipline());
		disciplineItem->setEditable(false);
		if(s.event() == QString())
			initItem(disciplineItem, s);
		root->appendRow(disciplineItem);
	}
	
	// find or create the event item
	if(s.event() != QString())
	{
		QStandardItem * eventItem = 0;
		for(int i = 0; i < disciplineItem->rowCount(); ++i)
			if(disciplineItem->child(i) && disciplineItem->child(i)->text() == s.event())
				eventItem = disciplineItem->child(i);
		if(!eventItem)
		{
			eventItem = new QStandardItem(s.event());
			eventItem->setEditable(false);
			if(s.scorinator() == QString())
				initItem(eventItem, s);
			disciplineItem->appendRow(eventItem);
		}
	
		// create the scorinator item
		if(s.scorinator() != QString())
		{
			QStandardItem * scorinatorItem = new QStandardItem(s.scorinator());
			scorinatorItem->setEditable(false);
			initItem(scorinatorItem, s);
			eventItem->appendRow(scorinatorItem);
		}
	}
}
