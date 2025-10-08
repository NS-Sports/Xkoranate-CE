#include "event.h"


#include <QtDebug>
#include "signuplist.h"
#include "xml/xmlsportreader.h"

XkorStartList XkorEvent::makeStartList(XkorRPList * rpList)
{
	XkorStartList rval;
	XkorSignupList ubersignuplist;
	
	// make signup lists for each file and dump athletes into the übersignuplist
	QList<XkorAthlete> a = m_signupList.athletes();
	for(QList<XkorAthlete>::iterator i = a.begin(); i != a.end(); ++i)
	{
		XkorAthlete ath = *i;
		ath.skill = m_signupList.adjustRank(ath.skill);
		ubersignuplist.addAthlete(ath);
	}
	
	// now create the start list
	double maxRPSkill = 0;
	for(QList<XkorGroup>::iterator i = m_groups.begin(); i != m_groups.end(); ++i)
	{
		XkorStartListGroup currentGroup;
		for(QList<QUuid>::iterator j = i->athletes.begin(); j != i->athletes.end(); ++j)
		{
			try
			{
				XkorAthlete a = ubersignuplist.getAthleteByID(*j);
				if(rpList)
				{
					if(rpList->useTeams())
						a.rpBonus = rpList->bonus(a.nation);
					else
						a.rpBonus = rpList->bonus(a.name);
					if(rpList->useWGStyleBonus())
					{
						a.rpSkill = a.skill + a.rpBonus * (100 / (100 - rpList->rpEffect()) - 1);
						if(a.rpSkill > maxRPSkill)
							maxRPSkill = a.rpSkill;
					}
					else
						a.rpSkill = a.rpBonus * (rpList->rpEffect() / 100) + a.skill * (1 - (rpList->rpEffect() / 100));
				}
				else
					a.rpSkill = a.skill;
				currentGroup.athletes.push_back(a);
			}
			catch(XkorSearchFailedException) {}
		}
		currentGroup.name = i->name;
		rval.groups.push_back(currentGroup);
	}
	if(rpList->useWGStyleBonus() && maxRPSkill > 1) // now scale back the bonuses if any are too high in WG style
		for(std::vector<XkorStartListGroup>::iterator i = rval.groups.begin(); i != rval.groups.end(); ++i)
			for(QList<XkorAthlete>::iterator j = i->athletes.begin(); j != i->athletes.end(); ++j)
				j->rpSkill /= maxRPSkill;
	
	// copy over settings from the event
	rval.name = m_name;
	
	return rval;
}

void XkorEvent::replaceCompetitionOptions(QHash<QString, QVariant> competitionOptions)
{
	// clear the old values
	for(QHash<QString, QVariant>::iterator i = competitionOptions.begin(); i != competitionOptions.end(); ++i)
		m_competitionOptions.remove(i.key());
	// set the new values
	for(QHash<QString, QVariant>::iterator i = competitionOptions.begin(); i != competitionOptions.end(); ++i)
		m_competitionOptions.insert(i.key(), i.value());
}

void XkorEvent::setResult(int matchday, QString result)
{
	m_results[matchday] = result;

	// if we’ve backtracked, we want to make sure we don’t have any residual “future” results
	for(QHash<int, QString>::iterator i = m_results.begin(); i != m_results.end(); ++i)
		if(i.key() > matchday)
			i.value() = "";
}

void XkorEvent::setSport(QString sport, QString newParadigm) noexcept(false)
{
	m_sport = sport;
	m_paradigm = newParadigm;
}
