#include "xmlreader.h"

#include "exceptions.h"

XkorXmlReader::XkorXmlReader(QString filename)
{
	m_rpList = 0;

	QFile * f = new QFile(filename);
	if(!f->exists())
	{
		raiseError(QObject::tr("File ‘%1’ not found by XkorXmlReader::XkorXmlReader(QString)").arg(filename));
		return;
	}
	f->open(QIODevice::ReadOnly);
	setDevice(f);

	while(!atEnd())
	{
		if(isStartElement())
		{
			if(name() == "scorinationFile" && attributes().value("version") == "0.3")
				readFile();
			else
				raiseError(QObject::tr("This file is not an xkoranate version 0.3 file."));
		}
		readNext();
	}

	if(hasError())
		qDebug() << error();

	delete f;
}

QString XkorXmlReader::error() const
{
	QXmlStreamReader::Error err = QXmlStreamReader::error();
	QString rval;
	switch(err)
	{
		case 0:
			return QString(); // no error
		case 1:
			rval = QObject::tr("An unexpected element was found.");
			break;
		case 3:
			rval = QObject::tr("The input file was not well-formed XML.");
			break;
		case 4:
			rval = QObject::tr("The document ended unexpectedly.");
			break;
		case 2:
			rval = errorString();
			break;
	}
	rval += QObject::tr("\nError occured at line %1, column %2.").arg(QString::number(lineNumber())).arg(QString::number(columnNumber()));
	return rval;
}

double XkorXmlReader::readDouble()
{
	return readElementText().toDouble();
}

void XkorXmlReader::readEvent()
{
	XkorEvent * event = new XkorEvent;
	QUuid id = QUuid(attributes().value("id").toString());
	event->setName(attributes().value("name").toString());
	while(!atEnd())
	{
		readNext();
		if(isEndElement())
			break;
		if(isStartElement())
		{
			if(name() == "sport")
				event->setSport(readString());
			else if(name() == "competition")
				event->setCompetition(readString());
			else if(name() == "paradigmOptions")
				event->setParadigmOptions(readOptions());
			else if(name() == "competitionOptions")
				event->setCompetitionOptions(readOptions());
			else if(name() == "results")
				event->setResults(readResults());
			else if(name() == "signupList")
				event->setSignupList(readEventSignupList());
			else if(name() == "group")
				event->addGroup(readGroup());
			else
				readUnknownElement();
		}
	}
	m_events.append(qMakePair(id, event));
}

XkorSignupList XkorXmlReader::readEventSignupList()
{
	XkorSignupList rval;

	while(!atEnd())
	{
		readNext();
		if(isEndElement())
			break;
		if(isStartElement())
		{
			if(name() == "maxRank")
				rval.setMaxRank(readDouble());
			else if(name() == "minRank")
				rval.setMinRank(readDouble());
			else if(name() == "signup")
				rval.addAthlete(readSignup());
			else
				readUnknownElement();
		}
	}
	return rval;
}

void XkorXmlReader::readFile()
{
	while(!atEnd())
	{
		readNext();
		if(isEndElement())
			break;
		if(isStartElement())
		{
			if(name() == "rpList")
				readRPList();
			else if(name() == "event")
				readEvent();
			else
				readUnknownElement();
		}
	}
}

XkorGroup XkorXmlReader::readGroup()
{
	XkorGroup rval;
	rval.name = attributes().value("name").toString();
	while(!atEnd())
	{
		readNext();
		if(isEndElement())
			break;
		if(isStartElement())
		{
			if(name() == "signup")
				rval.athletes.append(QUuid(readString()));
			else
				readUnknownElement();
		}
	}
	return rval;
}

int XkorXmlReader::readInt()
{
	return readElementText().toInt();
}

QList<QVariant> XkorXmlReader::readList()
{
	QList<QVariant> rval;
	while(!atEnd())
	{
		readNext();
		if(isEndElement())
			break;
		if(isStartElement())
		{
			if(name() == "double")
			{
				double value = readDouble();
				rval.append(value);
			}
			else if(name() == "int")
			{
				int value = readInt();
				rval.append(value);
			}
			else if(name() == "list")
			{
				QList<QVariant> value = readList();
				rval.append(QVariant(value)); // explicit conversion to QVariant needed to avoid calling append(QList<>)
			}
			else if(name() == "string")
			{
				QString value = readString();
				rval.append(value);
			}
			else
				readUnknownElement();
		}
	}
	// readNext(); // skip end element
	return rval;
}

QPair<QString, QHash<QString, double> > XkorXmlReader::readNation()
{
	QPair<QString, QHash<QString, double> > rval;
	rval.first = attributes().value("name").toString();

	while(!atEnd())
	{
		readNext();
		if(isEndElement())
			break;
		if(isStartElement())
		{
			if(name() == "property")
			{
				QString type = attributes().value("type").toString();
				double value = readDouble();
				rval.second.insert(type, value);
			}
			else
				readUnknownElement();
		}
	}
	return rval;
}

QHash<QString, QVariant> XkorXmlReader::readOptions()
{
	QHash<QString, QVariant> rval;
	while(!atEnd())
	{
		readNext();
		if(isEndElement())
			break;
		if(isStartElement())
		{
			QString type = attributes().value("type").toString();
			if(name() == "double")
			{
				double value = readDouble();
				rval.insert(type, value);
			}
			else if(name() == "int")
			{
				int value = readInt();
				rval.insert(type, value);
			}
			else if(name() == "list")
			{
				QList<QVariant> value = readList();
				rval.insert(type, value);
			}
			else if(name() == "string")
			{
				QString value = readString();
				rval.insert(type, value);
			}
			else
				readUnknownElement();
		}
	}
	return rval;
}

QHash<int, QString> XkorXmlReader::readResults()
{
	QHash<int, QString> rval;
	while(!atEnd())
	{
		readNext();
		if(isEndElement())
			break;
		if(isStartElement())
		{
			if(name() == "result")
			{
				int matchday = attributes().value("matchday").toString().toInt();
				rval[matchday] = readString();
			}
			else
				readUnknownElement();
		}
	}
	return rval;
}

void XkorXmlReader::readRPList()
{
	delete m_rpList;
	m_rpList = new XkorRPList;

	while(!atEnd())
	{
		readNext();
		if(isEndElement())
			break;
		if(isStartElement())
		{
			if(name() == "competitionName")
				m_rpList->setCompetitionName(readString());
			else if(name() == "maxBonus")
				m_rpList->setMaxBonus(readDouble());
			else if(name() == "minBonus")
				m_rpList->setMinBonus(readDouble());
			else if(name() == "nation")
				m_rpList->addBonus(readNation());
			else if(name() == "rpCalculationType")
				m_rpList->setRPCalculationType(readString());
			else if(name() == "rpEffect")
				m_rpList->setRPEffect(readDouble());
			else if(name() == "rpOptions")
				m_rpList->setRPOptions(readOptions());
			else if(name() == "useTeams")
				m_rpList->setUseTeams(readString() != "false");
			else
				readUnknownElement();
		}
	}
}

XkorAthlete XkorXmlReader::readSignup()
{
	XkorAthlete ath;
	ath.id = QUuid(attributes().value("id").toString());
	ath.name = attributes().value("name").toString();
	ath.nation = attributes().value("nation").toString();
	ath.skill = attributes().value("skill").toString().toDouble();
	ath.properties = readOptions();

	readNext();
	return ath;
}

QString XkorXmlReader::readString()
{
	return readElementText();
}

void XkorXmlReader::readUnknownElement()
{
	while(!atEnd())
	{
		readNext();
		if(isEndElement())
			break;
		if(isStartElement())
			readUnknownElement();
	}
}
