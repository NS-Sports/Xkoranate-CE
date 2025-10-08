#ifndef TIMEDPARADIGM_H
#define TIMEDPARADIGM_H

#include <QFormLayout>
#include <QSpinBox>

#include "paradigms/abstractparadigm.h"
#include "paradigms/comparators/timedresultcomparator.h"
#include "paradigms/options/timedparadigmoptions.h"

struct XkorTimedParadigm : public XkorAbstractParadigm
{
	Q_OBJECT

	public:
		XkorTimedParadigm() : XkorAbstractParadigm()
		{
			supportedCompetitions.insert("standard", true);
			supportedCompetitions.insert("multipleRun", true);
		}

		XkorTimedParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractParadigm(sport, userOptions)
		{
			supportedCompetitions.insert("standard", true);
			supportedCompetitions.insert("multipleRun", true);
		}

	protected:
		virtual XkorAbstractResultComparator * comparisonFunction(QString type = QString())
		{
			return new XkorTimedResultComparator(type, opt);
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

		static bool compareVariant(QVariant a, QVariant b)
		{
			return a.toDouble() < b.toDouble();
		}

		static bool compareVariantDescending(QVariant a, QVariant b)
		{
			return b.toDouble() < a.toDouble();
		}

		virtual QString formatScore(double score)
		{
			return timeFormat(score, opt.value("displayDigits").toInt());
		}

		virtual bool hasOptionsWidget() { return true; }

		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> paradigmOptions)
		{
			return new XkorTimedParadigmOptions(paradigmOptions);
		}

		virtual void calculateTotal(XkorResult & r)
		{
			if(opt.value("totalType").toString() == "best")
			{
				QList<QVariant> attempts = r.result["attempts"].toList();
				if(opt.value("sortOrder", "ascending").toString() == "ascending")
					sort(attempts.begin(), attempts.end(), &XkorTimedParadigm::compareVariant);
				else
					sort(attempts.begin(), attempts.end(), &XkorTimedParadigm::compareVariantDescending);
				r.setScore(attempts[0].toDouble());
				for(int j = 0; j < attempts.count(); ++j)
					if(r.result["attempts"].toList()[j] == r.result["score"])
						r.setScoreString(r.result["attemptStrings"].toList()[j].toString());
			}
			else
			{
				double sum = 0;
				QList<QVariant> attempts = r.result["attempts"].toList();

				// check if any of the scores are failures
				for(int i = 0; i < attempts.size(); ++i)
					if(r.result["attempts"].toList()[i].toDouble() == numeric_limits<double>::max() || r.result["attempts"].toList()[i].toDouble() == numeric_limits<double>::min())
					{
						r.setScore(r.result["attempts"].toList()[i].toDouble());
						r.setScoreString(r.result["attemptStrings"].toList()[i].toString());
						r.setOutput(outputLine(r));
						return;
					}

				for(QList<QVariant>::iterator j = attempts.begin(); j != attempts.end(); ++j)
					sum += j->toDouble();
				r.setScore(sum);
				r.setScoreString(formatScore(sum));
			}

			r.setOutput(outputLine(r));
		}

		virtual XkorResult individualResult(XkorAthlete ath, QString type = QString(), QString roundingType = QString())
		{
			QList<QVariant> statuses = readOptionList("statuses");
			QList<QVariant> statusProbs = readOptionList("statusProbs");
			QList<QVariant> advancementProbs = readOptionList("advancementProbs");

			for(int i = 0; i < statuses.size(); ++i)
				if(s->randUniform() < statusProbs[i].toDouble())
				{
					XkorResult rval((opt.value("sortOrder", "ascending") == "ascending" ? numeric_limits<double>::max() : -numeric_limits<double>::max()), statuses[i].toString(), ath);
					if(i < advancementProbs.size() && s->randUniform() < advancementProbs[i].toDouble())
						rval.result["causesAdvancement"] = true;
					return rval;
				}

			// if there was no status…
			double score = s->individualScore(type, ath.rpSkill);
			if(opt.contains("lapped") && score == opt.value("lappedValue").toDouble())
				return XkorResult((opt.value("sortOrder", "ascending") == "ascending" ? numeric_limits<double>::max() : -numeric_limits<double>::max()), opt.value("lapped").toString(), ath);
			else
				return XkorResult(roundScore(score, roundingType), formatScore(roundScore(score, roundingType)), ath);
		}

		virtual QString outputLine(XkorResult r)
		{
			int nameWidth = userOpt.value("nameWidth", 20).toInt() + 2;
			int resultWidth = opt.value("resultWidth", 5).toInt() + 2;

			QString rval = formatName(r.athlete.name, r.athlete.nation).leftJustified(nameWidth);
			QList<QVariant> attemptStrings = r.value("attemptStrings").toList();
			for(QList<QVariant>::iterator i = attemptStrings.begin(); i != attemptStrings.end(); ++i)
				rval += i->toString().rightJustified(resultWidth);

			if(opt.value("attempts").toInt() > 1 || userOpt.value("runNumber").toInt() > 0) // only show the overall score if there’s more than one attempt
				rval += r.scoreString().rightJustified(2 + resultWidth * ((1 + userOpt.value("runNumber").toInt()) * (opt.value("attempts", 1).toInt() + opt.value("furtherAttempts").toInt()) - r.value("attempts").toList().size() + 1));

			if(opt.contains("decathlonA"))
			{
				double a = opt.value("decathlonA").toDouble();
				double b = opt.value("decathlonB").toDouble();
				double c = opt.value("decathlonC").toDouble();
				double multiplier = opt.value("decathlonMultiplier", 1).toDouble();

				double roundedValue = (int)(r.score() * pow(10.0, opt.value("displayDigits").toDouble()) + 1) * pow(10.0, -opt.value("displayDigits").toDouble());

				int decathlonPoints;
				if(opt.value("sortOrder", "ascending").toString() == "ascending")
				{
					if(r.score() == numeric_limits<double>::max())
						decathlonPoints = 0;
					else
						decathlonPoints = floor(a * pow(b - multiplier * roundedValue, c));
				}
				else
				{
					if(r.score() == -numeric_limits<double>::max())
						decathlonPoints = 0;
					else
						decathlonPoints = floor(a * pow(multiplier * roundedValue - b, c));
				}

				rval += QString::number(decathlonPoints).rightJustified(resultWidth);
			}

			return rval;
		}

		virtual double roundScore(double score, QString roundingType = QString())
		{
			QString tiebreakerDigits = "tiebreakerDigits", roundingValue = "roundingValue", displayDigits = "displayDigits";
			if(roundingType != QString())
			{
				tiebreakerDigits = roundingType + "TiebreakerDigits";
				roundingValue = roundingType + "RoundingValue";
				displayDigits = roundingType + "DisplayDigits";
			}

			double rounding;
			if(opt.contains(tiebreakerDigits))
				rounding = pow(10.0, -opt.value(tiebreakerDigits).toDouble());
			else if(opt.contains(roundingValue))
				rounding = opt.value(roundingValue).toDouble();
			else
				rounding = pow(10.0, -opt.value(displayDigits).toDouble());

			if(score == numeric_limits<double>::max() || score == -numeric_limits<double>::max())
				return score;
			else if((int)(score / rounding) == score / rounding)
				return (int)(score / rounding) * rounding;
			else
				return (int)(score / rounding + 1) * rounding;
		}

		virtual void scorinate(QList<XkorAthlete> athletes, QList<XkorResult> previousResults = QList<XkorResult>())
		{
			// check that we can use the sport
			for(QStringList::iterator i = requiredValues.begin(); i != requiredValues.end(); ++i)
				if(!(s->hasValue(*i)))
				{
					qDebug() << "missing parameter" << *i << "in XkorAbstractParadigm::scorinate(QList<XkorAthlete>)";
					out.append(qMakePair(QString(""), QString("Sport does not support this paradigm")));
					return;
				}

			// initialize results
			out.clear();
			res.clear();

			QList<QVariant> attemptTypes = readOptionList("attemptTypes");

			for(QList<XkorAthlete>::iterator i = athletes.begin(); i != athletes.end(); ++i)
			{
				XkorResult r;
				r.athlete = *i;

				// load up the previous result, if any
				for(QList<XkorResult>::iterator j = previousResults.begin(); j != previousResults.end(); ++j)
					if(j->athlete == *i)
					{
						QList<QVariant> attempts = r.result["attempts"].toList(), attemptStrings = r.result["attemptStrings"].toList();
						attempts.append(j->result["attempts"].toList());
						attemptStrings.append(j->result["attemptStrings"].toList());
						r.result["attempts"] = attempts;
						r.result["attemptStrings"] = attemptStrings;
					}

				int priorAttempts = r.result["attempts"].toList().size();
				for(int j = priorAttempts; j < priorAttempts + opt.value("attempts", 1).toInt(); ++j)
				{
					XkorResult result;
					int attemptTypeCount = attemptTypes.count();
					qDebug() << j << attemptTypeCount;
					if(attemptTypeCount > 0 && j >= attemptTypeCount)
						result = individualResult(*i, attemptTypes.at(attemptTypeCount - 1).toString());
					else if(attemptTypeCount > 0)
						result = individualResult(*i, attemptTypes.at(j).toString());
					else
						result = individualResult(*i, "score");
					QList<QVariant> attempts = r.result["attempts"].toList(), attemptStrings = r.result["attemptStrings"].toList();
					attempts.append(result.score());
					attemptStrings.append(result.scoreString());
					r.result["attempts"] = attempts;
					r.result["attemptStrings"] = attemptStrings;
					if(result.result.contains("causesAdvancement"))
						r.result["causesAdvancement"] = result.value("causesAdvancement");
					if(opt.value("totalType").toString() != "best" && (result.score() == numeric_limits<double>::max() || result.score() == -numeric_limits<double>::max()))
						break;
				}
				calculateTotal(r);

				res.append(r);
			}

			// figure out who was advanced
			if(userOpt.value("allowAdvancement", "true") == "true" && opt.contains("spontaneousAdvancementProb"))
				for(QVector<XkorResult>::iterator i = res.begin(); i != res.end(); ++i)
					if(i->score() != numeric_limits<double>::max() && i->score() != -numeric_limits<double>::max() && s->randUniform() < opt.value("spontaneousAdvancementProb").toDouble())
					{
						i->result["advanced"] = true;
						i->setOutput(i->output() + "  ADV");
					}
			if(userOpt.value("allowAdvancement", "true") == "true" && opt.contains("advancementProbs"))
			{
				int advancements = 0;
				for(QVector<XkorResult>::iterator i = res.begin(); i != res.end(); ++i)
					if(i->result.contains("causesAdvancement"))
						++advancements;
				qDebug() << advancements;

				comparisonFunction()->sort(res);
				for(int i = 0; i < advancements; ++i)
				{
					int rand = s->individualScore("advancer");

					// find the lowest-ranked athlete that fits the criteria
					int advancedIndex = res.size() - 1;
					while(advancedIndex > 0)
					{
						if(res[advancedIndex].score() == numeric_limits<double>::max() || res[advancedIndex].score() == -numeric_limits<double>::max() || res[advancedIndex].result["advanced"].toBool() == true)
							--advancedIndex;
						else if(rand > 0)
						{
							--advancedIndex;
							--rand;
						}
						else
						{
							res[advancedIndex].result["advanced"] = true;
							res[advancedIndex].setOutput(res[advancedIndex].output() + "  ADV");
							break;
						}
					}
				}
			}

			if(opt.contains("furtherAttempts"))
			{
				comparisonFunction()->sort(res);
				for(int i = 0; i < opt.value("furtherAttemptCutoff").toInt(); ++i)
				{
					XkorResult r = res[i];
					for(int j = 0; j < opt.value("furtherAttempts").toInt(); ++j)
					{
						XkorResult result = individualResult(r.athlete, "score");
						QList<QVariant> attempts = r.result["attempts"].toList(), attemptStrings = r.result["attemptStrings"].toList();
						attempts.append(result.score());
						attemptStrings.append(result.scoreString());
						r.result["attempts"] = attempts;
						r.result["attemptStrings"] = attemptStrings;

					}
					calculateTotal(r);
					res[i] = r;
				}
			}

			generateOutput();
		}
};

#endif
