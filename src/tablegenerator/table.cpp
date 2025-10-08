#include "tablegenerator/table.h"


#include <algorithm>
#include <QFile>

#include <QTextStream>

void XkorTable::addMatchToData(XkorTableMatch m)
{
	QHash<QString, XkorTableRow>::iterator i = findTeam(m.team1);
	i->insertMatch(m.team2, m.score1, m.score2, true);
	i = findTeam(m.team2);
	i->insertMatch(m.team1, m.score2, m.score1, false);
}

void XkorTable::collapse(std::vector<std::vector<XkorTableRow> > & target, const std::vector<std::vector<XkorTableRow> > & source)
{
	for(std::vector<std::vector<XkorTableRow> >::const_iterator i = source.begin(); i != source.end(); ++i)
		target.push_back(*i);
}

QHash<QString, XkorTableRow>::iterator XkorTable::findTeam(QString name)
{
	QHash<QString, XkorTableRow>::iterator rval = unsortedData.find(name);
	if(rval == unsortedData.end())
		rval = unsortedData.insert(name, XkorTableRow(name));
	return rval;
}

void XkorTable::generate()
{
	data.clear();
	
	std::vector<XkorTableRow> oldData;
	oldData.reserve(unsortedData.size());
	for(QHash<QString, XkorTableRow>::iterator i = unsortedData.begin(); i != unsortedData.end(); ++i)
		oldData.push_back(i.value());
	
	collapse(data, sortTable(oldData));
}

double XkorTable::getPoints(const XkorTableRow & a)
{
	return a.wins() * pointsForWin + a.draws() * pointsForDraw + a.losses() * pointsForLoss;
}

void XkorTable::insertMatch(XkorTableMatch m)
{
	matches.push_back(m);
	addMatchToData(m);
}

std::vector<std::vector<XkorTableRow> > XkorTable::sortTable(const std::vector<XkorTableRow> & rows)
{
	std::vector<std::vector<XkorTableRow> > rval;
	
	if(rows.size() > 0)
	{
		std::vector<std::vector<XkorTableRow> > sorted;
		
		for(std::vector<QString>::iterator i = sortCriteria.begin(); i != sortCriteria.end(); ++i)
		{
			sorted = sorter.sort(rows, *i);
			if(sorted.size() > 1)
			{
				for(std::vector<std::vector<XkorTableRow> >::iterator i = sorted.begin(); i != sorted.end(); ++i)
					collapse(rval, sortTable(*i));
				return rval;
			}
		}
		
		// if the teams are tied
		sorted = sorter.sort(rows, "alphabetical");
		rval.push_back(sorted[0]);
	}
	else
		rval.push_back(std::vector<XkorTableRow>());
	return rval;
}

QString XkorTable::toText()
{
	QString rval;
	
	// header
	for(std::vector<XkorTableColumn>::iterator k = columns.begin(); k != columns.end(); ++k)
	{
		if(k->columnType == "name")
			rval += (k->heading).leftJustified(k->width);
		else
			rval += (k->heading).rightJustified(k->width);
		rval += " ";
	}
	rval += "\n";
	
	int rank = 1;
	int acc = 1;
	for(std::vector<std::vector<XkorTableRow> >::iterator i = data.begin(); i != data.end(); ++i)
	{
		for(std::vector<XkorTableRow>::iterator j = i->begin(); j != i->end(); ++j)
		{
			for(std::vector<XkorTableColumn>::iterator k = columns.begin(); k != columns.end(); ++k)
			{
				if(k != columns.begin())
					rval += " ";
				if(k->columnType == "position")
				{
					if(rank == acc)
						rval += QString::number(rank).rightJustified(k->width);
					else
						rval += QString(k->width, ' ');
				}
				else if(k->columnType == "name")
					rval += j->name().leftJustified(k->width);
				else if(k->columnType == "played")
					rval += QString::number(j->played()).rightJustified(k->width);
				else if(k->columnType == "wins")
					rval += QString::number(j->wins()).rightJustified(k->width);
				else if(k->columnType == "draws")
					rval += QString::number(j->draws()).rightJustified(k->width);
				else if(k->columnType == "losses")
					rval += QString::number(j->losses()).rightJustified(k->width);
				else if(k->columnType == "goalsFor")
					rval += QString::number(j->goalsFor()).rightJustified(k->width);
				else if(k->columnType == "goalsAgainst")
					rval += QString::number(j->goalsAgainst()).rightJustified(k->width);
				else if(k->columnType == "goalDifference")
					rval += (QString(j->goalDifference() > 0 ? "+" : (j->goalDifference() < 0 ? "−" : "")) + QString::number(abs((int)j->goalDifference()))).rightJustified(k->width);
				else if(k->columnType == "goalAverage")
					rval += (j->goalsAgainst() == 0 ? QString("∞") : QString::number(j->goalsFor() / j->goalsAgainst(), 'f', 3)).rightJustified(k->width);
				else if(k->columnType == "points")
					rval += QString::number(getPoints(*j)).rightJustified(k->width);
				else if(k->columnType == "winPercent")
					rval += (j->played() == 0 ? QString("undef") : QString::number((j->wins() + j->draws() / 2.0) / j->played(), 'f', 3)).rightJustified(k->width);
				else if(k->columnType == "winPercentNFL")
					rval += (j->wins() + j->losses() == 0 ? QString("undef") : QString::number(j->wins() / (j->wins() + j->losses()), 'f', 3)).rightJustified(k->width);
				else if(k->columnType == "winPercentPure")
					rval += (j->played() == 0 ? QString("undef") : QString::number(j->wins() / j->played(), 'f', 3)).rightJustified(k->width);
				else if(k->columnType == "resultsGrid")
				{
					QString gridLine;
					for(std::vector<std::vector<XkorTableRow> >::iterator y = data.begin(); y != data.end(); ++y)
						for(std::vector<XkorTableRow>::iterator z = y->begin(); z != y->end(); ++z)
						{
							if(y != data.begin())
								gridLine += "  ";
							std::pair<double, double> result = j->result(z->name());
							if(result.first >= 0)
								gridLine += QString::number(result.first) + "–" + QString::number(result.second);
							else
								gridLine += " — ";
						}
					rval += gridLine.rightJustified(k->width);
				}
			}
			rval += "\n";
			++acc;
		}
		rank = acc;
	}
	return rval;
}

void XkorTable::updateMatches()
{
	unsortedData.clear();
	for(std::vector<XkorTableMatch>::iterator i = matches.begin(); i != matches.end(); ++i)
		addMatchToData(*i);
}
