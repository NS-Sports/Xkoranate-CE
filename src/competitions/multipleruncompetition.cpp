#include "multipleruncompetition.h"

#include "paradigms/paradigmfactory.h"

QStringList XkorMultipleRunCompetition::matchdayNames()
{
	QStringList rval;
	for(int i = 1; i <= userOpt.value("runs", 2).toInt(); ++i)
		rval << QString("Run %1").arg(i);
	return rval;
}

QHash<QString, QVariant> XkorMultipleRunCompetition::revertToMatchday(int matchday)
{
	QList<QVariant> athletes, scores, scoreStrings, disqualifiedAthletes, disqualifiedScores, disqualifiedScoreStrings, disqualifiedOutputs;

	QList<QVariant> currentAthletes = userOpt.value("athletes").toList();
	QList<QVariant> currentScores = userOpt.value("scores").toList();
	QList<QVariant> currentScoreStrings = userOpt.value("scoreStrings").toList();
	for(int i = 0; i < currentAthletes.size(); ++i)
	{
		athletes << currentAthletes[i];
		if(currentScores[i].toList().size() >= matchday)
		{
			QList<QVariant> truncatedScores, truncatedScoreStrings;
			for(int j = 0; j < matchday; ++j)
			{
				truncatedScores << currentScores[i].toList()[j];
				truncatedScoreStrings << currentScoreStrings[i].toList()[j];
			}
			scores << QVariant(truncatedScores);
			scoreStrings << QVariant(truncatedScoreStrings);
		}
		else
		{
			scores << currentScores[i];
			scoreStrings << currentScoreStrings[i];
		}
	}

	QList<QVariant> currentDQAthletes = userOpt.value("disqualifiedAthletes").toList();
	QList<QVariant> currentDQScores = userOpt.value("disqualifiedScores").toList();
	QList<QVariant> currentDQScoreStrings = userOpt.value("disqualifiedScoreStrings").toList();
	QList<QVariant> currentDQOutputs = userOpt.value("disqualifiedOutputs").toList();
	for(int i = 0; i < currentDQAthletes.size(); ++i)
	{
		if(currentDQScores[i].toList().size() > matchday)
		{
			// return them to active duty
			athletes << currentDQAthletes[i];
			QList<QVariant> truncatedScores, truncatedScoreStrings;
			for(int j = 0; j < matchday; ++j)
			{
				truncatedScores << currentDQScores[i].toList()[j];
				truncatedScoreStrings << currentDQScoreStrings[i].toList()[j];
			}
			scores << QVariant(truncatedScores);
			scoreStrings << QVariant(truncatedScoreStrings);
		}
		else
		{
			disqualifiedAthletes << currentDQAthletes[i];
			disqualifiedScores << currentDQScores[i];
			disqualifiedScoreStrings << currentDQScoreStrings[i];
			disqualifiedOutputs << currentDQOutputs[i];
		}
	}

	resumeOpt["athletes"] = athletes;
	resumeOpt["scores"] = scores;
	resumeOpt["scoreStrings"] = scoreStrings;
	resumeOpt["disqualifiedAthletes"] = disqualifiedAthletes;
	resumeOpt["disqualifiedScores"] = disqualifiedScores;
	resumeOpt["disqualifiedScoreStrings"] = disqualifiedScoreStrings;
	resumeOpt["disqualifiedOutputs"] = disqualifiedOutputs;
	return resumeOpt;
}

void XkorMultipleRunCompetition::scorinate(int matchday)
{
	QHash<QString, QVariant> localParadigmOptions = paradigmOpt;
	localParadigmOptions.insert("runNumber", matchday); // pass the run number to the paradigm for results formatting and such
	localParadigmOptions.insert("nameWidth", nameWidth());

	XkorAbstractParadigm * p = XkorParadigmFactory::newParadigm(&sport, localParadigmOptions);

	resultsBuf[matchday].clear();
	XkorStartListGroup sl = startList.groups[0];

	// figure out who we’re scorinating
	QList<XkorAthlete> athleteList;
	QList<XkorResult> previousResults;
	if(matchday > 0)
	{
		QList<QVariant> athletes = userOpt.value("athletes").toList();
		QList<QVariant> scores = userOpt.value("scores").toList();
		QList<QVariant> scoreStrings = userOpt.value("scoreStrings").toList();
		for(int i = 0; i < athletes.size(); ++i)
		{
			XkorAthlete ath;
			for(QList<XkorAthlete>::iterator j = sl.athletes.begin(); j != sl.athletes.end(); ++j)
				if(QUuid(athletes[i].toString()) == j->id)
					ath = *j;

			XkorResult r;
			r.athlete = ath;
			QList<QVariant> attempts, attemptStrings;
			attempts.append(scores[i].toList());
			attemptStrings.append(scoreStrings[i].toList());
			r.result["attempts"] = attempts;
			r.result["attemptStrings"] = attemptStrings;

			athleteList << ath;
			previousResults << r;
		}
	}
	else
		athleteList = sl.athletes;

	p->scorinate(athleteList, previousResults);

	// remove the totals for new disqualified athletes
	//if(matchday > 0)
	//	for(QVector<XkorResult>::iterator i = results.begin(); i != results.end(); ++i)
	//		if(i->score() == numeric_limits<double>::min() || i->score() == numeric_limits<double>::max())
	//			i->setOutput(i->output().left(i->output().size() - paradigmOpt.value("resultWidth", 10).toInt()));

	// insert disqualified athletes
	QList<XkorResult> disqualifiedResults;
	QList<QVariant> disqualifiedAthletes = userOpt.value("disqualifiedAthletes").toList();
	QList<QVariant> disqualifiedScores = userOpt.value("disqualifiedScores").toList();
	QList<QVariant> disqualifiedScoreStrings = userOpt.value("disqualifiedScoreStrings").toList();
	QList<QVariant> disqualifiedOutputs = userOpt.value("disqualifiedOutputs").toList();
	for(int i = 0; i < disqualifiedAthletes.size(); ++i)
	{
		XkorAthlete ath;
		for(QList<XkorAthlete>::iterator j = sl.athletes.begin(); j != sl.athletes.end(); ++j)
			if(QUuid(disqualifiedAthletes[i].toString()) == j->id)
				ath = *j;

		XkorResult r;
		r.athlete = ath;
		QList<QVariant> attempts, attemptStrings;
		attempts.append(disqualifiedScores[i].toList());
		attemptStrings.append(disqualifiedScoreStrings[i].toList());
		r.result["attempts"] = attempts;
		r.result["attemptStrings"] = attemptStrings;
		r.setScore(attempts.at(attempts.size() - 1).toDouble());
		r.setScoreString(attemptStrings.at(attempts.size() - 1).toString());
		r.setOutput(disqualifiedOutputs[i].toString());
		disqualifiedResults << r;
	}

	p->addResults(disqualifiedResults);
	QVector<XkorResult> results = p->results();

	if(userOpt.value("sortResults", "true") == "true")
	{
		if(userOpt.value("sortByBestResult", "true") == "true")
			p->comparisonFunction()->sort(results);
		else
			p->comparisonFunction("noTiebreakers")->sort(results);
	}

	// save the qualifiers
	QList<QVariant> qualifiedAthletes, qualifyingScores, qualifyingScoreStrings;
	disqualifiedAthletes.clear();
	disqualifiedScores.clear();
	disqualifiedScoreStrings.clear();
	disqualifiedOutputs.clear();
	for(int i = 0; i < results.size(); ++i)
	{
		if(results[i].score() != numeric_limits<double>::max() && results[i].score() != numeric_limits<double>::min())
		{
			qualifiedAthletes << results[i].athlete.id.toString();
			qualifyingScores << QVariant(results[i].result["attempts"]);
			qualifyingScoreStrings << QVariant(results[i].result["attemptStrings"]);
		}
		else
		{
			disqualifiedAthletes << results[i].athlete.id.toString();
			disqualifiedScores << QVariant(results[i].result["attempts"]);
			disqualifiedScoreStrings << QVariant(results[i].result["attemptStrings"]);
			disqualifiedOutputs << results[i].output();
		}
	}
	resumeOpt["athletes"] = qualifiedAthletes;
	resumeOpt["scores"] = qualifyingScores;
	resumeOpt["scoreStrings"] = qualifyingScoreStrings;
	resumeOpt["disqualifiedAthletes"] = disqualifiedAthletes;
	resumeOpt["disqualifiedScores"] = disqualifiedScores;
	resumeOpt["disqualifiedScoreStrings"] = disqualifiedScoreStrings;
	resumeOpt["disqualifiedOutputs"] = disqualifiedOutputs;

	if(userOpt.value("sortResults", "true") == "true")
	{
		if(userOpt.value("sortByBestResult", "true") == "true")
			resultsBuf[matchday] = rankedListOutput(sl.name, results, p->comparisonFunction());
		else
			resultsBuf[matchday] = rankedListOutput(sl.name, results, p->comparisonFunction("noTiebreakers"));
	}
	else
	{
		resultsBuf[matchday] += sl.name + "\n";
		resultsBuf[matchday] += p->output() + "\n\n";
	}
	delete p;
}
