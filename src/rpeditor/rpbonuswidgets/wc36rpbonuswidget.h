#ifndef WC36RPBONUSWIDGET_H
#define WC36RPBONUSWIDGET_H

#include <QtGui>

#include "rpeditor/abstractrpbonuswidget.h"

#include <QFormLayout>
#include <QSpinBox>
#include <QTreeWidgetItem>

class XkorWC36RPBonusWidget : public XkorAbstractRPBonusWidget
{
	Q_OBJECT
	
	protected:
		QFormLayout * matchdayForm;
		QSpinBox * matchday;
		
		virtual void initItem(QTreeWidgetItem * item) { initItem(item, QString(), 0, 0); }
		virtual void initItem(QTreeWidgetItem * item, QString nation, double bestRPLevel, double quantity);
		virtual void setupLayout();
		virtual QString insertionText() { return tr("Add nation"); }
		virtual QString deletionText() { return tr("Remove nations"); }
		
		double calculateBonus(double bestRPLevel, double quantity);
		QString levelToString(double level);
	protected slots:
		void setListChanged();
	
	public:
		XkorWC36RPBonusWidget();
		QHash<QString, QHash<QString, double> > bonuses();
		double maxBonus() { return 15.4; }
		double minBonus() { return 0; }
		QHash<QString, QVariant> options();
		void setBonuses(QHash<QString, QHash<QString, double> > bonuses);
		void setMaxBonus(double) {;;;}
		void setMinBonus(double) {;;;}
		void setOptions(QHash<QString, QVariant> options);
	public slots:
		void setUseParticipantBonus(bool use = true) { if(use) treeWidget->setHeaderLabels(QStringList() << tr("Participant") << tr("Level") << tr("Quantity")); }
		void setUseTeamBonus(bool use = true) { if(use) treeWidget->setHeaderLabels(QStringList() << tr("Team") << tr("Level") << tr("Quantity")); }
};

#endif
