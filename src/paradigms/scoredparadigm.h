#ifndef SCOREDPARADIGM_H
#define SCOREDPARADIGM_H

#include <QFormLayout>
#include <QSpinBox>

#include "paradigms/timedparadigm.h"
#include "paradigms/options/timedparadigmoptions.h"

struct XkorScoredParadigm : public XkorTimedParadigm
{
	Q_OBJECT

	public:
		XkorScoredParadigm() : XkorTimedParadigm() {;;;}

		XkorScoredParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorTimedParadigm(sport, userOptions) {;;;}

	private:

		QString formatScore(double score)
		{
			return QString::number(score, 'f', opt.value("displayDigits").toInt());
		}
};

#endif // SCOREDPARADIGM_H
