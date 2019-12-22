#ifndef TABLEROW_H
#define TABLEROW_H

#include <QHash>
#include <QStringList>

#include "tablematch.h"

class XkorTableRow
{
	private:
		QString n; // name
		QHash<QString, double> scores;
		QMultiHash<QString, XkorTableMatch> h2hResults;
	public:
		XkorTableRow() {;;;}
		XkorTableRow(QString name) : n(name) {;;;}
		double awayGoals() const;
		double draws() const;
		double goalsAgainst() const;
		double goalDifference() const;
		double goalsFor() const;
		double h2hAwayGoals(QStringList & teams) const;
		double h2hGoalDifference(QStringList & teams) const;
		double h2hGoalsAgainst(QStringList & teams) const;
		double h2hGoalsFor(QStringList & teams) const;
		double h2hPoints(QStringList & teams) const;
		double h2hWins(QStringList & teams) const;
		void insertMatch(QString t2name, double t1score, double t2score, bool home);
		double losses() const;
		QString name() const;
		double played() const;
		std::pair<double, double> result(QString opponent) const;
		void setValue(QString columnName, double value);
		double wins() const;
		double value(QString columnName) const;
};

#endif
