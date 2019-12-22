#ifndef ATHLETE_H
#define ATHLETE_H

#include <QtCore>

struct XkorAthlete
{
	QString name;
	double rpBonus;
	double rpSkill; // modified skill after RP bonus is applied
	QUuid id;
	double skill;
	QString nation;
	QHash<QString, QVariant> properties;
	
	XkorAthlete() {;;;}
	XkorAthlete(QUuid newID);
	QVariant property(QString key);
	void setProperty(QString key, QVariant value);
	bool operator < (const XkorAthlete & that) const;
	bool operator == (const XkorAthlete & that) const;
};

#endif
