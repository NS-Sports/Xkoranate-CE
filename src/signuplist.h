#ifndef SIGNUPLIST_H
#define SIGNUPLIST_H

#include <QtCore>
#include <random>

#include "athlete.h"
#include "exceptions.h"

class XkorSignupList
{
	private:
		QList<XkorAthlete> ath;
		double min, max;

        std::mt19937 r;

		QUuid generateID();
	public:
		XkorSignupList();

		void addAthlete(XkorAthlete a);
		double adjustRank(double rank);
		QList<XkorAthlete> athletes();
		XkorAthlete getAthleteByID(const QUuid id) noexcept(false);
		double maxRank();
		double minRank();
		void setAthletes(QList<XkorAthlete> newAthletes);
		void setMaxRank(double newMax);
		void setMinRank(double newMin);
};

#endif
