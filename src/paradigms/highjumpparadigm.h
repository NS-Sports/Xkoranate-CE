#ifndef HIGHJUMPPARADIGM_H
#define HIGHJUMPPARADIGM_H

#include "paradigms/abstractparadigm.h"
#include "paradigms/comparators/highjumpresultcomparator.h"
#include "paradigms/options/timedparadigmoptions.h"

class XkorHighJumpParadigm : public XkorAbstractParadigm
{
	public:
		XkorHighJumpParadigm() : XkorAbstractParadigm()
		{
			supportedCompetitions.insert("standard", true);
		}

		XkorHighJumpParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractParadigm(sport, userOptions)
		{
			supportedCompetitions.insert("standard", true);
		}

		virtual void breakTie(QList<XkorAthlete>)
		{
			qDebug() << "unexpected tie break request in high jump paradigm";
		}

		virtual XkorAbstractResultComparator * comparisonFunction(QString type = QString())
		{
			return new XkorHighJumpResultComparator(type, opt);
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

			// load options
			int nameWidth = userOpt.value("nameWidth", 20).toInt() + 2;
			int resultWidth = opt.value("resultWidth", 3).toInt() + 2;

			QList<QVariant> heights = readOptionList("heights");
			int attemptsPerHeight = opt.value("attemptsPerHeight").toInt();
			int displayDigits = opt.value("displayDigits").toInt();

			// initialize results
			out.clear();
			res.clear();

			for(QList<XkorAthlete>::iterator athlete = athletes.begin(); athlete != athletes.end(); ++athlete)
			{
				XkorResult r;
				r.athlete = *athlete;
				QString resultString = formatName(athlete->name, athlete->nation).leftJustified(nameWidth);
				double bestHeight = 0;
				for(QList<QVariant>::iterator i = heights.begin(); i != heights.end(); ++i)
				{
					int failures = 0;
					for(int j = 0; j < attemptsPerHeight; ++j)
					{
						bool success = generateScore(athlete->rpSkill, i->toDouble());
						if(!success)
							++failures;
						else
							break;
					}

					QString subResult(failures, 'x');
					if(failures < attemptsPerHeight)
					{
						bestHeight = i->toDouble();
						subResult += "o";
					}
					resultString += subResult.rightJustified(resultWidth);
					r.setScore(bestHeight);
					if(failures == attemptsPerHeight)
						break;
					else
					{
						r.result["lastHeightFailures"] = failures;
						r.result["failures"] = r.value("failures").toInt() + failures;
					}
				}

				// show final result
				resultString = resultString.leftJustified(nameWidth + resultWidth * heights.size()) + QString::number(bestHeight, 'f', displayDigits).rightJustified(resultWidth);

				if(opt.contains("decathlonA"))
				{
					double a = opt.value("decathlonA").toDouble();
					double b = opt.value("decathlonB").toDouble();
					double c = opt.value("decathlonC").toDouble();
					double multiplier = opt.value("decathlonMultiplier", 1).toDouble();

					double roundedValue = QString::number(bestHeight, 'f', opt.value("displayDigits").toInt()).toDouble();

					int decathlonPoints;
					if(bestHeight == 0)
						decathlonPoints = 0;
					else
						decathlonPoints = floor(a * pow(multiplier * roundedValue - b, c));

					resultString += QString::number(decathlonPoints).rightJustified(resultWidth);
				}

				r.setOutput(resultString);
				out.append(qMakePair(athlete->name, resultString));

				res.append(r);
			}
		}

	protected:
		virtual double adjustSkill(double realSkill)
		{
			double rankModifier = opt.value("rankModifier", 0).toDouble();
			double rankAdjustmentFactor = opt.value("rankAdjustmentFactor", 1).toDouble();

			// randomize the skill a bit to compensate for the fact that long sequences of arrows would otherwise be highly predictable
			double rval = (realSkill + s->randUniform() * rankModifier) / (1 + rankModifier);
			rval = rval * rankAdjustmentFactor + (1 - rankAdjustmentFactor) / 2;
			return rval;
		}

		virtual XkorResult individualResult(double, double) { return XkorResult(); } // unused
		virtual XkorResult individualResult(double, double, QString) { return XkorResult(); } // unused

		virtual bool generateScore(double skill, double height)
		{
			double attackModifier = opt.value("attackModifier").toDouble();
			double rand = (s->randWeighted(skill) + s->randUniform() * attackModifier) / (1 + attackModifier);

			double threshold = opt.value("baseProb").toDouble() * pow(opt.value("depreciation").toDouble(), opt.value("depreciationCoefficient").toDouble() * height - opt.value("depreciationBase").toDouble());

			return ((1 - rand) < threshold); // (1 - rand) is used so that good athletes do well rather than poorly :)
		}
};

#endif // HIGHJUMPPARADIGM_H
