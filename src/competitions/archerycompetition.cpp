#include "competitions/archerycompetition.h"


#include "paradigms/paradigmfactory.h"

void XkorArcheryCompetition::scorinate(int matchday)
{
	QHash<QString, QVariant> localParadigmOptions = paradigmOpt;
	localParadigmOptions.insert("rankingRound", "true");
	localParadigmOptions.insert("nameWidth", nameWidth());

	XkorAbstractParadigm * p = XkorParadigmFactory::newParadigm(&sport, localParadigmOptions);

	resultsBuf[matchday].clear();
	XkorStartListGroup sl = startList.groups[0];
	p->scorinate(sl.athletes);

	// if there’s a tie, deal with it
	QVector<XkorResult> results = p->results();
	bool needsTiebreak = false;
	int i = 0;
	do
	{
		needsTiebreak = false;
		results = p->results();
		p->comparisonFunction()->sort(results);
		QList<XkorAthlete> lotsTiebreakAthletes, realTiebreakAthletes;
		XkorResult tieResult;

		int position = 0;
		for(QVector<XkorResult>::iterator j = results.begin(); j + 1 != results.end(); )
		{
			QVector<XkorResult>::iterator prev = j++; // make prev be what i was
			if(p->compare(*prev, *j) == 0)
			{
				if(position == userOpt.value("cutoff").toInt() - 1)
					tieResult = *j;
				else if(lotsTiebreakAthletes.contains(prev->athlete))
					lotsTiebreakAthletes << j->athlete;
				else
					lotsTiebreakAthletes << prev->athlete << j->athlete;
				needsTiebreak = true;
			}
			++position;
		}

		for(QVector<XkorResult>::iterator j = results.begin(); j != results.end(); ++j)
			if(p->compare(tieResult, *j) == 0)
			{
				realTiebreakAthletes << j->athlete;
				if(lotsTiebreakAthletes.contains(j->athlete))
					lotsTiebreakAthletes.removeAt(lotsTiebreakAthletes.indexOf(j->athlete));
			}

		p->breakTie(lotsTiebreakAthletes, "lots");
		if(i < 3)
			p->breakTie(realTiebreakAthletes);
		else
			p->breakTie(realTiebreakAthletes, "closest"); // closest arrow to the center
		++i;
	}
	while(needsTiebreak);

	resultsBuf[matchday] = rankedListOutput(sl.name, results, p->comparisonFunction());
	delete p;
}
