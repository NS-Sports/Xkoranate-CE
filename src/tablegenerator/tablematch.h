#ifndef TABLEMATCH_H
#define TABLEMATCH_H

#include <QString>

class XkorTableMatch
{
	public:
		QString team1;
		QString team2;
		double score1;
		double score2;
		XkorTableMatch(QString t1, QString t2, double s1, double s2) : team1(t1), team2(t2), score1(s1), score2(s2) {;;;}
		double goalDifference(QString team) const;
		double goalsAgainst(QString team) const;
		double goalsFor(QString team) const;
		QString opponent(QString team) const;
		double points(QString team) const;
};

#endif
