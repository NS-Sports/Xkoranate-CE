#include "rpeditor/rpbonuswidgets/olympicrpbonuswidget.h"

#include <QHeaderView>
#include "rpeditor/rpbonuswidgets/olympicrpbonusdelegate.h"

XkorOlympicRPBonusWidget::XkorOlympicRPBonusWidget()
{
	treeWidget->setColumnCount(2);
	treeWidget->setSortingEnabled(true);
	treeWidget->setItemDelegate(new XkorOlympicRPBonusDelegate);
	treeWidget->sortItems(0, Qt::AscendingOrder);
	setUseTeamBonus();
	
	// set the column widths
	treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	treeWidget->header()->setSectionResizeMode(1, QHeaderView::Fixed);
	treeWidget->header()->resizeSection(1, 100);
	
	QList<QAction *> actions;
	actions.append(insertAction);
	actions.append(deleteAction);
	setupLayout(actions);
}

QHash<QString, QHash<QString, double> > XkorOlympicRPBonusWidget::bonuses()
{
	QHash<QString, QHash<QString, double> > rval;
	for(QTreeWidgetItemIterator i(treeWidget); *i; ++i)
	{
		QHash<QString, double> properties;
		properties.insert("bonus", (*i)->text(1).toDouble());
		rval[(*i)->text(0)] = properties;
	}
	return rval;
}

void XkorOlympicRPBonusWidget::initItem(QTreeWidgetItem * item, QString nation, double bonus)
{
	item->setText(0, nation);
	item->setText(1, QString::number(bonus));
}

void XkorOlympicRPBonusWidget::setBonuses(QHash<QString, QHash<QString, double> > bonuses)
{
	clear();
	for(QHash<QString, QHash<QString, double> >::iterator i = bonuses.begin(); i != bonuses.end(); ++i)
		initItem(createItem(), i.key(), i.value()["bonus"]);
	emit listChanged();
}
