#ifndef ABSTRACTRPBONUSWIDGET_H
#define ABSTRACTRPBONUSWIDGET_H

#include "abstracttreewidget.h"

class XkorAbstractRPBonusWidget : public XkorAbstractTreeWidget
{
	Q_OBJECT
	
	public:
		virtual QHash<QString, QHash<QString, double> > bonuses() = 0;
		virtual double maxBonus() { return 1; }
		virtual double minBonus() { return 0; }
		virtual QHash<QString, QVariant> options() { return QHash<QString, QVariant>(); }
		virtual void setBonuses(QHash<QString, QHash<QString, double> > bonuses) = 0;
		virtual void setMaxBonus(double) {;;;}
		virtual void setMinBonus(double) {;;;}
		virtual void setOptions(QHash<QString, QVariant>) {;;;}
	public slots:
		virtual void setUseParticipantBonus(bool = true) = 0;
		virtual void setUseTeamBonus(bool = true) = 0;
};

#endif
