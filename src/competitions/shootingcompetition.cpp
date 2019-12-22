#include "shootingcompetition.h"

#include "paradigms/paradigmfactory.h"

void XkorShootingCompetition::scorinate(int matchday)
{
	QHash<QString, QVariant> localParadigmOptions = paradigmOpt;
	localParadigmOptions.insert("nameWidth", nameWidth());
	if(matchday == 0)
		localParadigmOptions.insert("qualifying", "true");

	XkorAbstractParadigm * p = XkorParadigmFactory::newParadigm(&sport, localParadigmOptions);

	resultsBuf[matchday].clear();
	XkorStartListGroup sl = startList.groups[0];

	// figure out who we’re scorinating
	QList<XkorAthlete> athleteList;
	QList<XkorResult> previousResults;
	if(matchday == 1)
	{
		QList<QVariant> qualifiers = userOpt.value("qualifiers").toList();
		QList<QVariant> qualifyingScores = userOpt.value("scores").toList();
		for(int i = 0; i < qualifiers.size() && i < qualifyingScores.size(); ++i)
		{
			XkorAthlete ath;
			for(QList<XkorAthlete>::iterator j = sl.athletes.begin(); j != sl.athletes.end(); ++j)
				if(QUuid(qualifiers[i].toString()) == j->id)
					ath = *j;
			previousResults << XkorResult(qualifyingScores[i].toDouble(), ath);
			athleteList << ath;
		}
	}
	else
		athleteList = sl.athletes;

	p->scorinate(athleteList, previousResults);

	QVector<XkorResult> results = p->results();

	// if there’s a tie, deal with it
	bool needsTiebreak = false;
	do
	{
		needsTiebreak = false;
		results = p->results();
		p->comparisonFunction()->sort(results);

		QList<XkorAthlete> tiebreakAthletes;
		QList<XkorResult> tiebreakResults;
		if(matchday == 0) // qualifying
		{
			int cutoff = userOpt.value("cutoff").toInt();
			if(cutoff > 0 && cutoff < results.size() && p->compare(results[cutoff - 1], results[cutoff]) == 0)
				tiebreakResults << results[cutoff];
		}
		else // final
			for(int i = 1; i <= 3 && i < results.size(); ++i)
				if(p->compare(results[i - 1], results[i]) == 0)
					tiebreakResults << results[i];

		if(!tiebreakResults.empty())
		{
			needsTiebreak = true;
			for(QVector<XkorResult>::iterator i = results.begin(); i != results.end(); ++i)
				for(QList<XkorResult>::iterator j = tiebreakResults.begin(); j != tiebreakResults.end(); ++j)
					if(p->compare(*i, *j) == 0)
						tiebreakAthletes << i->athlete;
		}

		p->breakTie(tiebreakAthletes);
	}
	while(needsTiebreak);

	// save the qualifiers
	if(matchday == 0)
	{
		int cutoff = (userOpt.value("cutoff").toInt() == 0 ? results.size() : userOpt.value("cutoff").toInt());
		QList<QVariant> qualifiedAthletes;
		QList<QVariant> qualifyingScores;
		for(int i = 0; i < cutoff; ++i)
		{
			qualifiedAthletes << results[i].athlete.id.toString();
			qualifyingScores << results[i].score();
		}
		resumeOpt["qualifiers"] = qualifiedAthletes;
		resumeOpt["scores"] = qualifyingScores;
	}

	resultsBuf[matchday] = rankedListOutput(sl.name, results, p->comparisonFunction());
	delete p;
}
