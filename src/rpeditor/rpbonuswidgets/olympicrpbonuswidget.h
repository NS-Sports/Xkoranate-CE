#ifndef OLYMPICRPBONUSWIDGET_H
#define OLYMPICRPBONUSWIDGET_H

#include "rpeditor/abstractrpbonuswidget.h"

#include <QTreeWidgetItem>

class XkorOlympicRPBonusWidget : public XkorAbstractRPBonusWidget
{
	Q_OBJECT
	
	protected:
		virtual void initItem(QTreeWidgetItem * item) { initItem(item, QString(), 0); }
		virtual void initItem(QTreeWidgetItem * item, QString nation, double bonus);
		virtual QString insertionText() { return tr("Add nation"); }
		virtual QString deletionText() { return tr("Remove nations"); }
	
	public:
		XkorOlympicRPBonusWidget();
		QHash<QString, QHash<QString, double> > bonuses();
		void setBonuses(QHash<QString, QHash<QString, double> > bonuses);
	public slots:
		void setUseParticipantBonus(bool use = true) { if(use) treeWidget->setHeaderLabels(QStringList() << tr("Participant") << tr("Bonus")); }
		void setUseTeamBonus(bool use = true) { if(use) treeWidget->setHeaderLabels(QStringList() << tr("Team") << tr("Bonus")); }
};

#endif
