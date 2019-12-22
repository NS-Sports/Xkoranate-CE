#ifndef ROUNDROBINCOMPETITION_H
#define ROUNDROBINCOMPETITION_H

#include "competitions/abstractcompetition.h"
#include "paradigms/abstractparadigm.h"
#include "tablegenerator/table.h"

class XkorRoundRobinCompetition : public XkorAbstractCompetition
{
	public:
		bool hasOptionsWidget() { return true; }
		
		virtual int matchdays();
		virtual QStringList matchdayNames();
		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> competitionOptions);
		void scorinate(int matchday);
	private:
		QList<XkorTable> tables;
		
		QList<std::pair<unsigned int, unsigned int> > generateFixtures(unsigned int matchday, unsigned int groupSize);
		std::vector<XkorTableColumn> generateTableColumns(QString groupName);
		unsigned int largestGroupSize(XkorStartList & sl);
		std::vector<XkorTableMatch> retrieveMatches(unsigned int groupNo);
		QHash<QString, QVariant> revertToMatchday(int matchday); // “matchday” is the first that will be erased
		void scorinateMatchday(unsigned int matchday);
		std::vector<QString> tableSortCriteria();
};

#endif
