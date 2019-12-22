#ifndef XMLINDEX_H
#define XMLINDEX_H

#include <map>
#include <QString>

#include "exceptions.h"

class XkorXmlIndex
{
	private:
		std::map<QString, QString> index;
		void insert(QString filename);
	public:
		void traverse(QString dir) throw(XkorFileNotFoundException);
		QString lookup(QString name) throw(XkorSearchFailedException);
		std::map<QString, QString> getAllFiles();
};

#endif
