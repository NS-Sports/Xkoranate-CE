#include "competitions/roundrobincompetition.h"


#include <algorithm>
#include "competitions/options/roundrobincompetitionoptions.h"
#include "paradigms/paradigmfactory.h"

XkorAbstractOptionsWidget * XkorRoundRobinCompetition::newOptionsWidget(QHash<QString, QVariant> competitionOptions)
{
	return new XkorRoundRobinCompetitionOptions(competitionOptions);
}

void XkorRoundRobinCompetition::scorinate(int matchday)
{
	if(largestGroupSize(startList) > 0) // if there are no teams, don’t bother!
	{
		// initialize the tables
		tables.clear();
		std::vector<QString> sortCriteria = tableSortCriteria();
		unsigned int groupNo = 0;
		for(std::vector<XkorStartListGroup>::iterator i = startList.groups.begin(); i != startList.groups.end(); ++i)
		{
			XkorTable t;
			t.setColumns(generateTableColumns(i->name));
			t.setMatches(retrieveMatches(groupNo));
			t.setSortCriteria(sortCriteria);
			t.setPointsForWin(userOpt.contains("pointsForWin") ? userOpt.value("pointsForWin").toInt() : 3);
			t.setPointsForDraw(userOpt.contains("pointsForDraw") ? userOpt.value("pointsForDraw").toInt() : 1);
			t.setPointsForLoss(userOpt.contains("pointsForLoss") ? userOpt.value("pointsForLoss").toInt() : 0);
			tables.append(t);
			++groupNo;
		}

		// initialize the results in the resume file options
		resumeOpt.insert("tableData", userOpt.value("tableData"));
		
		unsigned int numberOfLegs = 1;
		if(userOpt.contains("numberOfLegs"))
			numberOfLegs = userOpt.value("numberOfLegs").toUInt();

		scorinateMatchday(matchday);
		resultsBuf[matchday] += "\n";
	}
}

QList<std::pair<unsigned int, unsigned int> > XkorRoundRobinCompetition::generateFixtures(unsigned int matchday, unsigned int groupSize)
{
	// account for possibility of more than two legs
	bool reverseFixtures = (matchday % (2 * (groupSize - 1)) >= groupSize - 1); // if we’re in the 2nd, 4th, … leg, reverse home/away
	matchday %= (groupSize - 1);
	
	unsigned int matchesPerRound = groupSize / 2;
	QList<std::pair<unsigned int, unsigned int> > fixtures;
	
	// mix up the fixtures a bit
	unsigned int round = matchday / 2;
	if(matchday & 1)
		round += groupSize / 2;
	
	for(unsigned int m = 0; m < matchesPerRound; ++m)
	{
		unsigned int home = (round + m) % (groupSize - 1);
		unsigned int away = (groupSize - 1 - m + round) % (groupSize - 1);
		if(m == 0)
			away = groupSize - 1;
		
		// flip the fixtures if:
		// if this is the first match of an odd matchday, flip so that the last team isn’t always away
		// if we’re on an even-numbered leg (reverseFixtures is set)
		// if both of these are true, “flip it twice,” i.e., don’t flip it (thus use of xor)
		if((m == 0 && (matchday & 1)) xor reverseFixtures)
			fixtures.append(make_pair(away, home));
		else
			fixtures.append(make_pair(home, away));
	}

	return fixtures;
}

std::vector<XkorTableColumn> XkorRoundRobinCompetition::generateTableColumns(QString groupName)
{
	std::vector<XkorTableColumn> columns;

	int nameWidth = this->nameWidth();
	if(groupName.size() > nameWidth)
		nameWidth = groupName.size();
	int matchdayWidth = log10(matchdays()) + 1;
	int positionWidth = log10(largestGroupSize(startList)) + 1;
	QHash<QString, QVariant> opt = sport.paradigmOptions();

	std::vector<QString> sortCriteria = tableSortCriteria();
	bool usesGoalAverage = (find(sortCriteria.begin(), sortCriteria.end(), "goalAverage") != sortCriteria.end());
	bool usesGoalDifference = (find(sortCriteria.begin(), sortCriteria.end(), "goalDifference") != sortCriteria.end());
	bool usesGoalsAgainst = (std::find(sortCriteria.begin(), sortCriteria.end(), "goalsAgainst") != sortCriteria.end());
	bool usesGoalsFor = (find(sortCriteria.begin(), sortCriteria.end(), "goalsFor") != sortCriteria.end());
	bool usesWinPercent = (find(sortCriteria.begin(), sortCriteria.end(), "winPercent") != sortCriteria.end());
	bool usesWinPercentPure = (find(sortCriteria.begin(), sortCriteria.end(), "winPercentPure") != sortCriteria.end());
	bool usesWinPercentNFL = (find(sortCriteria.begin(), sortCriteria.end(), "winPercentNFL") != sortCriteria.end());
	bool usesPoints = (find(sortCriteria.begin(), sortCriteria.end(), "points") != sortCriteria.end());

	columns.push_back(XkorTableColumn("position", "", positionWidth));
	columns.push_back(XkorTableColumn("name", groupName, nameWidth));
	columns.push_back(XkorTableColumn("played", opt.value("tableHeaderPlayed", "Pld").toString(), matchdayWidth + 2));
	columns.push_back(XkorTableColumn("wins", opt.value("tableHeaderWins", "W").toString(), matchdayWidth + 2));
	if(opt.value("forceTieBreak") != 1 && userOpt.value("allowDraws") != "false")
		columns.push_back(XkorTableColumn("draws", opt.value("tableHeaderDraws", "D").toString(), matchdayWidth + 1));
	columns.push_back(XkorTableColumn("losses", opt.value("tableHeaderLosses", "L").toString(), matchdayWidth + 1));
	if(usesGoalAverage || usesGoalDifference || usesGoalsAgainst || usesGoalsFor)
	{
		columns.push_back(XkorTableColumn("goalsFor", opt.value("tableHeaderGoalsFor", "PF").toString(), matchdayWidth + 3));
		columns.push_back(XkorTableColumn("goalsAgainst", opt.value("tableHeaderGoalsAgainst", "PA").toString(), matchdayWidth + 2));
	}
	if(usesGoalAverage)
		columns.push_back(XkorTableColumn("goalAverage", opt.value("tableHeaderGoalAverage", "Avg").toString(), 7));
	if(usesGoalDifference)
		columns.push_back(XkorTableColumn("goalDifference", opt.value("tableHeaderGoalDifference", "PD").toString(), matchdayWidth + 2));
	if(usesWinPercent)
		columns.push_back(XkorTableColumn("winPercent", opt.value("tableHeaderWinPercent", "Win %").toString(), 8));
	if(usesWinPercentPure)
		columns.push_back(XkorTableColumn("winPercentPure", opt.value("tableHeaderWinPercent", "Win %").toString(), 8));
	if(usesWinPercentNFL)
		columns.push_back(XkorTableColumn("winPercentNFL", opt.value("tableHeaderWinPercent", "Win %").toString(), 8));
	if(usesPoints)
		columns.push_back(XkorTableColumn("points", opt.value("tableHeaderPoints", "Pts").toString(), matchdayWidth + 3));
	if(userOpt.value("showResultsGrid") == "true")
		columns.push_back(XkorTableColumn("resultsGrid", "", largestGroupSize(startList) * 5));

	return columns;
}

unsigned int XkorRoundRobinCompetition::largestGroupSize(XkorStartList & sl)
{
	int rval = 0;
	for(std::vector<XkorStartListGroup>::iterator i = sl.groups.begin(); i != sl.groups.end(); ++i)
		if(i->athletes.size() > rval)
			rval = i->athletes.size();
	if(rval & 1)
		rval += 1; // rval should be even
	return rval;
}

int XkorRoundRobinCompetition::matchdays()
{
	if(userOpt.contains("numberOfLegs"))
		return userOpt.value("numberOfLegs").toInt() * (largestGroupSize(startList) - 1);
	else
		return largestGroupSize(startList) - 1;
}

QStringList XkorRoundRobinCompetition::matchdayNames()
{
	QStringList rval;
	for(int i = 0; i < matchdays(); ++i)
		rval << QString("Matchday ") + QString::number(i + 1);
	return rval;
}

std::vector<XkorTableMatch> XkorRoundRobinCompetition::retrieveMatches(unsigned int groupNo)
{
	std::vector<XkorTableMatch> rval;
	
	if(userOpt.contains("tableData"))
	{
		QList<QVariant> tableData = userOpt.value("tableData").toList();
		if((unsigned int)tableData.size() > groupNo)
		{
			QList<QVariant> results = tableData[groupNo].toList();
			for(QList<QVariant>::iterator i = results.begin(); i != results.end(); ++i)
			{
				QRegularExpression rx("[0-9]+: (.+) ([0-9.]+)–([0-9.]+) (.+)"); // match scores of form “1: Aquilla 3–1 Busby”
				QRegularExpressionMatch match = rx.match(i->toString()); if(match.hasMatch())
					rval.push_back(XkorTableMatch(match.captured(1), match.captured(4), match.captured(2).toDouble(), match.captured(3).toDouble()));
			}
		}
	}
	return rval;
}

QHash<QString, QVariant> XkorRoundRobinCompetition::revertToMatchday(int matchday)
{
	// delete the results
	for(QHash<int, QString>::iterator i = resultsBuf.begin(); i != resultsBuf.end(); ++i)
		if(i.key() >= matchday)
			i.value() = "";

	// sort out the table data
	QList<QVariant> tableData = userOpt.value("tableData").toList();
	for(QList<QVariant>::iterator i = tableData.begin(); i != tableData.end(); ++i)
	{
		QList<QVariant> groupData = i->toList();
		for(int j = 0; j < groupData.size(); ++j)
		{
			QRegularExpression rx("([0-9]+): (.+ [0-9.]+–[0-9.]+ .+)"); // find out what matchday the score is from
			QRegularExpressionMatch match = rx.match(groupData[j].toString()); if(match.hasMatch() && match.captured(1).toDouble() >= matchday)
				groupData[j] = ""; // …blank the value
		}
		// actually delete the blanked values
		QList<QVariant>::iterator newEnd = remove(groupData.begin(), groupData.end(), "");
		groupData.erase(newEnd, groupData.end());
		*i = groupData;
	}
	resumeOpt["tableData"] = tableData;
	return resumeOpt;
}

void XkorRoundRobinCompetition::scorinateMatchday(unsigned int matchday)
{
	QHash<QString, QVariant> localParadigmOptions = paradigmOpt;
	localParadigmOptions.insert("nameWidth", nameWidth());

	XkorAbstractParadigm * p = XkorParadigmFactory::newParadigm(&sport, localParadigmOptions);

	QList<std::pair<unsigned int, unsigned int> > fixtures = generateFixtures(matchday, largestGroupSize(startList));
	
	QList<QVariant> tableData = resumeOpt.value("tableData").toList();

	// make sure we have a place to insert matches to
	while((unsigned int)(tableData.size()) < startList.groups.size())
		tableData.append(QVariant(QList<QVariant>()));

	unsigned int groupNo = 0;
	for(std::vector<XkorStartListGroup>::iterator i = startList.groups.begin(); i != startList.groups.end(); ++i)
	{
		QList<XkorAthlete> athletes;
		QList<QVariant> groupData = tableData[groupNo].toList();
		unsigned int groupSize = i->athletes.size();
		
		for(QList<std::pair<unsigned int, unsigned int> >::iterator j = fixtures.begin(); j != fixtures.end(); ++j)
			if(j->first < groupSize && j->second < groupSize)
			{
				athletes.append(i->athletes[j->first]);
				athletes.append(i->athletes[j->second]);
			}
		
		p->scorinate(athletes);

		if(userOpt.value("allowDraws").toString() == "false")
		{
			// if there’s a tie, deal with it
			QVector<XkorResult> results = p->results();
			for(int j = 0; j < results.size() - (results.size() % 2); j += 2)
			{
				XkorResult score1 = results[j];
				XkorResult score2 = results[j + 1];
				if(p->compare(score1, score2) == 0)
				{
					QList<XkorAthlete> tiebreakAthletes;
					tiebreakAthletes << score1.athlete << score2.athlete;
					p->breakTie(tiebreakAthletes);
				}
			}
		}

		resultsBuf[matchday] += i->name + "\n" + p->output() + "\n";
		
		// assemble teh tablez
		QList<QVariant> tiebreakers = p->option("tiebreakers").toList();
		QList<QVariant> tiebreakerNames = p->option("tiebreakerNames").toList();
		for(QList<std::pair<unsigned int, unsigned int> >::iterator j = fixtures.begin(); j != fixtures.end(); ++j)
			if(j->first < groupSize && j->second < groupSize)
			{
				XkorResult score1 = *(p->findResult(i->athletes[j->first].id));
				XkorResult score2 = *(p->findResult(i->athletes[j->second].id));

				// find the score we want, from the last tiebreaker
				double scoreValue1 = score1.score(), scoreValue2 = score2.score();
				qDebug() << scoreValue1 << scoreValue2;
				if(userOpt.value("allowDraws").toString() == "false")
				{
					QList<QVariant>::iterator currentTiebreaker = tiebreakers.begin();
					QStringList usedTiebreakerNames; // if we put extraTime + goldenGoal under the same “OT” name, we don’t want to add it twice
					for(QList<QVariant>::iterator k = tiebreakerNames.begin(); k != tiebreakerNames.end(); ++k)
					{
						// shootout scores don’t belong in tables
						if(currentTiebreaker->toString() != "shootout" && find(usedTiebreakerNames.begin(), usedTiebreakerNames.end(), k->toString()) == usedTiebreakerNames.end() && (score1.contains(k->toString()) || score2.contains(k->toString())))
						{
							scoreValue1 += score1.value(k->toString()).toDouble();
							scoreValue2 += score2.value(k->toString()).toDouble();
							qDebug() << "adding" << currentTiebreaker->toString() << score1.value(k->toString()).toDouble() << score2.value(k->toString()).toDouble();
							usedTiebreakerNames << k->toString();
						}
						++currentTiebreaker;
					}
				}
				tables[groupNo].insertMatch(i->athletes[j->first].name, i->athletes[j->second].name, scoreValue1, scoreValue2);

				// insert into the resume options
				groupData.append(QString::number(matchday) + ": " + i->athletes[j->first].name + " " + QString::number(scoreValue1) + "–" + QString::number(scoreValue2) + " " + i->athletes[j->second].name);
			}
		
		tables[groupNo].generate();
		tableData[groupNo] = groupData;
		resultsBuf[matchday] += tables[groupNo].toText();
		resultsBuf[matchday] += "\n";
		
		++groupNo;
	}
	resumeOpt["tableData"] = tableData;

	delete p;
}

std::vector<QString> XkorRoundRobinCompetition::tableSortCriteria()
{
	std::vector<QString> rval;
	
	if(userOpt.contains("sortCriteria"))
	{
		QList<QVariant> criteria = userOpt.value("sortCriteria").toList();
		for(QList<QVariant>::iterator i = criteria.begin(); i != criteria.end(); ++i)
			rval.push_back(i->toString());
	}
	else
		rval = XkorSortCriteriaWidget::defaultSortCriteria();
	return rval;
}
