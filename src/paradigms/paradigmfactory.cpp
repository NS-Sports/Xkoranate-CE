#include "paradigmfactory.h"

#include "paradigms/archeryparadigm.h"
#include "paradigms/autoracingparadigm.h"
#include "paradigms/bestofparadigm.h"
#include "paradigms/eliminationraceparadigm.h"
#include "paradigms/fencingparadigm.h"
#include "paradigms/footba11erparadigm.h"
#include "paradigms/golfinatorparadigm.h"
#include "paradigms/h2hparadigm.h"
#include "paradigms/highjumpparadigm.h"
#include "paradigms/howzzatparadigm.h"
#include "paradigms/nsfsparadigm.h"
#include "paradigms/nsfsbaseballparadigm.h"
#include "paradigms/nsfsgridironparadigm.h"
#include "paradigms/ordinalparadigm.h"
#include "paradigms/pgsparadigm.h"
#include "paradigms/pointsraceparadigm.h"
#include "paradigms/progressivethrowparadigm.h"
#include "paradigms/scoredparadigm.h"
#include "paradigms/shootingparadigm.h"
#include "paradigms/shorttrackparadigm.h"
#include "paradigms/tennisparadigm.h"
#include "paradigms/timedparadigm.h"
#include "paradigms/sqisparadigm.h"
#include "paradigms/wrestlingparadigm.h"

XkorAbstractParadigm * XkorParadigmFactory::newParadigm(const QString type)
{
	XkorAbstractParadigm * rval;
	if(type == "archery")
		rval = new XkorArcheryParadigm;
	else if(type == "autoracing")
		rval = new XkorAutoRacingParadigm;
	else if(type == "bestof")
		rval = new XkorBestOfParadigm;
	else if(type == "eliminationrace")
		rval = new XkorEliminationRaceParadigm;
	else if(type == "fencing")
		rval = new XkorFencingParadigm;
	else if(type == "footba11er")
		rval = new XkorFootba11erParadigm;
	else if(type == "golfinator")
		rval = new XkorGolfinatorParadigm;
	else if(type == "highjump")
		rval = new XkorHighJumpParadigm;
	else if(type == "howzzat")
		rval = new XkorHowzzatParadigm;
	else if(type == "nsfs")
		rval = new XkorNSFSParadigm;
	else if(type == "nsfs-baseball")
		rval = new XkorNSFSBaseballParadigm;
	else if(type == "nsfs-gridiron")
		rval = new XkorNSFSGridironParadigm;
	else if(type == "ordinal")
		rval = new XkorOrdinalParadigm;
	else if(type == "pgs")
		rval = new XkorPGSParadigm;
	else if(type == "pointsrace")
		rval = new XkorPointsRaceParadigm;
	else if(type == "progressivethrow")
		rval = new XkorProgressiveThrowParadigm;
	else if(type == "scored")
		rval = new XkorScoredParadigm;
	else if(type == "shooting")
		rval = new XkorShootingParadigm;
	else if(type == "shorttrack")
		rval = new XkorShortTrackParadigm;
	else if(type == "sqis")
		rval = new XkorSQISParadigm;
	else if(type == "tennis")
		rval = new XkorTennisParadigm;
	else if(type == "timed")
		rval = new XkorTimedParadigm;
	else if(type == "wrestling")
		rval = new XkorWrestlingParadigm;
	else if(type == "xkoranate-h2h")
		rval = new XkorH2HParadigm;
	else
		rval = new XkorTimedParadigm;
	return rval;
}

XkorAbstractParadigm * XkorParadigmFactory::newParadigm(XkorSport * s, QHash<QString, QVariant> userOpts)
{
	QString type = s->paradigm();
	XkorAbstractParadigm * rval = newParadigm(type);
	rval->init(s, userOpts);
	return rval;
}
