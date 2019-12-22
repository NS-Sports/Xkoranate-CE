#ifndef PROGRESSIVETHROWPARADIGM_H
#define PROGRESSIVETHROWPARADIGM_H

#include <QFormLayout>
#include <QSpinBox>

#include "paradigms/scoredparadigm.h"

struct XkorProgressiveThrowParadigm : public XkorScoredParadigm
{
	Q_OBJECT

	public:
		XkorProgressiveThrowParadigm() : XkorScoredParadigm() {;;;}
		XkorProgressiveThrowParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorScoredParadigm(sport, userOptions) {;;;}

	protected:
		virtual void scorinate(QList<XkorAthlete> athletes, QList<XkorResult> previousResults = QList<XkorResult>())
		{
			// check that we can use the sport
			for(QStringList::iterator i = requiredValues.begin(); i != requiredValues.end(); ++i)
				if(!(s->hasValue(*i)))
				{
					qDebug() << "missing parameter" << *i << "in XkorProgressiveThrowParadigm::scorinate(QList<XkorAthlete>)";
					out.append(qMakePair(QString(""), QString("Sport does not support this paradigm")));
					return;
				}

			// initialize results
			out.clear();
			res.clear();

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
					XkorResult result = individualResult(*i, "attempts");
					XkorResult accuracyResult = individualResult(*i, "accuracy", "accuracy");
					result.setScoreString(QString::number((int)(accuracyResult.score() * result.score())) + "/" + result.scoreString());
					result.setScore((int)(accuracyResult.score() * result.score()));

					QList<QVariant> attempts = r.result["attempts"].toList(), attemptStrings = r.result["attemptStrings"].toList();
					attempts.append(result.score());
					attemptStrings.append(result.scoreString());
					r.result["attempts"] = attempts;
					r.result["attemptStrings"] = attemptStrings;
					if(opt.value("totalType").toString() != "best" && (result.score() == numeric_limits<double>::max() || result.score() == -numeric_limits<double>::max()))
						break;
				}
				calculateTotal(r);

				res.append(r);
			}

			generateOutput();
		}
};

#endif // PROGRESSIVETHROWPARADIGM_H
