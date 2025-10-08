#ifndef ELIMINATIONRACEPARADIGM_H
#define ELIMINATIONRACEPARADIGM_H

#include <QFormLayout>
#include <QSpinBox>

#include "paradigms/timedparadigm.h"
#include "paradigms/comparators/eliminationraceresultcomparator.h"

struct XkorEliminationRaceParadigm : public XkorTimedParadigm
{
	Q_OBJECT

	public:
		XkorEliminationRaceParadigm() : XkorTimedParadigm()
		{
			supportedCompetitions.clear();
			supportedCompetitions.insert("standard", true);
		}

		XkorEliminationRaceParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorTimedParadigm(sport, userOptions)
		{
			supportedCompetitions.clear();
			supportedCompetitions.insert("standard", true);
		}

	protected:
		void awardPoints(QList<XkorResult> & activeRes, int lap, bool isFinal = false)
		{
			QList<XkorResult>::iterator bestResult, secondBestResult, thirdBestResult;
			bool bestResultFound = false, secondBestResultFound = false, thirdBestResultFound = false;
			for(QList<XkorResult>::iterator j = activeRes.begin(); j != activeRes.end(); ++j)
			{
				if(!bestResultFound || lapTime(*j, lap) < lapTime(*bestResult, lap))
				{
					if(bestResultFound)
					{
						if(secondBestResultFound)
						{
							thirdBestResult = secondBestResult;
							thirdBestResultFound = true;
						}
						secondBestResult = bestResult;
						secondBestResultFound = true;
					}
					bestResult = j;
					bestResultFound = true;
				}
				else if(!secondBestResultFound || lapTime(*j, lap) < lapTime(*secondBestResult, lap))
				{
					if(secondBestResultFound)
					{
						thirdBestResult = secondBestResult;
						thirdBestResultFound = true;
					}
					secondBestResult = j;
					secondBestResultFound = true;
				}
				else if(!thirdBestResultFound || lapTime(*j, lap) < lapTime(*thirdBestResult, lap))
				{
					thirdBestResult = j;
					thirdBestResultFound = true;
				}
			}
			if(bestResultFound)
				bestResult->result["points"] = bestResult->value("points").toInt() + (isFinal ? 3 : 2);
			if(secondBestResultFound)
				secondBestResult->result["points"] = secondBestResult->value("points").toInt() + (isFinal ? 2 : 1);
			if(thirdBestResultFound && isFinal)
				thirdBestResult->result["points"] = thirdBestResult->value("points").toInt() + 1;
		}

		virtual XkorAbstractResultComparator * comparisonFunction(QString type = QString())
		{
			return new XkorEliminationRaceResultComparator(type, opt);
		}

		double lapTime(XkorResult r, int lap)
		{
            double rval = 0;
			QList<QVariant> attempts = r.result["attempts"].toList();
			for(int i = 0; i < lap && i < attempts.size(); ++i)
				rval += attempts[i].toDouble();
			return rval;
		}

		virtual QString outputLine(XkorResult r)
		{
			int nameWidth = userOpt.value("nameWidth", 20).toInt() + 2;
			int resultWidth = opt.value("resultWidth", 9).toInt() + 2;
			bool usePoints = (opt.value("usePoints") == "true");

			QString rval = formatName(r.athlete.name, r.athlete.nation).leftJustified(nameWidth);
			rval += r.scoreString().rightJustified(resultWidth);
			if(usePoints && r.score() != numeric_limits<double>::max())
				rval += r.value("points").toString().rightJustified(7);

			return rval;
		}

		virtual void scorinate(QList<XkorAthlete> athletes, QList<XkorResult> = QList<XkorResult>())
		{
			int laps = opt.value("laps").toInt();
			int eliminations = athletes.size() - opt.value("finishers", 5).toInt();
			int firstElimination = opt.value("firstElimination", 12).toInt();
			int lastElimination = opt.value("lastElimination", 72).toInt();
			int eliminationSlots = (lastElimination - firstElimination) / 2 + 1;
			int eliminationsPerSlot = eliminations / eliminationSlots; // integer division
			int extraEliminations = eliminations - eliminationSlots * eliminationsPerSlot;
			bool usePoints = (opt.value("usePoints") == "true");

			// initialize results
			out.clear();
			res.clear();

			QList<XkorResult> activeRes;
			for(QList<XkorAthlete>::iterator i = athletes.begin(); i != athletes.end(); ++i)
			{
				XkorResult r;
				r.athlete = *i;

				for(int j = 0; j < laps; ++j)
				{
					XkorResult result = individualResult(*i, "score");

					QList<QVariant> attempts = r.result["attempts"].toList(), attemptStrings = r.result["attemptStrings"].toList();
					attempts.append(result.score());
					attemptStrings.append(result.scoreString());
					r.result["attempts"] = attempts;
					r.result["attemptStrings"] = attemptStrings;
				}
				calculateTotal(r);

				activeRes.append(r);
			}

			// award points for any sprints that don’t have corresponding eliminations
			if(usePoints)
				for(int i = 0; i < eliminationSlots - eliminations; ++i)
					awardPoints(activeRes, firstElimination + i * 2);

			int lastPointsSprint = 0;
			for(int i = 0; i < eliminations; ++i)
			{
				int eliminationLap;
				if(eliminationsPerSlot > 0)
				{
					int extraEliminationsPassed = min(extraEliminations, (i + 1) / (eliminationsPerSlot + 1)); // integer division
					eliminationLap = lastElimination - (eliminationSlots - (i - extraEliminationsPassed) / eliminationsPerSlot - 1) * 2;
				}
				else
					eliminationLap = lastElimination - (eliminations - i - 1) * 2;

				// calculate points for previous lap if this is a points race
				if(usePoints && lastPointsSprint < eliminationLap - 1)
					awardPoints(activeRes, eliminationLap - 1);
				lastPointsSprint = eliminationLap - 1;

				QList<XkorResult>::iterator worstResult;
				bool worstResultFound = false;
				for(QList<XkorResult>::iterator j = activeRes.begin(); j != activeRes.end(); ++j)
					if(!worstResultFound || lapTime(*j, eliminationLap) > lapTime(*worstResult, eliminationLap))
					{
						worstResult = j;
						worstResultFound = true;
					}
				XkorResult eliminated = *worstResult;
				eliminated.result["eliminationOrder"] = i + 1;
				eliminated.setScore(numeric_limits<double>::max());
				eliminated.setScoreString(QString("EL lap %1").arg(eliminationLap));
				eliminated.setOutput(outputLine(eliminated));
				res.append(eliminated);
				activeRes.removeOne(eliminated);
			}

			// points for final finishing position
			if(usePoints)
				awardPoints(activeRes, laps, true);

			// insert the actual finishers
			for(QList<XkorResult>::iterator i = activeRes.begin(); i != activeRes.end(); ++i)
			{
				i->setOutput(outputLine(*i));
				res.append(*i);
			}
		}
};

#endif // ELIMINATIONRACEPARADIGM_H
