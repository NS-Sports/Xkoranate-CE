#ifndef XMLWRITER_H
#define XMLWRITER_H

#include <QtXml>
#include "event.h"
#include "rplist.h"
#include "signuplist.h"

class XkorXmlWriter : public QXmlStreamWriter
{
	public:
		XkorXmlWriter(QString filename, XkorRPList * rpList, QList<QPair<QUuid, XkorEvent *> > events);

		void writeEvents(QList<QPair<QUuid, XkorEvent *> > events);
		void writeRPList(XkorRPList * rpLists);
		void writeVariant(QVariant value, QString key = QString());
};

#endif // XMLWRITER_H
