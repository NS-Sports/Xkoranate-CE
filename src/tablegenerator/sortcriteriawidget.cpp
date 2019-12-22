#include "tablegenerator/sortcriteriawidget.h"

#include "tablegenerator/sortcriteriadelegate.h"

XkorSortCriteriaWidget::XkorSortCriteriaWidget()
{
	sortTypes << "awayGoals" << "goalAverage" << "goalDifference" << "goalsAgainst" << "goalsFor" << "h2hAwayGoals" << "h2hGoalDifference" << "h2hGoalsAgainst" << "h2hGoalsFor" << "h2hPoints" << "h2hWins" << "points" << "winPercent" << "winPercentPure" << "winPercentNFL" << "wins";
	sortNames << tr("Away goals") << tr("Goal average") << tr("Goal difference") << tr("Goals against") << tr("Goals for") << tr("Head-to-head away goals") << tr("Head-to-head goal difference") << tr("Head-to-head goals against") << tr("Head-to-head goals for") << tr("Head-to-head points") << tr("Head-to-head wins") << tr("Points") << tr("Win percentage (draws count 50%)") << tr("Win percentage (draws count 0%)") << tr("Win percentage (draws ignored)") << tr("Wins");
	
	treeWidget->setColumnCount(1);
	treeWidget->setItemDelegate(new XkorSortCriteriaDelegate(sortTypes, sortNames));
	treeWidget->setHeaderHidden(true);
	treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
	
	QList<QAction *> actions;
	actions.append(insertAction);
	actions.append(deleteAction);
	setupLayout(actions, true); // vertical bar
}

std::vector<QString> XkorSortCriteriaWidget::defaultSortCriteria()
{
	std::vector<QString> rval;
	rval.push_back("points");
	rval.push_back("goalDifference");
	rval.push_back("goalsFor");
	rval.push_back("h2hPoints");
	rval.push_back("h2hGoalDifference");
	rval.push_back("h2hGoalsFor");
	return rval;
}

void XkorSortCriteriaWidget::initItem(QTreeWidgetItem * item, QString sortType)
{
	item->setData(0, 1093, sortType);
	
	// get the name of the sort type
	int n = sortTypes.indexOf(sortType);
	if(n >= 0 && n < sortNames.size())
		item->setText(0, sortNames[n]);
	else
		item->setText(0, tr("<unknown sort type %1>").arg(sortType));
}

std::vector<QString> XkorSortCriteriaWidget::sortCriteria() const
{
	std::vector<QString> rval;
	for(QTreeWidgetItemIterator i(treeWidget); *i; ++i)
		rval.push_back((*i)->data(0, 1093).toString());
	return rval;
}

void XkorSortCriteriaWidget::setSortCriteria(std::vector<QString> sc)
{
	isInUse = true;
	treeWidget->clear();
	for(std::vector<QString>::iterator i = sc.begin(); i != sc.end(); ++i)
		initItem(createItem(), *i);
	isInUse = false;
	emit listChanged();
}
