#ifndef POINTSRACEPARADIGM_H
#define POINTSRACEPARADIGM_H

#include "paradigms/abstractparadigm.h"
#include "paradigms/comparators/pointsraceresultcomparator.h"
#include "paradigms/options/timedparadigmoptions.h"

class XkorPointsRaceParadigm : public XkorAbstractParadigm
{
	public:
		XkorPointsRaceParadigm() : XkorAbstractParadigm()
		{
			supportedCompetitions.insert("standard", true);
		}

		XkorPointsRaceParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractParadigm(sport, userOptions)
		{
			supportedCompetitions.insert("standard", true);
		}

		virtual void breakTie(QList<XkorAthlete>)
		{
			qDebug() << "unexpected tie break request in points race paradigm";
		}

		virtual XkorAbstractResultComparator * comparisonFunction(QString type = QString())
		{
			return new XkorPointsRaceResultComparator(type, opt);
		}

		virtual bool hasOptionsWidget() { return true; }

		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> paradigmOptions)
		{
			return new XkorTimedParadigmOptions(paradigmOptions);
		}

		virtual void scorinate(QList<XkorAthlete> athletes, QList<XkorResult> = QList<XkorResult>())
		{
			for(QStringList::iterator i = requiredValues.begin(); i != requiredValues.end(); ++i)
				if(!(s->hasValue(*i)))
				{
					qDebug() << "missing parameter" << *i << "in XkorPointsRaceParadigm::output(XkorResults *)";
					out.append(qMakePair(QString(""), QString("Sport does not support this paradigm")));
					return;
				}

			// initialize results
			out.clear();
			res.clear();

			int maxPoints = opt.value("maxPoints").toInt();
			bool usePoints = (opt.value("usePoints", "true").toString() == "true");

			for(QList<XkorAthlete>::iterator i = athletes.begin(); i != athletes.end(); ++i)
			{
				XkorResult r;
				r.athlete = *i;
				if(usePoints)
					r.result["points"] = s->individualScore("points", i->rpSkill);
				r.result["laps"] = s->individualScore("laps", i->rpSkill);
				r.result["time"] = s->randWeighted(i->rpSkill);
				res.append(r);
			}

			double sum = 0;
			if(usePoints)
				for(QVector<XkorResult>::iterator i = res.begin(); i != res.end(); ++i)
					sum += i->value("points").toDouble();

			int nameWidth = userOpt.value("nameWidth", 20).toInt() + 2;
			int resultWidth = opt.value("resultWidth", 3).toInt() + 2;

			for(QVector<XkorResult>::iterator i = res.begin(); i != res.end(); ++i)
			{
				double curPoints = 0;
				if(usePoints)
					curPoints = max(0.0, floor(i->value("points").toDouble() + (maxPoints - sum) / res.size()));
				double curLaps = floor(i->value("laps").toDouble() + 0.5);

				QString status = generateStatus();
				if(!status.isEmpty())
				{
					i->setScore(-numeric_limits<double>::max());
					i->setScoreString(status.rightJustified(resultWidth));
				}
				else if(usePoints)
				{
					i->setScore(curPoints + curLaps * 20);
					i->setScoreString(QString::number(curPoints).rightJustified(resultWidth) + QString::number(curLaps).rightJustified(resultWidth));
				}
				else
				{
					i->setScore(curLaps);
					i->setScoreString(QString::number(curLaps).rightJustified(resultWidth));
				}
			}

			for(QVector<XkorResult>::iterator i = res.begin(); i != res.end(); ++i)
			{
				QString output = formatName(i->athlete.name, i->athlete.nation).leftJustified(nameWidth);
				output += i->scoreString();
				i->setOutput(output);
				out.append(qMakePair(i->athlete.name, output));
			}
		}

	protected:

		virtual QString generateStatus()
		{
			QList<QVariant> statuses = readOptionList("statuses");
			QList<QVariant> statusProbs = readOptionList("statusProbs");

			double rand = s->randUniform();

			double acc = 0;
			for(int i = 0; i < statuses.size(); ++i)
			{
				acc += statusProbs[i].toDouble();
				if(rand < acc)
					return statuses[i].toString();
			}
			return QString();
		}
};

#endif // POINTSRACEPARADIGM_H
