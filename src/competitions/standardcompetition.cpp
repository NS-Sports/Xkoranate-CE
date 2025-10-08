#include "competitions/standardcompetition.h"


#include "competitions/options/massstartcompetitionoptions.h"
#include "paradigms/paradigmfactory.h"

XkorAbstractOptionsWidget * XkorMassStartCompetition::newOptionsWidget(QHash<QString, QVariant> competitionOptions)
{
	return new XkorMassStartCompetitionOptions(competitionOptions);
}

void XkorMassStartCompetition::scorinate(int matchday)
{
	QHash<QString, QVariant> localParadigmOptions = paradigmOpt;
	localParadigmOptions.insert("nameWidth", nameWidth());

	XkorAbstractParadigm * p = XkorParadigmFactory::newParadigm(&sport, localParadigmOptions);

	QVector<XkorResult> allResults;
	resultsBuf[matchday].clear();
	for(unsigned int i = 0; i < startList.groups.size(); ++i)
	{
		p->scorinate(startList.groups[i].athletes);
		QVector<XkorResult> groupResults = p->results();
		for(QVector<XkorResult>::iterator j = groupResults.begin(); j != groupResults.end(); ++j)
			j->result["heat"] = i; // set the heat number so that the sorting works out correctly
		if(userOpt.value("sortResults", "true") == "true")
		{
			p->comparisonFunction()->sort(groupResults);
			resultsBuf[matchday] += rankedListOutput(startList.groups[i].name, groupResults, p->comparisonFunction());
		}
		else
		{
			resultsBuf[matchday] += startList.groups[i].name + "\n";
			resultsBuf[matchday] += p->output() + "\n\n";
		}
		allResults << groupResults;
	}

	// do the complete ranking
	if(userOpt.value("sortResults", "true") == "true" && userOpt.value("overallRanking", "false") == "true")
	{
		p->comparisonFunction()->sort(allResults);
		resultsBuf[matchday] += rankedListOutput("Overall ranking", allResults, p->comparisonFunction());
	}

	delete p;
}
