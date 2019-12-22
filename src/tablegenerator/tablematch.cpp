#include "tablematch.h"

double XkorTableMatch::goalDifference(QString team) const
{
	double rval = 0;
	if(team == team1)
		rval = score1 - score2;
	else if(team == team2)
		rval = score2 - score1;
	return rval;
}

double XkorTableMatch::goalsAgainst(QString team) const
{
	double rval = 0;
	if(team == team1)
		rval = score2;
	else if(team == team2)
		rval = score1;
	return rval;
}

double XkorTableMatch::goalsFor(QString team) const
{
	double rval = 0;
	if(team == team1)
		rval = score1;
	else if(team == team2)
		rval = score2;
	return rval;
}

QString XkorTableMatch::opponent(QString team) const
{
	QString rval;
	if(team == team1)
		rval = team2;
	else if(team == team2)
		rval = team1;
	return rval;
}

double XkorTableMatch::points(QString team) const
{
	double rval = 0;
	if(team == team1)
	{
		if(score1 > score2)
			rval = 3;
		else if(score1 == score2)
			rval = 1;
	}
	else if(team == team2)
	{
		if(score2 > score1)
			rval = 3;
		else if(score1 == score2)
			rval = 1;
	}
	return rval;
}
