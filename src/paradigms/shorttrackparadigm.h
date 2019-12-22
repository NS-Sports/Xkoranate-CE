#ifndef SHORTTRACKPARADIGM_H
#define SHORTTRACKPARADIGM_H

#include "timedparadigm.h"
#include "paradigms/options/shorttrackparadigmoptions.h"

class XkorShortTrackParadigm : public XkorTimedParadigm
{
	public:
		XkorShortTrackParadigm() : XkorTimedParadigm()
		{
			supportedCompetitions.clear();
			supportedCompetitions.insert("standard", true);
		}

		XkorShortTrackParadigm(XkorSport * sport, QHash<QString, QVariant> userOptions) : XkorTimedParadigm(sport, userOptions)
		{
			supportedCompetitions.clear();
			supportedCompetitions.insert("standard", true);
		}

		virtual XkorAbstractOptionsWidget * newOptionsWidget(QHash<QString, QVariant> paradigmOptions)
		{
			return new XkorShortTrackParadigmOptions(paradigmOptions);
		}
};

#endif // SHORTTRACKPARADIGM_H
