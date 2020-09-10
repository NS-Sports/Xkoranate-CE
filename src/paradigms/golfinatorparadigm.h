#ifndef GOLFINATORPARADIGM_H
#define GOLFINATORPARADIGM_H

#include "paradigms/abstractparadigm.h"
#include "paradigms/comparators/basicresultcomparator.h"
#include "paradigms/options/golfinatorparadigmoptions.h"

class XkorGolfinatorParadigm : public XkorAbstractParadigm
{
	public:
		XkorGolfinatorParadigm() : XkorAbstractParadigm()
		{
			supportedCompetitions.insert("standard", true);
		}

		XkorGolfinatorParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorAbstractParadigm(sport, userOptions)
		{
			supportedCompetitions.insert("standard", true);
		}

		virtual XkorAbstractResultComparator * comparisonFunction(QString type = QString())
		{
			return new XkorBasicResultComparator(type, opt);
		}

		virtual bool hasOptionsWidget() { return true; }

		virtual XkorAbstractAthleteWidget * newAthleteWidget()
		{
			return new XkorAthleteWidget(QStringList() << "name" << "nation" << "skill" << "style",
						     QStringList() << tr("Participant") << tr("Team") << tr("Skill") << tr("Style"),
						     QStringList() << "string" << "string" << "skill" << "golfStyle");
		}

		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> paradigmOptions)
		{
			return new XkorGolfinatorParadigmOptions(paradigmOptions);
		}

		virtual void scorinate(QList<XkorAthlete> athletes, QList<XkorResult> = QList<XkorResult>())
		{
			// load options
			int nameWidth = userOpt.value("nameWidth", 20).toInt() + 2;
			int resultWidth = opt.value("resultWidth", 1).toInt() + 1;

			int holes = opt.value("holes", 18).toInt();
			QList<QVariant> yardage = userOpt.value("yardage", QList<QVariant>() << 435 << 529 << 198 << 517 << 451 << 408 << 178 << 457 << 414 << 495 << 505 << 155 << 614 << 435 << 478 << 381 << 455 << 357).toList();
			QList<QVariant> par = userOpt.value("par", QList<QVariant>() << 4 << 5 << 3 << 5 << 4 << 4 << 3 << 4 << 4 << 4 << 4 << 3 << 5 << 4 << 4 << 4 << 4 << 4).toList();
			QList<QVariant> differential = userOpt.value("differential", QList<QVariant>() << 0.19 << -0.36 << 0.15 << 0 << 0.23 << 0.19 << 0.24 << 0.13 << 0.18 << 0.23 << 0.32 << 0.3 << -0.14 << 0.25 << 0.35 << 0.19 << 0.63 << -0.46).toList();
			QList<QVariant> sand = userOpt.value("sand", QList<QVariant>() << 1 << 1 << 1 << 0 << 1 << 0 << 1 << 1 << 1 << 0 << 0 << 0 << 1 << 1 << 0 << 0 << 1 << 0).toList();
			QList<QVariant> water = userOpt.value("water", QList<QVariant>() << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 1 << 1 << 0 << 0 << 0 << 0 << 0 << 0).toList();
			QList<QVariant> narrow = userOpt.value("narrow", QList<QVariant>() << 0 << 1 << 0 << 0 << 1 << 1 << 0 << 1 << 0 << 1 << 0 << 1 << 0 << 1 << 1 << 0 << 0 << 0).toList();
			QList<QVariant> green = userOpt.value("green", QList<QVariant>() << 0 << 0 << 1 << 1 << 1 << 0 << 1 << 0 << 1 << 0 << 0 << 0 << 1 << 1 << 0 << 1 << 0 << 0).toList();

			// initialize results
			out.clear();
			res.clear();

			for(QList<XkorAthlete>::iterator athlete = athletes.begin(); athlete != athletes.end(); ++athlete)
			{
				XkorResult r;
				r.athlete = *athlete;

				// get athlete’s style
				QString style = r.athlete.property("style").toString();
				int length = style.mid(0, 1).toInt();
				int accuracy = style.mid(1, 1).toInt();
				int longIrons = style.mid(2, 1).toInt();
				int shortIrons = style.mid(3, 1).toInt();
				int wedges = style.mid(4, 1).toInt();
				int putting = style.mid(5, 1).toInt();

				QList<QVariant> attempts;
				int totalScore = 0;
				for(int i = 0; i < holes; ++i)
				{
					// hole parameters
					int holePar = par[i].toInt();
					double holeYards = yardage[i].toDouble() / 800.0;
					double holeDifferential = differential[i].toDouble();
					double longIronMultiplier = 1 + water[i].toInt() + narrow[i].toInt();
					double shortIronMultiplier = 1 + narrow[i].toInt() + green[i].toInt();
					double wedgeMultiplier = 1 + sand[i].toInt() + green[i].toInt();

					// calculate skill for this hole
					double holeSkill = 6.5 + (longIronMultiplier * longIrons + shortIronMultiplier * shortIrons + wedgeMultiplier * wedges) / (longIronMultiplier + shortIronMultiplier + wedgeMultiplier);

					// rpSkill * 2 because the original formula uses rank + RP
					double magicNumber = r.athlete.rpSkill * 2 + (6.5 + length * holeYards + accuracy * (1 - holeYards) + holeSkill * (holePar - 2.5)) / (holePar - 1.5);
					magicNumber = magicNumber * 0.06 + 0.02;
					double puttingNumber = 0.66 - 0.04 * putting;

					double rand1 = s->randUniform();
					double rand2 = s->randUniform();
					double rand3 = s->randUniform();

					int shotsToGreen = 0, putts = 0, adjustment = 0;
					if(rand1 < 0.02)
						shotsToGreen = holePar - 3;
					else if(rand1 > 0.98)
						shotsToGreen = holePar;
					else if(rand1 < magicNumber)
						shotsToGreen = holePar - 2;
					else
						shotsToGreen = holePar - 1;

					if(rand2 < puttingNumber)
						putts = 2;
					else if(rand2 > 0.95)
						putts = 3;
					else
						putts = 1;

					if(rand3 < abs(holeDifferential))
						adjustment = holeDifferential > 0 ? 1 : -1;

					int score = shotsToGreen + putts + adjustment;
					if(score == 0)
						++score; // you can’t finish the hole in zero strokes!

					attempts.push_back(score);
					totalScore += score;
				}
				r.result["attempts"] = attempts;
				r.setScore(totalScore);
				r.setOutput(outputLine(r, nameWidth, resultWidth));
				res.append(r);
			}
			generateOutput();
		}

	protected:
		virtual XkorResult individualResult(double, double) { return XkorResult(); } // unused
		virtual XkorResult individualResult(double, double, QString) { return XkorResult(); } // unused

		virtual QString outputLine(XkorResult r, double nameWidth, double resultWidth)
		{
			QString rval = formatName(r.athlete).leftJustified(nameWidth);
			QList<QVariant> attempts = r.value("attempts").toList();

			for(int i = 0; i < attempts.size(); ++i)
				rval += attempts[i].toString().rightJustified(resultWidth);
			rval += QString::number(r.score()).rightJustified(resultWidth + 3);
			return rval;
		}
};

#endif // GOLFINATORPARADIGM_H
