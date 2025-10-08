#include "competitions/matchescompetition.h"


#include "competitions/options/matchescompetitionoptions.h"
#include "paradigms/paradigmfactory.h"

XkorAbstractOptionsWidget * XkorMatchesCompetition::newOptionsWidget(QHash<QString, QVariant> competitionOptions)
{
	return new XkorMatchesCompetitionOptions(competitionOptions);
}

void XkorMatchesCompetition::scorinate(int matchday)
{
	QHash<QString, QVariant> localParadigmOptions = paradigmOpt;
	localParadigmOptions.insert("nameWidth", nameWidth());

	XkorAbstractParadigm * p = XkorParadigmFactory::newParadigm(&sport, localParadigmOptions);

	resultsBuf[matchday].clear();
	for(std::vector<XkorStartListGroup>::iterator i = startList.groups.begin(); i != startList.groups.end(); ++i)
	{
		resultsBuf[matchday] += i->name + "\n";
		p->scorinate(i->athletes);

		if(userOpt.value("allowDraws").toString() == "false")
		{
			// if there’s a tie, deal with it
			QVector<XkorResult> results = p->results();
			for(int j = 0; j < i->athletes.size() - (i->athletes.size() % 2); j += 2)
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

		resultsBuf[matchday] += p->output();
		resultsBuf[matchday] += "\n";
	}
	delete p;
}
