#ifndef ABSTRACTPARADIGM_H
#define ABSTRACTPARADIGM_H

#include <algorithm>
#include <cmath>
#include <QHash>
#include <QLayout>
#include <QStringList>
#include <QtDebug>

#include "result.h"
#include "startlist.h"
#include "abstractoptionswidget.h"
#include "paradigms/comparators/basicresultcomparator.h"
#include "signuplisteditor/athletewidget.h"
#include "tablegenerator/table.h"

class XkorAbstractParadigm : public QObject
{
	Q_OBJECT
	public:
		XkorAbstractParadigm()
		{
			s = 0;
		}
		
		XkorAbstractParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions)
		{
			s = 0;
			init(sport, userOptions);
		}

		virtual void addResults(QList<XkorResult> previousResults)
		{
			for(QList<XkorResult>::iterator i = previousResults.begin(); i != previousResults.end(); ++i)
			{
				XkorResult r = *i;
				r.setOutput(outputLine(r));
				res.append(r);
			}
		}
		
		virtual void breakTie(QList<XkorAthlete>, QString = QString())
		{
			return; // tell whoever’s requesting a tiebreaker to stuff it
		}

		virtual int compare(XkorResult a, XkorResult b, QString type = QString())
		{
			XkorAbstractResultComparator * f = comparisonFunction(type);
			if((*f)(a, b)) // if(a < b)
				return -1;
			else if((*f)(b, a)) // if(b < a)
				return 1;
			else
				return 0;
		}

		virtual XkorAbstractResultComparator * comparisonFunction(QString type = QString())
		{
			return new XkorBasicResultComparator(type, opt);
		}

		virtual QString defaultCompetition()
		{
			return "standard";
		}

		virtual QVector<XkorResult>::iterator findResult(QUuid id)
		{
			for(QVector<XkorResult>::iterator i = res.begin(); i != res.end(); ++i)
				if(i->athlete.id == id)
					return i;
			return res.end();
		}

		virtual bool hasOptionsWidget() { return false; }
		
		void init(XkorSport * sport, QHash<QString, QVariant> userOptions)
		{
			s = sport;
			userOpt = userOptions;
			opt = s->paradigmOptions();
		}

		virtual XkorAbstractAthleteWidget * newAthleteWidget()
		{
			return new XkorAthleteWidget(QStringList() << "name" << "nation" << "skill",
						     QStringList() << tr("Participant") << tr("Team") << tr("Skill"),
						     QStringList() << "string" << "string" << "skill");
		}

		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant>) { return 0; }

		virtual QString output()
		{
			QString rval;
			for(QList<QPair<QString, QString> >::iterator i = out.begin(); i != out.end(); ++i)
				rval += i->second + "\n";
			return rval;
		}

		virtual QVariant option(QString key)
		{
			return opt.value(key);
		}
		
		virtual QVector<XkorResult> results() { return res; }
		
		virtual void scorinate(QList<XkorAthlete> athletes, QList<XkorResult> previousResults = QList<XkorResult>()) = 0;

		virtual bool supportsCompetition(QString competition)
		{
			return supportedCompetitions[competition];
		}
		
		virtual ~XkorAbstractParadigm() {;;;}
	
	protected:
		XkorSport * s;
		QHash<QString, bool> supportedCompetitions;
		QHash<QString, QVariant> opt, userOpt;
		QStringList requiredValues;
		QVector<XkorResult> res;
		QList<QPair<QString, QString> > out;
		
		virtual XkorResult individualResult(XkorAthlete athlete, QString) { return XkorResult(0, athlete); }
		
		// helper functions for subclasses
		virtual QString formatName(QString name, QString nation)
		{
			if(userOpt.value("showTLAs", "true").toString() == "true" && !nation.isEmpty())
				return name + " (" + nation + ")";
			else
				return name;
		}

		virtual QString formatName(XkorAthlete ath)
		{
			return formatName(ath.name, ath.nation);
		}

		virtual void generateOutput()
		{
			out.clear();
			for(QVector<XkorResult>::iterator i = res.begin(); i != res.end(); ++i)
				if(i->output() != QString())
					out.append(qMakePair(i->athlete.name, i->output()));
		}

		virtual QString outputLine(XkorResult r)
		{
			int nameWidth = userOpt.value("nameWidth", 20).toInt() + 2;
			int resultWidth = opt.value("resultWidth", 10).toInt() + 2;

			QString rval = formatName(r.athlete.name, r.athlete.nation).leftJustified(nameWidth);
			rval += r.scoreString().rightJustified(resultWidth);

			return rval;
		}

		QList<QVariant> readOptionList(QString name)
		{
			if(opt.contains(name))
			{
				QVariant val = opt.value(name);
				if(val.type() == QVariant::List)
					return val.toList();
				else
					return QList<QVariant>() << val; // create a single-item list
			}
			else // this is important, we don’t want to trick the paradigm into thinking there’s a value when there isn’t one
				return QList<QVariant>();
		}
		
		virtual QString timeFormat(double n, uint displayDigits)
		{
			QString rval;

			if(n > 3600) // hours
			{
				rval += QString::number((int)(n / 3600));
				rval += ":";
				if((int)n % 3600 < 600) // need a leading zero for minutes
					rval += "0";
			}
			if(n > 60) // minutes
			{
				rval += QString::number((int)(n / 60) % 60);
				rval += ":";
				if((int)n % 60 < 10) // need a leading zero for seconds
					rval += "0";
			}
			rval += QString::number(n - (int)(n / 60) * 60, 'f', displayDigits);
			return rval;
		}
};

#endif
