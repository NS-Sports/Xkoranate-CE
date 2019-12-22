#ifndef RPLIST_H
#define RPLIST_H

#include <QtCore>

class XkorRPList
{
	private:
		QString m_competitionName;
		QHash<QString, QHash<QString, double> > bon;
		QString rpCalcType;
		QHash<QString, QVariant> rpOpts;
		double max, min;
		double eff;
		bool teams;
	public:
		XkorRPList();
		void addBonus(QPair<QString, QHash<QString, double> > properties) { bon.insert(properties.first, properties.second); }
		double bonus(const QString & code);
		QHash<QString, QHash<QString, double> > bonuses() { return bon; }
		QString competitionName() { return m_competitionName; }
		double maxBonus() { return max; }
		double minBonus() { return min; }
		QString rpCalculationType() { return rpCalcType; }
		double rpEffect() { return eff; }
		QHash<QString, QVariant> rpOptions() { return rpOpts; }
		bool useTeams() { return teams; }
		bool useWGStyleBonus() { return (rpCalcType == "relative"); }
		void setBonuses(QHash<QString, QHash<QString, double> > newBonuses) { bon = newBonuses; }
		void setCompetitionName(QString newName) { m_competitionName = newName; }
		void setMaxBonus(double newMax) { max = newMax; }
		void setMinBonus(double newMin) { min = newMin; }
		void setRPCalculationType(QString newType) { rpCalcType = newType; }
		void setRPEffect(double newEffect) { eff = newEffect; }
		void setRPOptions(QHash<QString, QVariant> newOpts) { rpOpts = newOpts; }
		void setUseTeams(bool newValue) { teams = newValue; }

};

#endif
