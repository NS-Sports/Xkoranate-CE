#include "xmltablereader.h"
#include <QRegularExpression>

XkorXmlTableReader::XkorXmlTableReader(QString filename)
{
	QFile * f = new QFile(filename);
	if(!f->exists())
	{
		raiseError(QObject::tr("File ‘%1’ not found by XkorXmlTableReader::XkorXmlTableReader(QString)").arg(filename));
		return;
	}
	f->open(QIODevice::ReadOnly);
	setDevice(f);

	while(!atEnd())
	{
		if(isStartElement())
		{
			if(name() == "table" && attributes().value("version") == "0.3")
				readFile();
			else
				raiseError(QObject::tr("This file is not an xkoranate version 0.3 table."));
		}
		readNext();
	}

	if(hasError())
		qDebug() << error();

	delete f;
}

QString XkorXmlTableReader::error() const
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

double XkorXmlTableReader::readDouble()
{
	return readElementText().toDouble();
}

void XkorXmlTableReader::readFile()
{
	while(!atEnd())
	{
		readNext();
		if(isEndElement())
			break;
		if(isStartElement())
		{
			if(name() == "sortCriteria")
				m_table.setSortCriteria(readSortCriteria());
			else if(name() == "pointsForWin")
				m_table.setPointsForWin(readDouble());
			else if(name() == "pointsForDraw")
				m_table.setPointsForDraw(readDouble());
			else if(name() == "pointsForLoss")
				m_table.setPointsForLoss(readDouble());
			else if(name() == "columnWidth")
				m_table.setColumnWidth(readInt());
			else if(name() == "goalName")
				m_table.setGoalName(readString());
			else if(name() == "showDraws")
				m_table.setShowDraws(readString() == "true");
			else if(name() == "showResultsGrid")
				m_table.setShowResultsGrid(readString() == "true");
			else if(name() == "matches")
				readMatches();
			else
				readUnknownElement();
		}
	}
}

int XkorXmlTableReader::readInt()
{
	return readElementText().toInt();
}

void XkorXmlTableReader::readMatches()
{
	std::vector<XkorTableMatch> matchesList;

	while(!atEnd())
	{
		readNext();
		if(isEndElement())
			break;
		if(isStartElement())
		{
			if(name() == "match")
			{
				QString matchText = readString();

				QRegularExpression rx("([0-9]+)[-–:]([0-9]+)"); // match scores of form Aquilla 3–1 Busby, with en dash, hyphen-minus, or colon as delimiter
				QRegularExpressionMatch match = rx.match(matchText);
				if(match.hasMatch())
				{
					QString homeTeam = matchText.left(match.capturedStart() - 1);
					QString awayTeam = matchText.right(matchText.size() - match.capturedStart() - match.capturedLength() - 1);
					double homeScore = match.captured(1).toDouble();
					double awayScore = match.captured(2).toDouble();
					matchesList.push_back(XkorTableMatch(homeTeam, awayTeam, homeScore, awayScore));
				}
				m_matches += matchText + "\n";
			}
			else
				readUnknownElement();
		}
	}

	m_table.setMatches(matchesList);
}

std::vector<QString> XkorXmlTableReader::readSortCriteria()
{
	std::vector<QString> rval;

	while(!atEnd())
	{
		readNext();
		if(isEndElement())
			break;
		if(isStartElement())
		{
			if(name() == "sortCriterion")
				rval.push_back(readString());
			else
				readUnknownElement();
		}
	}

	return rval;
}

QString XkorXmlTableReader::readString()
{
	return readElementText();
}

void XkorXmlTableReader::readUnknownElement()
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

