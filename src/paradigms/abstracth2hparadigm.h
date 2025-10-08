#ifndef ABSTRACTH2HPARADIGM_H
#define ABSTRACTH2HPARADIGM_H

#include "paradigms/abstractparadigm.h"
#include "paradigms/comparators/h2hresultcomparator.h"

class XkorAbstractH2HParadigm : public XkorAbstractParadigm
{
	public:
		XkorAbstractH2HParadigm() : XkorAbstractParadigm()
		{
			supportedCompetitions.insert("matches", true);
			supportedCompetitions.insert("roundRobin", true);
		}

		XkorAbstractH2HParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractParadigm(sport, userOptions)
		{
			supportedCompetitions.insert("matches", true);
			supportedCompetitions.insert("roundRobin", true);
		}

		virtual void breakTie(QList<XkorAthlete> athletes, QString = QString())
		{
			int acc = 0;

			QList<QVariant> tiebreakers = readOptionList("tiebreakers");
			QList<QVariant> tiebreakerNames = readOptionList("tiebreakerNames");

			for(QList<XkorAthlete>::iterator away = athletes.begin(); away != athletes.end(); ++away)
			{
				if(acc & 1)
				{
					QList<XkorAthlete>::iterator home = away; // home = away - 1
					--home;

					QVector<XkorResult>::iterator score1 = findResult(home->id), score2 = findResult(away->id);

					QList<QVariant>::iterator name = tiebreakerNames.begin();
					for(QList<QVariant>::iterator i = tiebreakers.begin(); i != tiebreakers.end(); ++i)
					{
						QPair<XkorResult, XkorResult> score;
						if(i->toString() == "decision")
							score = generateDecisionScore(*score1, *score2, name->toString());
						else if(i->toString() == "extratime")
							score = generateETScore(*score1, *score2, name->toString());
						else if(i->toString() == "goldengoal")
							score = generateGGScore(*score1, *score2, name->toString());
						else if(i->toString() == "ifaf")
							score = generateIFAFScore(*score1, *score2, name->toString(), score1->result["OTs"].toInt() + 1);
						else if(i->toString() == "shootout")
							score = generateSOScore(*score1, *score2, name->toString());
						else
							qDebug() << "bad tiebreaker" << i->toString();

						// output the score
						for(QList<QPair<QString, QString> >::iterator j = out.begin(); j != out.end(); ++j)
							if(j->first == home->name)
								j->second = outputLine(score.first, score.second);

						// let the competition know the result
						*score1 = score.first;
						*score2 = score.second;

						// keep track of the number of overtimes under IFAF rules
						if(i->toString() == "ifaf")
						{
							score1->result["OTs"] = score1->result["OTs"].toInt() + 1;
							score2->result["OTs"] = score2->result["OTs"].toInt() + 1;
						}

						// are we done?
						if(compare(score.first, score.second) != 0)
							break;

						// are we on the last tiebreaker? if so, let’s do it again
						if(i + 1 == tiebreakers.end())
						{
							--i;
							--name;
						}

						++name;
					}
				}
				++acc;
			}
		}

		virtual XkorAbstractResultComparator * comparisonFunction(QString type = QString())
		{
			return new XkorH2HResultComparator(type, opt);
		}

		virtual XkorAbstractAthleteWidget * newAthleteWidget()
		{
			if(userOpt.value("styleMods") != "false" || userOpt.value("NSFSStyleMods") != "false")
				return new XkorAthleteWidget(QStringList() << "name" << "nation" << "skill" << "style",
							     QStringList() << tr("Participant") << tr("Team") << tr("Skill") << tr("Style"),
							     QStringList() << "string" << "string" << "skill" << "double",
							     -5, 5, 1);
			else
				return XkorAbstractParadigm::newAthleteWidget();
		}

		virtual void scorinate(QList<XkorAthlete> athletes, QList<XkorResult> = QList<XkorResult>())
		{
			for(QStringList::iterator i = requiredValues.begin(); i != requiredValues.end(); ++i)
				if(!(s->hasValue(*i)))
				{
					qDebug() << "missing parameter" << *i << "in XkorAbstractH2HParadigm::output(XkorResults *)";
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

					QPair<XkorResult, XkorResult> score = generateFTScore(*home, *away);

					QString status;
					if(compare(score.first, score.second) == 0)
						status = generateStatus();

					if(!status.isEmpty() && compare(score.first, score.second) == -1)
						score.second.result["status"] = status;
					else if(!status.isEmpty())
						score.first.result["status"] = status;

					// output the FT score
					out.append(qMakePair(home->name, outputLine(score.first, score.second)));
					res.append(score.first);
					res.append(score.second);

					if(compare(score.first, score.second) == 0 && opt.value("forceTieBreak").toInt() == 1) // tiebreakers are required for this sport
						breakTie(QList<XkorAthlete>() << *home << *away);
				}
				++acc;
			}
		}

	protected:
		virtual QString formatGaelicScore(QList<QVariant> subScores, int totalScore)
		{
			QString result;
			if(opt.value("resultStyle") == "australian")
			{
				for(int i = 0; i < subScores.size(); ++i)
				{
					if(i > 0)
						result += ".";
					result += QString::number(subScores[i].toInt());
				}
				result += "." + QString::number(totalScore);
			}
			else // gaelic
			{
				for(int i = 0; i < subScores.size(); ++i)
				{
					if(i > 0)
						result += "-";
					result += QString::number(subScores[i].toInt());
				}
				result += " (" + QString::number(totalScore) + ")";
			}
			return result;
		}

		virtual QString formatGaelicScore(XkorResult score1, XkorResult score2)
		{
			QString rval;
			rval = formatName(score1.athlete) + " " + formatGaelicScore(score1.value("subScores").toList(), score1.score());
			if(score1.score() > score2.score())
				rval += " def. ";
			else if(score1.score() == score2.score())
				rval += " drew with ";
			else
				rval += " def. by ";
			rval += formatName(score2.athlete) + " " + formatGaelicScore(score2.value("subScores").toList(), score2.score());

			// extra time and stuff
			QList<QString> usedTiebreakers;
			QList<QVariant> tiebreakers = readOptionList("tiebreakers");
			QList<QVariant> tiebreakerNames = readOptionList("tiebreakerNames");
			QList<QVariant>::iterator type = tiebreakers.begin();
			for(QList<QVariant>::iterator i = tiebreakerNames.begin(); i != tiebreakerNames.end(); ++i)
			{
				if(!usedTiebreakers.contains(i->toString()) && (!score1.value(i->toString()).isNull() || !score2.value(i->toString()).isNull()))
					rval += " (" + formatGaelicScore(score1.value(i->toString()).toList(), score1.value(i->toString() + "SubScores").toDouble())
						+ " – " + formatGaelicScore(score2.value(i->toString()).toList(), score2.value(i->toString() + "SubScores").toDouble()) + ")";
				usedTiebreakers << i->toString();
				++type;
			}
			return rval;
		}

		virtual QString formatPeriodScore(XkorResult score1, XkorResult score2)
		{
			int nameWidth = userOpt.value("nameWidth", 20).toInt() + 2;
			int resultWidth = opt.value("resultWidth").toInt();
			int periods = opt.value("periods").toInt();

			QString homeScore = formatName(score1.athlete).leftJustified(nameWidth);
			QString awayScore = formatName(score2.athlete).leftJustified(nameWidth);

			QList<QVariant> subScores1 = score1.value("subScores").toList();
			QList<QVariant> subScores2 = score2.value("subScores").toList();
			for(int i = 0; i < subScores1.size() || i < subScores2.size() || i < periods; ++i)
			{
				QString homeSubScore, awaySubScore;
				if(i < subScores1.size())
					homeSubScore = subScores1[i].toString().rightJustified(resultWidth);
				else
					homeSubScore = QString("X").rightJustified(resultWidth);
				if(i < subScores2.size())
					awaySubScore = subScores2[i].toString().rightJustified(resultWidth);
				else
					awaySubScore = QString("X").rightJustified(resultWidth);

				// if we have a result that’s too long, pad the other one to match
				if(homeSubScore.size() > resultWidth)
					awaySubScore = awaySubScore.rightJustified(homeSubScore.size());
				if(awaySubScore.size() > resultWidth)
					homeSubScore = homeSubScore.rightJustified(awaySubScore.size());

				homeScore.append(" " + homeSubScore);
				awayScore.append(" " + awaySubScore);
			}

			double homeTotal = score1.score();
			double awayTotal = score2.score();

			// overtime and stuff
			QList<QString> usedTiebreakers;
			QList<QVariant> tiebreakers = readOptionList("tiebreakers");
			QList<QVariant> tiebreakerNames = readOptionList("tiebreakerNames");
			QList<QVariant>::iterator type = tiebreakers.begin();
			for(QList<QVariant>::iterator i = tiebreakerNames.begin(); i != tiebreakerNames.end(); ++i)
			{
				if(!usedTiebreakers.contains(i->toString()) && (!score1.value(i->toString()).isNull() || !score2.value(i->toString()).isNull()))
				{
					if(score1.contains(i->toString()))
						homeScore.append(score1.value(i->toString()).toString().rightJustified(resultWidth));
					else
						homeScore.append(QString("X").rightJustified(resultWidth));
					if(score2.contains(i->toString()))
						awayScore.append(score2.value(i->toString()).toString().rightJustified(resultWidth));
					else
						awayScore.append(QString("X").rightJustified(resultWidth));
					homeTotal += score1.value(i->toString()).toDouble();
					awayTotal += score2.value(i->toString()).toDouble();
				}
				usedTiebreakers << i->toString();
				++type;
			}

			homeScore.append(QString::number(homeTotal).rightJustified(resultWidth + 2));
			awayScore.append(QString::number(awayTotal).rightJustified(resultWidth + 2));

			return homeScore + "\n" + awayScore + "\n";
		}

		virtual QString formatScore(double score1, double score2)
		{
			return QString::number(score1) + "–" + QString::number(score2);
		}

		virtual QString formatScore(XkorResult score1, XkorResult score2)
		{
			QString rval = formatName(score1.athlete) + " " + formatScore(score1.score(), score2.score()) + " " + formatName(score2.athlete);

			// extra time and stuff
			QList<QString> usedTiebreakers;
			QList<QVariant> tiebreakers = readOptionList("tiebreakers");
			QList<QVariant> tiebreakerNames = readOptionList("tiebreakerNames");
			QList<QVariant>::iterator type = tiebreakers.begin();
			for(QList<QVariant>::iterator i = tiebreakerNames.begin(); i != tiebreakerNames.end(); ++i)
			{
				if(!usedTiebreakers.contains(i->toString()) && (!score1.value(i->toString()).isNull() || !score2.value(i->toString()).isNull()))
				{
					if(type->toString() == "shootout")
						rval += " (" + formatScore(score1.value(i->toString()).toDouble(), score2.value(i->toString()).toDouble()) + " " + i->toString() + ")";
					else
						rval += " (" + formatScore(score1.score() + score1.value(i->toString()).toDouble(), score2.score() +score2.value(i->toString()).toDouble()) + " " + i->toString() + ")";
				}
				usedTiebreakers << i->toString();
				++type;
			}

			return rval;
		}

		virtual int generateConversions(double count, int forceValue = -1)
		{
			int rval = 0;

			QList<QVariant> conversionValues = readOptionList("conversionValues");
			QList<QVariant> conversionSelection = readOptionList("conversionSelection");
			QList<QVariant> conversionSuccess = readOptionList("conversionSuccess");

			for(int i = 0; i < count; ++i)
			{
				double rand = s->randUniform();
				QList<QVariant>::iterator selection = conversionSelection.begin(), successProb = conversionSuccess.begin();
				double selectionAcc = selection->toDouble(); // the total probability of us selecting any of the conversion types we’ve tried so far
				int conversionIndex = 0;
				for(QList<QVariant>::iterator j = conversionValues.begin(); j != conversionValues.end(); ++j)
				{
					if(forceValue == conversionIndex || (forceValue < 0 && rand < selectionAcc))
					{
						if(s->randUniform() < successProb->toDouble())
							rval += j->toInt(); // conversion successful!
						break;
					}

					++selection;
					selectionAcc += selection->toDouble();
					++successProb;
					++conversionIndex;
				}
			}

			return rval;
		}

		virtual QPair<XkorResult, XkorResult> generateDecisionScore(XkorResult home, XkorResult away, QString str)
		{
			double homeScore = s->randWeighted(home.athlete.rpSkill, away.athlete.rpSkill) * numeric_limits<int>::max();
			double awayScore = s->randWeighted(away.athlete.rpSkill, home.athlete.rpSkill) * numeric_limits<int>::max();
			if(homeScore > awayScore)
				home.result["decision"] = str;
			else
				away.result["decision"] = str;
			return qMakePair(home, away);
		}

		virtual QPair<XkorResult, XkorResult> generateETScore(XkorResult home, XkorResult away, QString str)
		{
			double attackMultiplier = opt.value("etAttackCoeff").toDouble();
			bool homeAdvantage = (userOpt.value("homeAdvantage").toString() == "true");
			QList<QVariant> pointValues = readOptionList("pointValues");
			QList<QVariant> attackCoeffs = readOptionList("attackCoeffs");

			QString scoreType = (str.isEmpty() ? "score" : str);
			QString subScoreType = (str.isEmpty() ? "subScores" : str + "SubScores");

			QList<QVariant>::iterator attackCoeff = attackCoeffs.begin();
			for(int i = 0; i < pointValues.size(); ++i) // for each type of scoring
			{
				int score1 = generateScore(home.athlete.rpSkill, away.athlete.rpSkill, 0, 0, homeAdvantage, attackCoeff->toDouble() * attackMultiplier);
				int score2 = generateScore(away.athlete.rpSkill, home.athlete.rpSkill, 0, 0, false, attackCoeff->toDouble() * attackMultiplier);

				// add to overall score
				home.result[scoreType] = home.value(scoreType).toDouble() + score1 * pointValues[i].toInt();
				away.result[scoreType] = away.value(scoreType).toDouble() + score2 * pointValues[i].toInt();

				// store the sub-score
				QList<QVariant> homeScores = home.result[subScoreType].toList();
				QList<QVariant> awayScores = away.result[subScoreType].toList();
				homeScores.append(score1);
				awayScores.append(score2);
				home.result[subScoreType] = homeScores;
				away.result[subScoreType] = awayScores;

				if(i == 0 && opt.value("conversionValues") != QVariant())
				{
					home.result[scoreType] = home.value(scoreType).toDouble() + generateConversions(score1);
					away.result[scoreType] = away.value(scoreType).toDouble() + generateConversions(score2);
				}

				++attackCoeff;
			}

			return qMakePair(home, away);
		}

		virtual QPair<XkorResult, XkorResult> generateFTScore(XkorAthlete home, XkorAthlete away)
		{
			XkorResult hRes, aRes;
			hRes.athlete = home;
			aRes.athlete = away;

			bool homeAdvantage = (userOpt.value("homeAdvantage").toString() == "true");
			QList<QVariant> pointValues = readOptionList("pointValues");
			QList<QVariant> attackCoeffs = readOptionList("attackCoeffs");

			QList<QVariant>::iterator attackCoeff = attackCoeffs.begin();
			for(QList<QVariant>::iterator i = pointValues.begin(); i != pointValues.end(); ++i) // for each type of scoring
			{
				// zero out the styles because we use our own system
				int score1 = generateScore(home.rpSkill, away.rpSkill, 0, 0, homeAdvantage, attackCoeff->toDouble());
				int score2 = generateScore(away.rpSkill, home.rpSkill, 0, 0, false, attackCoeff->toDouble());

				// calculate style effect
				if(s->hasValue("style") && (userOpt.value("styleMods") != "false" || userOpt.value("NSFSStyleMods") != "false")) // on by default
				{
					QPair<int, int> styleResults = generateStyleModification(score1, score2, home.property("style").toDouble(), away.property("style").toDouble());
					score1 = styleResults.first;
					score2 = styleResults.second;
				}

				// add to overall score
				hRes.setScore(hRes.score() + score1 * i->toInt());
				aRes.setScore(aRes.score() + score2 * i->toInt());

				// store the sub-score
				QList<QVariant> hResScores = hRes.result["subScores"].toList();
				QList<QVariant> aResScores = aRes.result["subScores"].toList();
				hResScores.append(score1);
				aResScores.append(score2);
				hRes.result["subScores"] = hResScores;
				aRes.result["subScores"] = aResScores;

				if(i == pointValues.begin() && opt.value("conversionValues") != QVariant())
				{
					hRes.setScore(hRes.score() + generateConversions(score1));
					aRes.setScore(aRes.score() + generateConversions(score2));
				}

				++attackCoeff;
			}

			return qMakePair(hRes, aRes);
		}

		virtual QPair<XkorResult, XkorResult> generateGGScore(XkorResult home, XkorResult away, QString str)
		{
			double goldenGoalProb = opt.value("goldenGoalProb").toDouble();
			double homeAdvantage = opt.value("homeAdvantageGG").toDouble();

			QString scoreType = (str.isEmpty() ? "score" : str);

			if(s->randUniform() < goldenGoalProb) // someone scored; who was it?
			{
				// what type of score?
				int scoreValue = 1;
				QList<QVariant> ggPointProbs = readOptionList("goldenGoalPointProbs");
				QList<QVariant> pointValues = readOptionList("pointValues");
				if(ggPointProbs == QVariant())
					scoreValue = pointValues.begin()->toInt();
				else
				{
					double rand = s->randUniform();
					double probAcc = 0;
					QList<QVariant>::iterator currentValue = pointValues.begin();
					for(QList<QVariant>::iterator i = ggPointProbs.begin(); i != ggPointProbs.end(); ++i)
					{
						probAcc += i->toDouble();
						if(rand < probAcc)
						{
							scoreValue = currentValue->toInt();
							break;
						}
						++currentValue;
					}
				}

				double homeSkill = home.athlete.rpSkill, awaySkill = away.athlete.rpSkill;
				if(userOpt.value("homeAdvantage") == "true")
				{
					homeSkill = homeSkill * (1 - homeAdvantage) + homeAdvantage;
					awaySkill = awaySkill * (1 - homeAdvantage);
				}
				double otScore1 = s->randWeighted(homeSkill, awaySkill);
				double otScore2 = s->randWeighted(awaySkill, homeSkill);

				if(otScore1 > otScore2)
					home.result[scoreType] = home.result[scoreType].toDouble() + scoreValue;
				else
					away.result[scoreType] = away.result[scoreType].toDouble() + scoreValue;
			}
			return qMakePair(home, away);
		}

		virtual QPair<XkorResult, XkorResult> generateIFAFScore(XkorResult home, XkorResult away, QString str, int otNumber)
		{

			// what type of score?
			int firstScore = 0, secondScore = 0;
			QList<QVariant> ifafPointProbs = readOptionList("ifafPointProbs");
			QList<QVariant> pointValues = readOptionList("pointValues");

			// coin flip to decide which team went first
			// theoretically this should be done only once, but it doesn’t matter
			bool homeTeamWentFirst = (s->randUniform() > 0.5);
			XkorResult * firstTeam, * secondTeam;
			if(homeTeamWentFirst)
			{
				firstTeam = &home;
				secondTeam = &away;
			}
			else
			{
				firstTeam = &away;
				secondTeam = &home;
			}

			// scorinate random numbers
			double homeAdvantage = opt.value("homeAdvantageIFAF").toDouble();
			double firstSkill = firstTeam->athlete.rpSkill, secondSkill = secondTeam->athlete.rpSkill;
			if(homeAdvantage)
			{
				firstSkill = firstSkill * (1 - homeAdvantage) + homeAdvantage;
				secondSkill = secondSkill * (1 - homeAdvantage);
			}
			double randFirst = s->randWeighted(firstSkill, secondSkill);
			double randSecond = s->randWeighted(secondSkill, firstSkill);

			QList<QVariant>::iterator currentValue = pointValues.begin();
			bool canHaveConversion = true; // only the first value can have a conversion
			double probAcc = 0;
			for(QList<QVariant>::iterator i = ifafPointProbs.begin(); i != ifafPointProbs.end(); ++i)
			{
				probAcc += i->toDouble();

				if(firstScore == 0 && randFirst < probAcc)
					firstScore = currentValue->toInt();
				if(secondScore == 0 && randSecond < probAcc)
					secondScore = currentValue->toInt();
				if(canHaveConversion)
				{
					if(firstScore > 0)
					{
						if(otNumber >= 3)
							firstScore += generateConversions(1, 1);
						else
							firstScore += generateConversions(1);
					}
					if(secondScore > 0)
					{
						if(otNumber >= 3)
							secondScore += generateConversions(1, 1);
						else if(firstScore == secondScore)
							secondScore += generateConversions(1, 0); // get the easiest conversion possible if we’re tied
						else if(firstScore > secondScore)
							secondScore += generateConversions(1);
					}
				}
				if(firstScore > 0)
					break; // if the team that went first scored a touchdown, their opponent won’t bother attempting a field goal
				++currentValue;
				canHaveConversion = false;
			}

			firstTeam->result[str] = firstTeam->value(str).toDouble() + firstScore;
			secondTeam->result[str] = secondTeam->value(str).toDouble() + secondScore;

			return qMakePair(home, away);
		}

		virtual QPair<XkorResult, XkorResult> generateSOScore(XkorResult home, XkorResult away, QString str)
		{
			int pkCount = opt.value("shootoutLength").toInt();
			double pkProb = opt.value("shootoutProb").toDouble();

			for(int count = 0; (int)abs(home.value(str).toInt() - away.value(str).toInt()) <= pkCount - count; ++count)
			{
				if(s->randUniform() < pkProb)
					home.result[str] = home.value(str).toInt() + 1;
				if(s->randUniform() < pkProb)
					away.result[str] = away.value(str).toInt() + 1;
				if(count >= pkCount)
					--count;
			}

			return qMakePair(home, away);
		}

		virtual int generateScore(double skill, double oppSkill, double style, double oppStyle, bool homeAdvantage = false, double attackMultiplier = 1) = 0;

		virtual QPair<int, int> generateStyleModification(int homeScore, int awayScore, double homeStyle, double awayStyle)
		{
			if(userOpt.value("NSFSStyleMods").toString() == "true")
			{
				double styleCoeffA = opt.value("NSFSStyleCoeffA", 2.0991677816057).toDouble();
				double styleCoeffB = opt.value("NSFSStyleCoeffB", 1.2442581729602).toDouble();
				double styleExponent = opt.value("NSFSStyleExponent", -0.42705203296846).toDouble();
				double styleOffset = opt.value("NSFSStyleOffset", 0.072435335725325).toDouble();

				double styleModifier = (homeStyle + awayStyle) / 2.0 + s->randGaussian();
				int result = homeScore - awayScore;

				double styleMultiplier = styleCoeffA / (1 + styleCoeffB * pow(M_E, styleExponent * styleModifier)) + styleOffset;

				homeScore *= styleMultiplier;
				awayScore *= styleMultiplier;

				// if a negative style modifier changed the result to a draw, fix it
				if(homeScore == awayScore && result > 0)
					++homeScore;
				else if(homeScore == awayScore && result < 0)
					++awayScore;

				return qMakePair(homeScore, awayScore);
			}
			else
			{
				int styleEffect = 0;
				int maxStyleEffect = (homeScore == awayScore ? 0 : abs(homeScore - awayScore) - 1) + min(homeScore, awayScore); // what’s the maximum change we can make that will preserve W/D/L?
				styleEffect = (int)(s->individualScore("style", (homeStyle + awayStyle) / 20 + 0.5));
				if(styleEffect < -maxStyleEffect)
					styleEffect = -maxStyleEffect;
				homeScore += max(-homeScore, styleEffect); // don’t drop the score below 0
				awayScore += max(-awayScore, styleEffect); // don’t drop the score below 0
				return qMakePair(homeScore, awayScore);
			}
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

		virtual QString outputLine(XkorResult home, XkorResult away)
		{
			QString rval;
			if(!home.value("status").toString().isEmpty())
				rval = formatName(home.athlete) + " def. by " + formatName(away.athlete) + " (" + home.value("status").toString() + ")";
			else if(!away.value("status").toString().isEmpty())
				rval = formatName(home.athlete) + " def. " + formatName(away.athlete) + " (" + away.value("status").toString() + ")";
			else if(!home.value("decision").isNull())
				rval = formatName(home.athlete) + " " + home.value("decision").toString() + formatScore(home.score(), away.score()) + " " + formatName(away.athlete);
			else if(!away.value("decision").isNull())
				rval = formatName(home.athlete) + " " + formatScore(home.score(), away.score()) + away.value("decision").toString() + " " + formatName(away.athlete);
			else if(opt.value("resultStyle").toString() == "periods" && userOpt.value("usePeriods", "true") == "true")
				rval = formatPeriodScore(home, away);
			else if(opt.value("resultStyle") == "australian" || opt.value("resultStyle") == "gaelic")
				rval = formatGaelicScore(home, away);
			else
				rval = formatScore(home, away);
			return rval;
		}
};

#endif
