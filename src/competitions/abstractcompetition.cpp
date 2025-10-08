#include "competitions/abstractcompetition.h"


#include <QTextStream>

void XkorAbstractCompetition::init(XkorStartList & sl, XkorSport & s, QHash<QString, QVariant> paradigmOptions, QHash<QString, QVariant> competitionOptions, QHash<int, QString> results)
{
	startList = sl;
	sport = s;
	paradigmOpt = paradigmOptions;
	userOpt = competitionOptions;
	resultsBuf = results;
}

int XkorAbstractCompetition::nameWidth()
{
	int maximumWidth = 20;
	int showTeamNames = (paradigmOpt.value("showTLAs", "true").toString() == "true" ? 1 : 0);
	std::vector<XkorStartListGroup> groups = startList.groups;
	for(std::vector<XkorStartListGroup>::iterator i = groups.begin(); i != groups.end(); ++i)
	{
		QList<XkorAthlete> athletes = i->athletes;
		for(QList<XkorAthlete>::iterator j = athletes.begin(); j != athletes.end(); ++j)
			if(j->name.size() + showTeamNames * (3 + j->nation.size()) > maximumWidth)
				maximumWidth = j->name.size() + showTeamNames * (3 + j->nation.size());
	}
	return maximumWidth;
}

QString XkorAbstractCompetition::rankedListOutput(QString title, QVector<XkorResult> results, XkorAbstractResultComparator * comparator)
{
	QString rval;
	int rankDigits = (int)log10(results.size()) + 1;
	rval = QString(rankDigits + 1, ' ') + title + "\n";
	XkorResult prev;
	for(int i = 0; i < results.size(); ++i)
	{
		if(!(prev.athlete == XkorAthlete()) && ((!(*comparator)(prev, results[i]) && !(*comparator)(results[i], prev)) || (!comparator->isRankable(prev) && !comparator->isRankable(results[i])))) // if prev.athlete is set && (prev == results[i] || (!isRankable(prev) && !isRankable(results[i])))
			rval += QString(rankDigits + 1, ' ') + results[i].output() + "\n";
		else if(!comparator->isRankable(results[i]) && comparator->isRankable(prev))
			rval += QString("—").rightJustified(rankDigits) + " " + results[i].output() + "\n";
		else
			rval += QString::number(i + 1).rightJustified(rankDigits) + " " + results[i].output() + "\n";
		prev = results[i];
	}
	rval += "\n";
	return rval;
}

QString XkorAbstractCompetition::results(int matchday)
{
	return resultsBuf[matchday];
}

QHash<QString, QVariant> XkorAbstractCompetition::resumeFileOptions()
{
	return resumeOpt;
}
