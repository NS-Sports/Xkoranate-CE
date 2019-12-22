#include "athlete.h"

XkorAthlete::XkorAthlete(QUuid newID)
{
	id = newID;
}

QVariant XkorAthlete::property(QString key)
{
	if(key == "name")
		return name;
	else if(key == "id")
		return id.toString();
	else if(key == "nation")
		return nation;
	else if(key == "skill")
		return skill;
	else
		return properties[key];
}

void XkorAthlete::setProperty(QString key, QVariant value)
{
	if(key == "name")
		name = value.toString();
	else if(key == "id")
		id = QUuid(value.toString());
	else if(key == "nation")
		nation = value.toString();
	else if(key == "skill")
		skill = value.toDouble();
	else
		properties[key] = value;
}

bool XkorAthlete::operator < (const XkorAthlete & that) const
{
	return (this->id < that.id);

}

bool XkorAthlete::operator == (const XkorAthlete & that) const
{
	return (this->id == that.id);
}
