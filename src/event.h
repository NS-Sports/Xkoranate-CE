#ifndef EVENT_H
#define EVENT_H

#include <QtCore>

#include "group.h"
#include "rplist.h"
#include "signuplist.h"
#include "startlist.h"
#include "xml/xmlindex.h"

class XkorEvent
{

	public:
		XkorEvent() {;;;}

		QString competition() const { return m_competition; }
		QHash<QString, QVariant> competitionOptions() const { return m_competitionOptions; }
		QList<XkorGroup> groups() const { return m_groups; }
		XkorStartList makeStartList(XkorRPList * rpList);
		QString name() const { return m_name; }
		QString paradigm() const { return m_paradigm; }
		QHash<QString, QVariant> paradigmOptions() const { return m_paradigmOptions; }
		QHash<int, QString> results() const { return m_results; }
		XkorSignupList signupList() const { return m_signupList; }
		QString sport() const { return m_sport; }

		void addGroup(XkorGroup group) { m_groups.append(group); }
		void replaceCompetitionOptions(QHash<QString, QVariant> competitionOptions);
		void setCompetition(QString competition) { m_competition = competition; }
		void setCompetitionOptions(QHash<QString, QVariant> competitionOptions) { m_competitionOptions = competitionOptions; }
		void setGroups(QList<XkorGroup> groups) { m_groups = groups; }
		void setName(QString name) { m_name = name; }
		void setParadigmOptions(QHash<QString, QVariant> paradigmOptions) { m_paradigmOptions = paradigmOptions; }
		void setResult(int matchday, QString result);
		void setResults(QHash<int, QString> results) { m_results = results; }
		void setSignupList(XkorSignupList signupList) { m_signupList = signupList; }
		void setSport(QString sport, QString newParadigm = QString()) noexcept(false);
	
	private:
		QString m_competition;
		QHash<QString, QVariant> m_competitionOptions;
		QString m_name;
		QList<XkorGroup> m_groups;
		QString m_paradigm;
		QHash<QString, QVariant> m_paradigmOptions;
		QHash<int, QString> m_results;
		XkorSignupList m_signupList;
		QString m_sport;
		
};

#endif
