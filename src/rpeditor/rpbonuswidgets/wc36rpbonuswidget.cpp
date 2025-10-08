#include "rpeditor/rpbonuswidgets/wc36rpbonuswidget.h"
#include <QToolBar>

#include <QHeaderView>
#include <QLabel>
#include "rpeditor/rpbonuswidgets/wc36rpbonusdelegate.h"

XkorWC36RPBonusWidget::XkorWC36RPBonusWidget()
{
	// matchday box
	matchday = new QSpinBox;
	connect(matchday, SIGNAL(valueChanged(int)), this, SLOT(setListChanged()));
	
	matchdayForm = new QFormLayout;
	matchdayForm->addRow(tr("Current matchday:"), matchday);
	
	treeWidget->setColumnCount(3);
	treeWidget->setSortingEnabled(true);
	treeWidget->setItemDelegate(new XkorWC36RPBonusDelegate);
	treeWidget->sortItems(0, Qt::AscendingOrder);
	setUseTeamBonus();
	
	// set the column widths
	treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	treeWidget->header()->setSectionResizeMode(1, QHeaderView::Fixed);
	treeWidget->header()->resizeSection(1, 150);
	treeWidget->header()->setSectionResizeMode(2, QHeaderView::Fixed);
	treeWidget->header()->resizeSection(2, 100);
	
	setupLayout();
}

QHash<QString, QHash<QString, double> > XkorWC36RPBonusWidget::bonuses()
{
	QHash<QString, QHash<QString, double> > rval;
	for(QTreeWidgetItemIterator i(treeWidget); *i; ++i)
	{
		QHash<QString, double> properties;
		double bestRPLevel = (*i)->data(1, 1093).toDouble();
		double quantity = (*i)->text(2).toDouble();
		properties.insert("bestRPLevel", bestRPLevel);
		properties.insert("quantity", quantity);
		properties.insert("bonus", calculateBonus(bestRPLevel, quantity));
		rval[(*i)->text(0)] = properties;
	}
	return rval;
}

double XkorWC36RPBonusWidget::calculateBonus(double bestRPLevel, double quantity)
{
	double rpMultiplier = (bestRPLevel + 1) * bestRPLevel / 2 + 1;
	double quantityMultiplier = 0;
	if(quantity >= matchday->value() / 2)
		quantityMultiplier = 1.4;
	else if(quantity > 0)
		quantityMultiplier = 1;
	return rpMultiplier * quantityMultiplier;
}

void XkorWC36RPBonusWidget::initItem(QTreeWidgetItem * item, QString nation, double bestRPLevel, double quantity)
{
	item->setText(0, nation);
	item->setText(1, levelToString(bestRPLevel));
	item->setData(1, 1093, bestRPLevel);
	item->setText(2, QString::number(quantity));
}

QString XkorWC36RPBonusWidget::levelToString(double level)
{
	if(level == 4)
		return "Exceptional";
	else if(level == 3)
		return "Great";
	else if(level == 2)
		return "Good";
	else if(level == 1)
		return "Poor";
	else
		return "Slani";
}

QHash<QString, QVariant> XkorWC36RPBonusWidget::options()
{
	QHash<QString, QVariant> rval;
	rval.insert("matchday", matchday->value());
	return rval;
}

void XkorWC36RPBonusWidget::setBonuses(QHash<QString, QHash<QString, double> > bonuses)
{
	clear();
	for(QHash<QString, QHash<QString, double> >::iterator i = bonuses.begin(); i != bonuses.end(); ++i)
		initItem(createItem(), i.key(), i.value()["bestRPLevel"], i.value()["quantity"]);
	emit listChanged();
}

void XkorWC36RPBonusWidget::setupLayout()
{
	// tool bar
	QToolBar * toolBar = new QToolBar;
        toolBar->setIconSize(QSize(this->style()->pixelMetric(QStyle::PM_SmallIconSize), this->style()->pixelMetric(QStyle::PM_SmallIconSize)));
	toolBar->addAction(insertAction);
	toolBar->addAction(deleteAction);

	layout = new QGridLayout(this);
	layout->addLayout(matchdayForm, 0, 0);
	layout->addWidget(treeWidget, 1, 0);
	layout->addWidget(toolBar, 2, 0, Qt::AlignCenter);

	layout->setContentsMargins(0, 0, 0, 0);
}

void XkorWC36RPBonusWidget::setListChanged()
{
	emit listChanged();
}

void XkorWC36RPBonusWidget::setOptions(QHash<QString, QVariant> options)
{
	matchday->setValue(options["matchday"].toInt());
}
