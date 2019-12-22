#ifndef SIGNUPLIST_H
#define SIGNUPLIST_H

#include <QtCore>
#include <tr1/random>

#include "athlete.h"
#include "exceptions.h"

class XkorSignupList
{
	private:
		QList<XkorAthlete> ath;
		double min, max;

		std::tr1::mt19937 r;

		QUuid generateID();
	public:
		XkorSignupList();

		void addAthlete(XkorAthlete a);
		double adjustRank(double rank);
		QList<XkorAthlete> athletes();
		XkorAthlete getAthleteByID(const QUuid id) throw(XkorSearchFailedException);
		double maxRank();
		double minRank();
		void setAthletes(QList<XkorAthlete> newAthletes);
		void setMaxRank(double newMax);
		void setMinRank(double newMin);
};

#endif
