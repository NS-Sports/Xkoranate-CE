#ifndef PGSPARADIGM_H
#define PGSPARADIGM_H

#include "timedparadigm.h"
#include "paradigms/options/pgsparadigmoptions.h"

class XkorPGSParadigm : public XkorTimedParadigm
{
	public:
		XkorPGSParadigm() : XkorTimedParadigm()
		{
			supportedCompetitions.clear();
			supportedCompetitions.insert("matches", true);
		}

		XkorPGSParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorTimedParadigm(sport, userOptions)
		{
			supportedCompetitions.clear();
			supportedCompetitions.insert("matches", true);
		}

		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> paradigmOptions)
		{
			return new XkorPGSParadigmOptions(paradigmOptions);
		}

		void calculateTotal(XkorResult & r)
		{
			QList<QVariant> attempts = r.result["attempts"].toList(), attemptStrings = r.result["attemptStrings"].toList();
			r.setScore(attempts[attempts.size() - 1].toDouble());
			r.setScoreString(attemptStrings[attemptStrings.size() - 1].toString());

			r.setOutput(outputLine(r));
		}

		virtual QString formatScore(double score)
		{
			return QString("+") + XkorTimedParadigm::formatScore(score);
		}

		virtual QString outputLine(XkorResult r)
		{
			int nameWidth = userOpt.value("nameWidth", 20).toInt() + 2;
			int resultWidth = opt.value("resultWidth", 5).toInt() + 2;

			QString rval = formatName(r.athlete.name, r.athlete.nation).leftJustified(nameWidth);
			QList<QVariant> attemptStrings = r.value("attemptStrings").toList();
			for(QList<QVariant>::iterator i = attemptStrings.begin(); i != attemptStrings.end(); ++i)
				rval += i->toString().rightJustified(resultWidth);

			return rval;
		}

		virtual void scorinate(QList<XkorAthlete> athletes, QList<XkorResult> = QList<XkorResult>())
		{
			// initialize results
			out.clear();
			res.clear();

			int acc = 0;
			for(QList<XkorAthlete>::iterator away = athletes.begin(); away != athletes.end(); ++away)
			{
				if(acc & 1)
				{
					QList<XkorAthlete>::iterator home = away; // home = away - 1
					--home;

					XkorResult homeResult, awayResult;
					homeResult.athlete = *home;
					awayResult.athlete = *away;

					for(int i = 0; i < opt.value("attempts", 1).toInt(); ++i)
					{
						XkorResult result;

						// scorinate the home result
						result = individualResult(*home, "score");
						QList<QVariant> attempts = homeResult.result["attempts"].toList(), attemptStrings = homeResult.result["attemptStrings"].toList();
						attempts.append(result.score());
						attemptStrings.append(result.scoreString());
						homeResult.result["attempts"] = attempts;
						homeResult.result["attemptStrings"] = attemptStrings;

						// scorinate the away result
						result = individualResult(*away, "score");
						attempts = awayResult.result["attempts"].toList();
						attemptStrings = awayResult.result["attemptStrings"].toList();
						attempts.append(result.score());
						attemptStrings.append(result.scoreString());
						awayResult.result["attempts"] = attempts;
						awayResult.result["attemptStrings"] = attemptStrings;
					}

					// sort out PGS stuff
					double totalHomeScore = 0, totalAwayScore = 0, thisScore = 0;
					double maxScore = userOpt.value("maxScore", 1.5).toDouble();
					for(int i = 0; i < opt.value("attempts", 1).toInt(); ++i)
					{
						if(homeResult.value("attempts").toList()[i] == numeric_limits<double>::max())
						{
							totalHomeScore = maxScore;
							totalAwayScore = 0;
							QList<QVariant> attempts = awayResult.result["attempts"].toList(), attemptStrings = awayResult.result["attemptStrings"].toList();
							attempts[i] = 0;
							attemptStrings[i] = "";
							awayResult.result["attempts"] = attempts;
							awayResult.result["attemptStrings"] = attemptStrings;
						}
						else if(awayResult.value("attempts").toList()[i] == numeric_limits<double>::max())
						{
							totalAwayScore = maxScore;
							totalHomeScore = 0;
							QList<QVariant> attempts = homeResult.result["attempts"].toList(), attemptStrings = homeResult.result["attemptStrings"].toList();
							attempts[i] = 0;
							attemptStrings[i] = "";
							homeResult.result["attempts"] = attempts;
							homeResult.result["attemptStrings"] = attemptStrings;
						}
						else
						{
							totalHomeScore += homeResult.value("attempts").toList()[i].toDouble();
							totalAwayScore += awayResult.value("attempts").toList()[i].toDouble();
							if(totalHomeScore > totalAwayScore)
							{
								totalHomeScore -= totalAwayScore;
								thisScore = totalHomeScore;
								if(totalHomeScore > maxScore)
									totalHomeScore = maxScore;
								totalAwayScore = 0;

								QList<QVariant> attempts = homeResult.result["attempts"].toList(), attemptStrings = homeResult.result["attemptStrings"].toList();
								attempts[i] = thisScore;
								attemptStrings[i] = formatScore(thisScore);
								homeResult.result["attempts"] = attempts;
								homeResult.result["attemptStrings"] = attemptStrings;

								attempts = awayResult.result["attempts"].toList();
								attemptStrings = awayResult.result["attemptStrings"].toList();
								attempts[i] = 0;
								attemptStrings[i] = "";
								awayResult.result["attempts"] = attempts;
								awayResult.result["attemptStrings"] = attemptStrings;
							}
							else
							{
								totalAwayScore -= totalHomeScore;
								thisScore = totalAwayScore;
								if(totalAwayScore > maxScore)
									totalAwayScore = maxScore;
								totalHomeScore = 0;

								QList<QVariant> attempts = homeResult.result["attempts"].toList(), attemptStrings = homeResult.result["attemptStrings"].toList();
								attempts[i] = 0;
								attemptStrings[i] = "";
								homeResult.result["attempts"] = attempts;
								homeResult.result["attemptStrings"] = attemptStrings;

								attempts = awayResult.result["attempts"].toList();
								attemptStrings = awayResult.result["attemptStrings"].toList();
								attempts[i] = thisScore;
								attemptStrings[i] = formatScore(thisScore);
								awayResult.result["attempts"] = attempts;
								awayResult.result["attemptStrings"] = attemptStrings;
							}
						}
						qDebug() << totalHomeScore << totalAwayScore;
					}

					homeResult.setOutput(outputLine(homeResult));
					awayResult.setOutput(outputLine(awayResult) + "\n");

					res.append(homeResult);
					res.append(awayResult);
				}
				++acc;
			}

			generateOutput();
		}
};

#endif // PGSPARADIGM_H
