#ifndef PARADIGMFACTORY_H
#define PARADIGMFACTORY_H

#include "paradigms/abstractparadigm.h"

class XkorParadigmFactory
{
	public:
		static XkorAbstractParadigm * newParadigm(const QString type);
		static XkorAbstractParadigm * newParadigm(XkorSport * s, QHash<QString, QVariant> userOpts);
};

#endif
