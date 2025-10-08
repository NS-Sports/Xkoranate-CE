#ifndef ORDINALPARADIGM_H
#define ORDINALPARADIGM_H

#include "paradigms/abstractparadigm.h"
#include "paradigms/options/timedparadigmoptions.h"

class XkorOrdinalParadigm : public XkorAbstractParadigm
{
	public:
		XkorOrdinalParadigm() : XkorAbstractParadigm()
		{
			supportedCompetitions.insert("standard", true);
		}

		XkorOrdinalParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractParadigm(sport, userOptions)
		{
			supportedCompetitions.insert("standard", true);
		}

		virtual void breakTie(QList<XkorAthlete>)
		{
			qDebug() << "unexpected tie break request in ordinal paradigm";
		}

		virtual XkorAbstractResultComparator * comparisonFunction(QString type = QString())
		{
			QHash<QString, QVariant> comparisonOpt = opt;
			comparisonOpt.insert("sortOrder", "descending");
			return new XkorBasicResultComparator(type, comparisonOpt);
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
					qDebug() << "missing parameter" << *i << "in XkorOrdinalParadigm::output(XkorResults *)";
					out.append(qMakePair(QString(""), QString("Sport does not support this paradigm")));
					return;
				}

			// initialize results
			out.clear();
			res.clear();

			QList<QVariant> statusSortOrder = readOptionList("statusSortOrder");
			QList<QVariant> statuses = readOptionList("statuses");

			for(QList<XkorAthlete>::iterator i = athletes.begin(); i != athletes.end(); ++i)
			{
				XkorResult r;
				r.athlete = *i;
				QString status = generateStatus();
				if(status == QString())
					r.setScore(generateScore(i->rpSkill));
				else
				{
					r.setScore(generateScore(i->rpSkill) - statusSortOrder.at(statuses.indexOf(status)).toDouble());
					r.setScoreString(status);
				}
				res.append(r);
			}

			comparisonFunction()->sort(res);

			int nameWidth = userOpt.value("nameWidth", 20).toInt() + 2;
			int resultWidth = opt.value("resultWidth", 3).toInt() + 2;

			int acc = 1;
			for(QVector<XkorResult>::iterator i = res.begin(); i != res.end(); ++i)
			{
				QString output = formatName(i->athlete.name, i->athlete.nation).leftJustified(nameWidth);
				if(i->score() < 0)
					output += i->scoreString().rightJustified(resultWidth);
				else
					output += QString::number(acc).rightJustified(resultWidth);
				i->setOutput(output);
				out.append(qMakePair(i->athlete.name, output));
				++acc;
			}
		}

	protected:

		virtual double generateScore(double skill)
		{
			double attackModifier = opt.value("attackModifier").toDouble();
			return (s->randWeighted(skill) + attackModifier * s->randUniform()) / (1 + attackModifier);
		}

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

#endif // ORDINALPARADIGM_H
