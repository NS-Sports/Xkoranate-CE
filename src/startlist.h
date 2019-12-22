#ifndef STARTLIST_H
#define STARTLIST_H

#include <QString>

#include "athlete.h"
#include "sport.h"
#include <vector>

using namespace std;

struct XkorStartListGroup
{
	QString name;
	QList<XkorAthlete> athletes;
};

struct XkorStartList
{
	QString name;
	std::vector<XkorStartListGroup> groups;
};

#endif
