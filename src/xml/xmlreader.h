#ifndef XMLREADER_H
#define XMLREADER_H

#include <QtXml>
#include "event.h"
#include "rplist.h"
#include "signuplist.h"

class XkorXmlReader : public QXmlStreamReader
{
	public:
		XkorXmlReader(QString filename);

		QString error() const;
		QList<QPair<QUuid, XkorEvent *> > events() const { return m_events; }
		double readDouble();
		void readEvent();
		XkorSignupList readEventSignupList();
		void readFile();
		XkorGroup readGroup();
		int readInt();
		QList<QVariant> readList();
		QPair<QString, QHash<QString, double> > readNation();
		QHash<QString, QVariant> readOptions();
		QHash<int, QString> readResults();
		void readRPList();
		XkorAthlete readSignup();
		QString readString();
		void readUnknownElement();
		XkorRPList * rpList() const { return m_rpList; }
	private:
		QList<QPair<QUuid, XkorEvent *> > m_events;
		XkorRPList * m_rpList;
};

#endif // XMLREADER_H
