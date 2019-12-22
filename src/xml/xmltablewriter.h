#ifndef XMLTABLEWRITER_H
#define XMLTABLEWRITER_H

#include <QtXml>
#include "tablegenerator/table.h"

class XkorXmlTableWriter : public QXmlStreamWriter
{
	public:
		XkorXmlTableWriter(QString filename, XkorTable t);

		void writeTable(XkorTable t);
};

#endif // XMLTABLEWRITER_H
