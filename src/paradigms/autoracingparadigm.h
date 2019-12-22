#ifndef AUTORACINGPARADIGM_H
#define AUTORACINGPARADIGM_H

#include <QFormLayout>
#include <QSpinBox>

#include "paradigms/timedparadigm.h"
#include "paradigms/options/autoracingparadigmoptions.h"

struct XkorAutoRacingParadigm : public XkorTimedParadigm
{
	Q_OBJECT

	public:
		XkorAutoRacingParadigm() : XkorTimedParadigm()
		{
			supportedCompetitions.clear();
			supportedCompetitions.insert("standard", true);
		}

		XkorAutoRacingParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorTimedParadigm(sport, userOptions)
		{
			supportedCompetitions.clear();
			supportedCompetitions.insert("standard", true);
		}

		virtual XkorAbstractAthleteWidget * newAthleteWidget()
		{
			if(userOpt.value("skillType") == "attributes")
				return new XkorAthleteWidget(QStringList() << "name" << "nation" << "acceleration" << "cornering" << "reliability",
							     QStringList() << tr("Participant") << tr("Team") << tr("Acceleration") << tr("Cornering") << tr("Reliability"),
							     QStringList() << "string" << "string" << "double" << "double" << "double",
							     0, 10, 1);
			else
				return XkorAbstractParadigm::newAthleteWidget();
		}

		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> paradigmOptions)
		{
			return new XkorAutoRacingParadigmOptions(paradigmOptions);
		}

	protected:

		virtual XkorResult individualResult(XkorAthlete ath, int lap = 0)
		{
			double randCoeff = opt.value("randCoeff", 0.6).toDouble();
			double randOffset = opt.value("randBase", 0.6).toDouble();
			double skillCoeff = opt.value("skillCoeff", 0.3).toDouble();
			double reliabilityCoeff = opt.value("reliabilityCoeff", 0.03).toDouble();

			double lapRecord = userOpt.value("lapRecord", 90).toDouble();
			double lapVariance = userOpt.value("lapVariance", 15).toDouble();
			double trackAcceleration = userOpt.value("trackAcceleration", 5).toDouble();
			double trackCornering = userOpt.value("trackCornering", 5).toDouble();
			bool useAttributes = userOpt.value("skillType") == "attributes";

			double reliability = (useAttributes ? ath.property("reliability").toDouble() : ath.skill * 10);
			if(s->randUniform() < reliabilityCoeff / (1 + reliability))
				return XkorResult(numeric_limits<double>::max(), opt.value("unreliableText", "DNF").toString() + QString(" lap %1").arg(lap), ath);

			double lapTime = 0;
			if(useAttributes)
			{
				double acceleration = ath.property("acceleration").toDouble(), cornering = ath.property("cornering").toDouble();
				lapTime = lapRecord + lapVariance * pow(s->randUniform() * randCoeff + randOffset, skillCoeff * (acceleration * trackAcceleration + cornering * trackCornering) / (trackAcceleration + trackCornering));
			}
			else
				lapTime = lapRecord + lapVariance * pow(s->randUniform() * randCoeff + randOffset, skillCoeff * (ath.skill * 9 + 1));

			lapTime = roundScore(lapTime);
			return XkorResult(lapTime, formatScore(lapTime), ath);
		}

		virtual QString outputLine(XkorResult r)
		{
			return XkorAbstractParadigm::outputLine(r);
		}

		virtual void scorinate(QList<XkorAthlete> athletes, QList<XkorResult> = QList<XkorResult>())
		{
			// initialize results
			out.clear();
			res.clear();

			int laps = userOpt.value("laps", 50).toInt();
			bool isQualifying = opt.value("totalType") == "best";
			double startingGridMultiplier = (userOpt.value("useStartingGrid") == "true" ? opt.value("startingGridPenalty", 2).toDouble() : 0);

			double startingGridPenalty = startingGridMultiplier;
			for(QList<XkorAthlete>::iterator i = athletes.begin(); i != athletes.end(); ++i)
			{
				XkorResult r;
				r.athlete = *i;

				for(int j = 0; j < laps; ++j)
				{
					XkorResult result = individualResult(*i, j + 1);

					QList<QVariant> attempts = r.result["attempts"].toList(), attemptStrings = r.result["attemptStrings"].toList();
					attempts.append(result.score() + (j == 0 ? startingGridPenalty : 0));
					if(j == 0)
						qDebug() << startingGridPenalty;
					attemptStrings.append(result.scoreString());
					r.result["attempts"] = attempts;
					r.result["attemptStrings"] = attemptStrings;

					if(!isQualifying && result.score() == numeric_limits<double>::max())
						break; // did not finish
				}

				calculateTotal(r);

				res.append(r);
				startingGridPenalty += startingGridMultiplier;
			}

			// who’s on first?
			double firstPlaceTime = numeric_limits<double>::max();
			for(QVector<XkorResult>::iterator i = res.begin(); i != res.end(); ++i)
				if(i->score() < firstPlaceTime)
					firstPlaceTime = i->score();

			// calculate laps behind
			for(QVector<XkorResult>::iterator i = res.begin(); i != res.end(); ++i)
			{
				QList<QVariant> attempts = i->result["attempts"].toList();
				double currentTime = i->score();
				if(currentTime != numeric_limits<double>::max() && currentTime > firstPlaceTime)
					for(int j = 0; attempts.count() > 0; ++j)
					{
						i->setScore(currentTime + firstPlaceTime * j); // set the score according to finish on the current lap
						currentTime -= attempts.takeLast().toDouble();
						if(currentTime < firstPlaceTime)
						{
							// if j == 0, do nothing
							if(j == 1)
								i->setScoreString("−1 lap");
							else if(j > 1)
								i->setScoreString(QString("−%1 laps").arg(j));
							i->setOutput(outputLine(*i));
							break;
						}
					}
			}

			generateOutput();
		}
};

#endif // AUTORACINGPARADIGM_H
