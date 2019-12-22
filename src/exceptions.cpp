#include "exceptions.h"

XkorFileNotFoundException::XkorFileNotFoundException(std::string errorText, QString fileType, QString fileName) : std::logic_error(errorText)
{
	type = fileType;
	name = fileName;
}

QString XkorFileNotFoundException::fileType()
{
	return type;
}

QString XkorFileNotFoundException::fileName()
{
	return name;
}
