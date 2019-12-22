#ifndef XMLTABLEREADER_H
#define XMLTABLEREADER_H

#include <QtXml>
#include "tablegenerator/table.h"

class XkorXmlTableReader : public QXmlStreamReader
{
	public:
		XkorXmlTableReader(QString filename);

		QString error() const;
		QString matches() const { return m_matches; }
		double readDouble();
		void readFile();
		int readInt();
		void readMatches();
		std::vector<QString> readSortCriteria();
		QString readString();
		void readUnknownElement();
		XkorTable table() const { return m_table; }
	private:
		XkorTable m_table;
		QString m_matches;
};

#endif // XMLTABLEREADER_H
