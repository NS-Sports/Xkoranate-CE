#include "signuplist.h"

#include <algorithm>
#include <ctime>

XkorSignupList::XkorSignupList()
{
	r.seed(time(0));
	min = 0;
	max = 1;
}

void XkorSignupList::addAthlete(XkorAthlete a)
{
	if(a.id == QUuid())
		a.id = generateID();
	ath.append(a);
}

double XkorSignupList::adjustRank(double rank)
{
	return (rank - min) / (max - min);
}

QList<XkorAthlete> XkorSignupList::athletes()
{
	return ath;
}

QUuid XkorSignupList::generateID()
{
	return QUuid(r(), r(), r(), r(), r(), r(), r(), r(), r(), r(), r());
}

XkorAthlete XkorSignupList::getAthleteByID(const QUuid id) noexcept(false)
{
	for(QList<XkorAthlete>::iterator i = ath.begin(); i != ath.end(); ++i)
		if(i->id == id)
			return *i;

	std::string err("Failed to find athlete with ID ");
	err = err + id.toString().toStdString();
	err = err + " in XkorSignupList::getAthleteByID(const int)";
	throw XkorSearchFailedException(err);
}

double XkorSignupList::maxRank()
{
	return max;
}

double XkorSignupList::minRank()
{
	return min;
}

void XkorSignupList::setAthletes(QList<XkorAthlete> newAthletes)
{
	for(QList<XkorAthlete>::iterator i = newAthletes.begin(); i != newAthletes.end(); ++i)
		if(i->id == QUuid())
			i->id = generateID();
	ath = newAthletes;
}

void XkorSignupList::setMaxRank(double newMax)
{
	max = newMax;
}

void XkorSignupList::setMinRank(double newMin)
{
	min = newMin;
}
