#include "tablerow.h"

double XkorTableRow::awayGoals() const
{
	return scores["_awayGoals"];
}

double XkorTableRow::draws() const
{
	return scores["_draws"];
}

double XkorTableRow::goalsAgainst() const
{
	return scores["_goalsAgainst"];
}

double XkorTableRow::goalDifference() const
{
	return scores["_goalDifference"];
}

double XkorTableRow::goalsFor() const
{
	return scores["_goalsFor"];
}

double XkorTableRow::h2hAwayGoals(QStringList & teams) const
{
	double rval = 0;
	for(QStringList::const_iterator i = teams.begin(); i != teams.end(); ++i)
	{
		QList<XkorTableMatch> teamResults = h2hResults.values(*i);
		for(QList<XkorTableMatch>::const_iterator j = teamResults.begin(); j != teamResults.end(); ++j)
			if(j->team2 == n) // if this team was away
				rval += j->goalsFor(n);
	}
	return rval;
}

double XkorTableRow::h2hGoalDifference(QStringList & teams) const
{
	double rval = 0;
	for(QStringList::const_iterator i = teams.begin(); i != teams.end(); ++i)
	{
		QList<XkorTableMatch> teamResults = h2hResults.values(*i);
		for(QList<XkorTableMatch>::const_iterator j = teamResults.begin(); j != teamResults.end(); ++j)
			rval += j->goalDifference(n);
	}
	return rval;
}

double XkorTableRow::h2hGoalsAgainst(QStringList & teams) const
{
	double rval = 0;
	for(QStringList::const_iterator i = teams.begin(); i != teams.end(); ++i)
	{
		QList<XkorTableMatch> teamResults = h2hResults.values(*i);
		for(QList<XkorTableMatch>::const_iterator j = teamResults.begin(); j != teamResults.end(); ++j)
			rval += j->goalsAgainst(n);
	}
	return rval;
}

double XkorTableRow::h2hGoalsFor(QStringList & teams) const
{
	double rval = 0;
	for(QStringList::const_iterator i = teams.begin(); i != teams.end(); ++i)
	{
		QList<XkorTableMatch> teamResults = h2hResults.values(*i);
		for(QList<XkorTableMatch>::const_iterator j = teamResults.begin(); j != teamResults.end(); ++j)
			rval += j->goalsFor(n);
	}
	return rval;
}

double XkorTableRow::h2hPoints(QStringList & teams) const
{
	double rval = 0;
	for(QStringList::const_iterator i = teams.begin(); i != teams.end(); ++i)
	{
		QList<XkorTableMatch> teamResults = h2hResults.values(*i);
		for(QList<XkorTableMatch>::const_iterator j = teamResults.begin(); j != teamResults.end(); ++j)
			rval += j->points(n);
	}
	return rval;
}

double XkorTableRow::h2hWins(QStringList & teams) const
{
	double rval = 0;
	for(QStringList::const_iterator i = teams.begin(); i != teams.end(); ++i)
	{
		QList<XkorTableMatch> teamResults = h2hResults.values(*i);
		for(QList<XkorTableMatch>::const_iterator j = teamResults.begin(); j != teamResults.end(); ++j)
			rval += (j->points(n) == 3 ? 1 : 0);
	}
	return rval;
}

void XkorTableRow::insertMatch(QString t2name, double t1score, double t2score, bool home)
{
	if(home)
		h2hResults.insert(t2name, XkorTableMatch(n, t2name, t1score, t2score));
	else
	{
		h2hResults.insert(t2name, XkorTableMatch(t2name, n, t2score, t1score));
		scores["_awayGoals"] += t1score;
	}
	++scores["_played"];
	if(t1score > t2score)
		++scores["_wins"];
	else if(t1score == t2score)
		++scores["_draws"];
	else
		++scores["_losses"];
	scores["_goalsFor"] += t1score;
	scores["_goalsAgainst"] += t2score;
	scores["_goalDifference"] += t1score - t2score;
}

double XkorTableRow::losses() const
{
	return scores["_losses"];
}

QString XkorTableRow::name() const
{
	return n;
}

double XkorTableRow::played() const
{
	return scores["_played"];
}

std::pair<double, double> XkorTableRow::result(QString opponent) const
{
	std::pair<double, double> rval = std::make_pair(-1, -1);
	QList<XkorTableMatch> teamResults = h2hResults.values(opponent);
	for(QList<XkorTableMatch>::const_iterator i = teamResults.begin(); i != teamResults.end(); ++i)
		if(i->team1 == n && i->team2 == opponent)
			rval = std::make_pair(i->score1, i->score2);
	return rval;
}

void XkorTableRow::setValue(QString columnName, double value)
{
	scores[columnName] = value;
}

double XkorTableRow::value(QString columnName) const
{
	return scores[columnName];
}

double XkorTableRow::wins() const
{
	return scores["_wins"];
}
