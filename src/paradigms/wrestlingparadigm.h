#ifndef WRESTLINGPARADIGM_H
#define WRESTLINGPARADIGM_H

#include "paradigms/abstractparadigm.h"
#include "paradigms/options/timedparadigmoptions.h"

struct XkorWrestlingParadigmResult
{
		double homeScore;
		double awayScore;
		QString result;
		QString reversedResult;
		QString annotation;

		XkorWrestlingParadigmResult() {;;;}
		XkorWrestlingParadigmResult(double _homeScore, double _awayScore, QString _result, QString _reversedResult, QString _annotation = QString()) : homeScore(_homeScore), awayScore(_awayScore), result(_result), reversedResult(_reversedResult), annotation(_annotation) {;;;}

		XkorWrestlingParadigmResult reverse() { return XkorWrestlingParadigmResult(awayScore, homeScore, reversedResult, result, annotation); }
};

class XkorWrestlingParadigm : public XkorAbstractParadigm
{
	public:
		XkorWrestlingParadigm() : XkorAbstractParadigm()
		{
			supportedCompetitions.insert("matches", true);
			supportedCompetitions.insert("roundRobin", true);
		}

		XkorWrestlingParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractParadigm(sport, userOptions)
		{
			supportedCompetitions.insert("matches", true);
			supportedCompetitions.insert("roundRobin", true);
		}

		virtual void breakTie(QList<XkorAthlete>)
		{
			qDebug() << "unexpected tie break request in wrestling paradigm";
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
					qDebug() << "missing parameter" << *i << "in XkorSQISParadigm::output(XkorResults *)";
					out.append(qMakePair(QString(""), QString("Sport does not support this paradigm")));
					return;
				}

			// initialize results
			out.clear();
			res.clear();

			int acc = 0;
			for(QList<XkorAthlete>::iterator away = athletes.begin(); away != athletes.end(); ++away)
			{
				if(acc & 1) // if we’re on the second team in a match
				{
					QList<XkorAthlete>::iterator home = away; // home = away - 1
					--home;

					XkorWrestlingParadigmResult score = generateScore(home->rpSkill, away->rpSkill);
					QString result = formatName(*home) + " " + score.result + " " + formatName(*away);
					if(score.annotation != QString())
						result += " (" + score.annotation + ")";

					out.append(qMakePair(home->name, result));
					res.append(XkorResult(score.homeScore, *home));
					res.append(XkorResult(score.awayScore, *away));
				}
				++acc;
			}
		}

	protected:
		virtual QString formatScore(int score1, int score2)
		{
			return QString::number(score1) + "–" + QString::number(score2);
		}

		virtual XkorWrestlingParadigmResult generateScore(double homeSkill, double awaySkill)
		{
			XkorWrestlingParadigmResult rval;

			QList<QPair<XkorWrestlingParadigmResult, double> > probArr;

			QList<QVariant> results = readOptionList("results");
			QList<QVariant> reversedResults = readOptionList("reversedResults");
			QList<QVariant> winnerScores = readOptionList("winnerScores");
			QList<QVariant> loserScores = readOptionList("loserScores");
			QList<QVariant> resultProbs = readOptionList("resultProbs");
			QList<QVariant> annotations = readOptionList("annotations");

			if(results.size() == 0 && reversedResults.size() == 0) // generate these automatically
			{
				for(int i = 0; i < winnerScores.size() && i < loserScores.size(); ++i)
				{
					results.append(winnerScores[i].toString() + "–" + loserScores[i].toString());
					reversedResults.append(loserScores[i].toString() + "–" + winnerScores[i].toString());
				}
			}

			for(int i = 0; i < results.size() && i < reversedResults.size() && i < winnerScores.size() && i < loserScores.size() && i < resultProbs.size(); ++i)
			{
				XkorWrestlingParadigmResult r;
				if(i < annotations.size())
					r = XkorWrestlingParadigmResult(winnerScores[i].toDouble(), loserScores[i].toDouble(), results[i].toString(), reversedResults[i].toString(), annotations[i].toString());
				else
					r = XkorWrestlingParadigmResult(winnerScores[i].toDouble(), loserScores[i].toDouble(), results[i].toString(), reversedResults[i].toString());
				probArr.append(qMakePair(r, resultProbs[i].toDouble()));
			}

			double homeRand = s->randWeighted(homeSkill, awaySkill);
			double awayRand = s->randWeighted(awaySkill, homeSkill);

			double difference;
			if(homeRand - awayRand > -opt.value("mandatoryHomeAdvantage", 0).toDouble())
				difference = s->randWeighted(homeSkill, awaySkill);
			else
				difference = s->randWeighted(awaySkill, homeSkill);

			double acc = 0;
			for(QList<QPair<XkorWrestlingParadigmResult, double> >::iterator i = probArr.begin(); i != probArr.end(); ++i)
			{
				acc += i->second;
				if(difference < acc)
				{
					if(homeRand > awayRand)
						rval = i->first;
					else
						rval = i->first.reverse();
					break;
				}
			}
			return rval;
		}
};

#endif // WRESTLINGPARADIGM_H
