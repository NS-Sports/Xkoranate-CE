#ifndef XMLSPORTREADER_H
#define XMLSPORTREADER_H

#include <QtXml>
#include "sport.h"

class XkorXmlSportReader : public QXmlStreamReader
{
	public:
		XkorXmlSportReader(QString filename);

		QString error() const;
		QMap<double, double> readDataPoints();
		double readDouble();
		void readFile();
		int readInt();
		QList<QVariant> readList();
		QHash<QString, QVariant> readOptions();
		QString readString();
		void readUnknownElement();
		XkorSport sport() const { return m_sport; }
	private:
		XkorSport m_sport;
        QString m_filename;
};

#endif // XMLSPORTREADER_H
